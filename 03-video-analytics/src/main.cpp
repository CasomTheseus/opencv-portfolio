// 03 - Video analytics toolkit.
//
// Three modes on the same input:
//   motion : MOG2 background subtraction + contour filtering
//   flow   : Farneback dense optical flow, HSV direction/magnitude view
//   track  : saturated-object tracking + virtual line crossing counter
//
// Every mode writes an annotated video and a per-frame CSV report.

#include "portfolio/common.hpp"

#include <deque>

namespace fs = std::filesystem;
using cv::Mat;
using cv::Point;
using cv::Point2f;
using cv::Rect;
using cv::Scalar;
using cv::Size;
using std::vector;

namespace {

enum class Mode { Motion, Flow, Track };

struct Options {
    std::string input;
    fs::path output{"outputs/video"};
    Mode mode = Mode::Motion;
    bool gui = false;
    bool noVideo = false;
    int maxFrames = 0;
};

void usage() {
    std::cout
        << "Usage: video_analytics --input <video|camera-index> [options]\n\n"
        << "  --input <path|index>  Video file or camera index, e.g. 0 (required)\n"
        << "  --mode <motion|flow|track>  Analysis mode (default: motion)\n"
        << "  --output <dir>        Output directory (default: outputs/video)\n"
        << "  --max-frames <n>      Stop after n frames\n"
        << "  --no-video            Do not write the annotated video\n"
        << "  --gui                 Show frames while processing\n"
        << "  --help                Show this help\n\n"
        << "Examples:\n"
        << "  ./build/bin/video_analytics --input assets/generated/demo_video.mp4 --mode motion\n"
        << "  ./build/bin/video_analytics --input assets/generated/demo_video.mp4 --mode track\n";
}

Mode parseMode(const std::string& text) {
    if (text == "flow") return Mode::Flow;
    if (text == "track") return Mode::Track;
    return Mode::Motion;
}

std::string modeName(Mode mode) {
    switch (mode) {
        case Mode::Flow:
            return "flow";
        case Mode::Track:
            return "track";
        case Mode::Motion:
        default:
            return "motion";
    }
}

bool isInteger(const std::string& text) {
    return !text.empty() &&
           std::all_of(text.begin(), text.end(), [](unsigned char c) { return std::isdigit(c); });
}

void drawHud(Mat& frame, const std::string& text) {
    cv::putText(frame, text, Point(18, 36), cv::FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 0, 0), 4,
                cv::LINE_AA);
    cv::putText(frame, text, Point(18, 36), cv::FONT_HERSHEY_SIMPLEX, 0.8, Scalar(255, 255, 255),
                1, cv::LINE_AA);
}

// ---------------------------------------------------------------------------
// Motion mode
// ---------------------------------------------------------------------------
Mat processMotion(const Mat& frame, cv::Ptr<cv::BackgroundSubtractorMOG2>& mog, int& objects,
                  int& foregroundArea) {
    Mat mask;
    mog->apply(frame, mask);
    cv::threshold(mask, mask, 200, 255, cv::THRESH_BINARY);
    foregroundArea = cv::countNonZero(mask);

    const Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, Size(5, 5));
    cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);
    cv::morphologyEx(mask, mask, cv::MORPH_DILATE, kernel);

    Mat annotated = frame.clone();
    vector<vector<Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    objects = 0;
    for (const auto& contour : contours) {
        if (cv::contourArea(contour) < 300.0) continue;
        const Rect box = cv::boundingRect(contour);
        cv::rectangle(annotated, box, Scalar(0, 255, 255), 2);
        ++objects;
    }
    drawHud(annotated, "MOG2  objects=" + std::to_string(objects));
    return annotated;
}

// ---------------------------------------------------------------------------
// Flow mode
// ---------------------------------------------------------------------------
Mat processFlow(const Mat& frame, Mat& previousGray, bool& hasPrevious, double& meanMagnitude,
                double& maxMagnitude) {
    Mat gray;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    Mat annotated = frame.clone();

    if (!hasPrevious) {
        gray.copyTo(previousGray);
        hasPrevious = true;
        meanMagnitude = 0.0;
        maxMagnitude = 0.0;
        drawHud(annotated, "Farneback  warming up");
        return annotated;
    }

    Mat flow;
    cv::calcOpticalFlowFarneback(previousGray, gray, flow, 0.5, 3, 15, 3, 5, 1.2, 0);

    vector<Mat> planes;
    cv::split(flow, planes);
    Mat magnitude;
    Mat angle;
    cv::cartToPolar(planes[0], planes[1], magnitude, angle);
    meanMagnitude = cv::mean(magnitude)[0];
    cv::minMaxLoc(magnitude, nullptr, &maxMagnitude);

    Mat hue;
    angle.convertTo(hue, CV_8U, 180.0 / (2.0 * CV_PI));
    Mat value;
    cv::normalize(magnitude, value, 0, 255, cv::NORM_MINMAX, CV_8U);
    Mat saturation(frame.size(), CV_8U, Scalar(255));
    vector<Mat> channels{hue, saturation, value};
    Mat hsv;
    cv::merge(channels, hsv);
    Mat flowBgr;
    cv::cvtColor(hsv, flowBgr, cv::COLOR_HSV2BGR);

    Mat blended;
    cv::addWeighted(frame, 0.35, flowBgr, 0.65, 0, blended);
    drawHud(blended, "Farneback  mean=" + portfolio::fixed(meanMagnitude, 2) +
                         "  max=" + portfolio::fixed(maxMagnitude, 1));

    gray.copyTo(previousGray);
    return blended;
}

// ---------------------------------------------------------------------------
// Track mode
// ---------------------------------------------------------------------------
struct Track {
    int id = 0;
    Point2f center;
    bool rightSide = false;
    int lost = 0;
    int crossings = 0;
    std::deque<Point> trail;
};

Mat processTrack(const Mat& frame, vector<Track>& tracks, int& nextId, int& totalCrossings,
                 double lineX) {
    Mat hsv;
    cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);
    Mat mask;
    cv::inRange(hsv, Scalar(0, 90, 70), Scalar(180, 255, 255), mask);
    const Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, Size(5, 5));
    cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);
    cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel);

    vector<vector<Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    vector<Point2f> detections;
    for (const auto& contour : contours) {
        if (cv::contourArea(contour) < 400.0) continue;
        const cv::Moments moments = cv::moments(contour);
        if (moments.m00 == 0.0) continue;
        detections.emplace_back(static_cast<float>(moments.m10 / moments.m00),
                                static_cast<float>(moments.m01 / moments.m00));
    }

    vector<bool> used(detections.size(), false);

    for (auto& track : tracks) {
        int bestIndex = -1;
        double bestDistance = 1e9;
        for (size_t i = 0; i < detections.size(); ++i) {
            if (used[i]) continue;
            const double distance = cv::norm(track.center - detections[i]);
            if (distance < bestDistance) {
                bestDistance = distance;
                bestIndex = static_cast<int>(i);
            }
        }

        if (bestIndex >= 0 && bestDistance <= 120.0) {
            used[bestIndex] = true;
            track.center = detections[bestIndex];
            track.lost = 0;
            track.trail.emplace_back(cvRound(track.center.x), cvRound(track.center.y));
            if (track.trail.size() > 30) track.trail.pop_front();

            const bool nowRight = track.center.x >= lineX;
            if (nowRight != track.rightSide && std::fabs(track.center.x - lineX) > 3.0) {
                track.rightSide = nowRight;
                ++track.crossings;
                ++totalCrossings;
            }
        } else {
            ++track.lost;
        }
    }

    for (size_t i = 0; i < detections.size(); ++i) {
        if (used[i]) continue;
        Track track;
        track.id = nextId++;
        track.center = detections[i];
        track.rightSide = detections[i].x >= lineX;
        track.trail.emplace_back(cvRound(track.center.x), cvRound(track.center.y));
        tracks.push_back(std::move(track));
    }

    tracks.erase(std::remove_if(tracks.begin(), tracks.end(),
                                [](const Track& track) { return track.lost > 15; }),
                 tracks.end());

    Mat annotated = frame.clone();
    cv::line(annotated, Point(cvRound(lineX), 0), Point(cvRound(lineX), annotated.rows),
             Scalar(0, 255, 255), 2, cv::LINE_AA);

    for (const auto& track : tracks) {
        for (const auto& point : track.trail) {
            cv::circle(annotated, point, 2, Scalar(255, 0, 255), cv::FILLED);
        }
        cv::circle(annotated, track.center, 7, Scalar(255, 0, 255), 2, cv::LINE_AA);
        cv::putText(annotated, "ID " + std::to_string(track.id),
                    track.center + Point2f(10.0f, -10.0f), cv::FONT_HERSHEY_SIMPLEX, 0.55,
                    Scalar(0, 0, 0), 3, cv::LINE_AA);
        cv::putText(annotated, "ID " + std::to_string(track.id),
                    track.center + Point2f(10.0f, -10.0f), cv::FONT_HERSHEY_SIMPLEX, 0.55,
                    Scalar(255, 255, 255), 1, cv::LINE_AA);
    }

    drawHud(annotated, "tracks=" + std::to_string(tracks.size()) +
                           "  crossings=" + std::to_string(totalCrossings));
    return annotated;
}

}  // namespace

int main(int argc, char** argv) {
    const portfolio::Args args(argc, argv);
    if (args.has("help") || args.has("h")) {
        usage();
        return 0;
    }

    Options opt;
    opt.input = args.get("input");
    opt.output = args.get("output", "outputs/video");
    opt.mode = parseMode(args.get("mode", "motion"));
    opt.gui = args.has("gui");
    opt.noVideo = args.has("no-video");
    opt.maxFrames = args.getInt("max-frames", 0);

    if (opt.input.empty()) {
        usage();
        return 1;
    }

    cv::VideoCapture capture;
    if (isInteger(opt.input)) {
        capture.open(std::stoi(opt.input));
    } else {
        capture.open(opt.input);
    }
    if (!capture.isOpened()) {
        std::cerr << "Cannot open video source: " << opt.input << '\n';
        return 1;
    }
    if (!portfolio::ensureDirectory(opt.output)) return 1;

    double fps = capture.get(cv::CAP_PROP_FPS);
    if (!(fps > 1.0 && fps < 240.0)) fps = 25.0;

    Mat firstFrame;
    capture.read(firstFrame);
    if (firstFrame.empty()) {
        std::cerr << "Video source returned no frames.\n";
        return 1;
    }

    cv::VideoWriter writer;
    if (!opt.noVideo) {
        const fs::path videoPath = opt.output / "annotated.mp4";
        writer.open(videoPath.string(), cv::VideoWriter::fourcc('m', 'p', '4', 'v'), fps,
                    firstFrame.size());
        if (!writer.isOpened()) {
            writer.open(videoPath.string(), cv::VideoWriter::fourcc('a', 'v', 'c', '1'), fps,
                        firstFrame.size());
        }
        if (writer.isOpened()) {
            std::cout << "Writing annotated video: " << videoPath << '\n';
        } else {
            std::cerr << "Warning: no video writer available, continuing with CSV only.\n";
        }
    }

    const fs::path csvPath = opt.output / "stats.csv";
    std::ofstream csv(csvPath);
    if (opt.mode == Mode::Motion) {
        csv << "frame,objects,foreground_area,elapsed_ms\n";
    } else if (opt.mode == Mode::Flow) {
        csv << "frame,mean_magnitude,max_magnitude,elapsed_ms\n";
    } else {
        csv << "frame,track_id,x,y,right_side,track_crossings,total_crossings,elapsed_ms\n";
    }

    std::cout << "Mode: " << modeName(opt.mode) << "  fps=" << portfolio::fixed(fps, 2)
              << "  size=" << firstFrame.cols << "x" << firstFrame.rows << '\n';

    cv::Ptr<cv::BackgroundSubtractorMOG2> mog =
        cv::createBackgroundSubtractorMOG2(500, 25.0, true);

    Mat previousGray;
    bool hasPrevious = false;

    vector<Track> tracks;
    int nextId = 1;
    int totalCrossings = 0;
    const double lineX = firstFrame.cols * 0.5;

    Mat frame = firstFrame;
    int frameIndex = 0;
    double totalMs = 0.0;

    while (!frame.empty()) {
        if (opt.maxFrames > 0 && frameIndex >= opt.maxFrames) break;

        portfolio::Stopwatch watch;
        Mat annotated;
        double meanMagnitude = 0.0;
        double maxMagnitude = 0.0;
        int motionObjects = 0;
        int motionForeground = 0;

        switch (opt.mode) {
            case Mode::Motion:
                annotated = processMotion(frame, mog, motionObjects, motionForeground);
                break;
            case Mode::Flow:
                annotated = processFlow(frame, previousGray, hasPrevious, meanMagnitude,
                                        maxMagnitude);
                break;
            case Mode::Track:
                annotated = processTrack(frame, tracks, nextId, totalCrossings, lineX);
                break;
        }

        const double elapsed = watch.ms();
        totalMs += elapsed;

        if (opt.mode == Mode::Motion) {
            csv << frameIndex << ',' << motionObjects << ',' << motionForeground << ','
                << portfolio::fixed(elapsed, 2) << '\n';
        } else if (opt.mode == Mode::Flow) {
            csv << frameIndex << ',' << portfolio::fixed(meanMagnitude, 3) << ','
                << portfolio::fixed(maxMagnitude, 3) << ',' << portfolio::fixed(elapsed, 2)
                << '\n';
        } else {
            if (tracks.empty()) {
                csv << frameIndex << ",-1,0,0,0,0," << totalCrossings << ','
                    << portfolio::fixed(elapsed, 2) << '\n';
            } else {
                for (const auto& track : tracks) {
                    csv << frameIndex << ',' << track.id << ',' << portfolio::fixed(track.center.x, 1)
                        << ',' << portfolio::fixed(track.center.y, 1) << ','
                        << (track.rightSide ? 1 : 0) << ',' << track.crossings << ','
                        << totalCrossings << ',' << portfolio::fixed(elapsed, 2) << '\n';
                }
            }
        }

        if (writer.isOpened()) writer.write(annotated);

        if (opt.gui) {
            cv::imshow("video analytics", annotated);
            const int key = cv::waitKey(1);
            if (key == 27) break;
        }

        ++frameIndex;
        if (!capture.read(frame)) break;
    }

    csv.close();
    if (writer.isOpened()) writer.release();
    capture.release();

    std::cout << "\n===== Video analytics summary =====\n"
              << "Mode        : " << modeName(opt.mode) << '\n'
              << "Frames      : " << frameIndex << '\n'
              << "Avg time    : "
              << portfolio::fixed(frameIndex > 0 ? totalMs / frameIndex : 0.0, 2)
              << " ms/frame\n";
    if (opt.mode == Mode::Track) {
        std::cout << "Crossings   : " << totalCrossings << '\n';
    }
    std::cout << "Report      : " << csvPath << '\n';
    if (!opt.noVideo) {
        std::cout << "Video       : " << (opt.output / "annotated.mp4") << '\n';
    }

    if (opt.gui) cv::destroyAllWindows();
    return 0;
}
