// 02 - Label localisation with ORB + RANSAC homography.
//
// Given a template label, find it in a scene, estimate the homography and
// report a confidence score. The same pipeline answers "is the label there?"
// (OK / NG) and produces a rectified crop for downstream OCR.

#include "portfolio/common.hpp"

namespace fs = std::filesystem;
using cv::DMatch;
using cv::KeyPoint;
using cv::Mat;
using cv::Point;
using cv::Point2f;
using cv::Rect;
using cv::Scalar;
using std::vector;

namespace {

struct Options {
    fs::path templatePath;
    fs::path input;
    fs::path output{"outputs/labels"};
    int maxFeatures = 2000;
    float ratio = 0.75f;
    double ransacThreshold = 4.0;
    int minInliers = 12;
    double minScore = 0.35;
    bool gui = false;
    int limit = 0;
};

void usage() {
    std::cout
        << "Usage: label_localization --template <label.png> --input <image|dir> [options]\n\n"
        << "  --template <path>     Template label image (required)\n"
        << "  --input <path>        Scene image or directory (required)\n"
        << "  --output <dir>        Output directory (default: outputs/labels)\n"
        << "  --min-inliers <n>     Minimum RANSAC inliers (default: 12)\n"
        << "  --min-score <0-1>     Minimum inlier ratio (default: 0.35)\n"
        << "  --ratio <0-1>         Lowe ratio test (default: 0.75)\n"
        << "  --ransac <px>         RANSAC reprojection threshold (default: 4.0)\n"
        << "  --max-features <n>    ORB feature budget (default: 2000)\n"
        << "  --limit <n>           Process at most n scenes\n"
        << "  --gui                 Show annotated scenes\n"
        << "  --help                Show this help\n\n"
        << "Example:\n"
        << "  ./build/bin/label_localization --template assets/label_template.png \\\n"
        << "      --input assets/labels --output outputs/labels\n";
}

struct MatchResult {
    bool ok = false;
    bool homographyValid = false;
    int goodMatches = 0;
    int inliers = 0;
    double inlierRatio = 0.0;
    double reprojError = 0.0;
    double quadAreaRatio = 0.0;
    Mat homography;
    Mat aligned;
    Mat annotated;
};

class OrbLocalizer {
public:
    bool init(const Mat& templ, const Options& opt, std::string& error) {
        templ_ = templ.clone();
        cv::cvtColor(templ_, templGray_, cv::COLOR_BGR2GRAY);
        orb_ = cv::ORB::create(opt.maxFeatures, 1.2f, 8, 31, 0, 2,
                               cv::ORB::HARRIS_SCORE, 31, 20);
        orb_->detectAndCompute(templGray_, cv::noArray(), templKeypoints_, templDescriptors_);
        if (templDescriptors_.empty()) {
            error = "Template has no ORB descriptors (is it blank or too small?)";
            return false;
        }
        return true;
    }

    MatchResult locate(const Mat& scene, const Options& opt) const {
        MatchResult result;
        result.annotated = scene.clone();

        Mat gray;
        cv::cvtColor(scene, gray, cv::COLOR_BGR2GRAY);

        vector<KeyPoint> keypoints;
        Mat descriptors;
        orb_->detectAndCompute(gray, cv::noArray(), keypoints, descriptors);
        if (descriptors.empty()) {
            drawNoLabel(result.annotated, "NO KEYPOINTS");
            return result;
        }

        cv::BFMatcher matcher(cv::NORM_HAMMING, false);
        vector<vector<DMatch>> knn;
        matcher.knnMatch(descriptors, templDescriptors_, knn, 2);

        vector<Point2f> scenePoints;
        vector<Point2f> templatePoints;
        for (const auto& pair : knn) {
            if (pair.size() < 2) continue;
            if (pair[0].distance < opt.ratio * pair[1].distance) {
                scenePoints.push_back(keypoints[pair[0].queryIdx].pt);
                templatePoints.push_back(templKeypoints_[pair[0].trainIdx].pt);
            }
        }
        result.goodMatches = static_cast<int>(scenePoints.size());

        if (scenePoints.size() < 6) {
            drawNoLabel(result.annotated, "NO LABEL");
            return result;
        }

        Mat inlierMask;
        result.homography = cv::findHomography(scenePoints, templatePoints, cv::RANSAC,
                                               opt.ransacThreshold, inlierMask);
        if (result.homography.empty() || inlierMask.empty()) {
            drawNoLabel(result.annotated, "HOMOGRAPHY FAILED");
            return result;
        }

        result.inliers = cv::countNonZero(inlierMask);
        result.inlierRatio =
            result.goodMatches > 0 ? static_cast<double>(result.inliers) / result.goodMatches : 0.0;

        // Mean reprojection error over the RANSAC inliers.
        double errorSum = 0.0;
        int errorCount = 0;
        for (size_t i = 0; i < scenePoints.size(); ++i) {
            if (inlierMask.at<uchar>(static_cast<int>(i)) == 0) continue;
            vector<Point2f> src{scenePoints[i]};
            vector<Point2f> dst;
            cv::perspectiveTransform(src, dst, result.homography);
            errorSum += cv::norm(dst[0] - templatePoints[i]);
            ++errorCount;
        }
        result.reprojError = errorCount > 0 ? errorSum / errorCount : 0.0;

        // Project the template corners back into the scene for drawing/validation.
        vector<Point2f> sceneCorners;
        Mat inverse;
        cv::invert(result.homography, inverse);
        if (!inverse.empty() && cv::checkRange(inverse)) {
            const vector<Point2f> templateCorners = {
                {0.0f, 0.0f},
                {static_cast<float>(templ_.cols), 0.0f},
                {static_cast<float>(templ_.cols), static_cast<float>(templ_.rows)},
                {0.0f, static_cast<float>(templ_.rows)}};
            cv::perspectiveTransform(templateCorners, sceneCorners, inverse);
            const double area = std::fabs(cv::contourArea(sceneCorners));
            result.quadAreaRatio = area / (static_cast<double>(scene.cols) * scene.rows);
            result.homographyValid = std::isfinite(result.quadAreaRatio) &&
                                     result.quadAreaRatio > 0.01 && result.quadAreaRatio < 0.95;
        }

        if (result.homographyValid) {
            cv::warpPerspective(scene, result.aligned, result.homography, templ_.size());
        }

        result.ok = result.homographyValid && result.inliers >= opt.minInliers &&
                    result.inlierRatio >= opt.minScore;

        const Scalar color = result.ok ? portfolio::okColor() : portfolio::warnColor();
        if (result.homographyValid) {
            vector<Point> polygon;
            polygon.reserve(sceneCorners.size());
            for (const auto& corner : sceneCorners) {
                polygon.emplace_back(cvRound(corner.x), cvRound(corner.y));
            }
            cv::polylines(result.annotated, polygon, true, color, 4, cv::LINE_AA);
            const Rect box = cv::boundingRect(polygon);
            const std::string text = std::string(result.ok ? "OK" : "NG") +
                                     "  inliers=" + std::to_string(result.inliers) +
                                     "  score=" + portfolio::fixed(result.inlierRatio, 2);
            portfolio::drawBadge(result.annotated, box, color, text, 0.65);
        } else {
            drawNoLabel(result.annotated, "NO LABEL");
        }

        return result;
    }

private:
    static void drawNoLabel(Mat& image, const std::string& text) {
        cv::putText(image, text, Point(24, 48), cv::FONT_HERSHEY_SIMPLEX, 1.1,
                    Scalar(0, 0, 0), 5, cv::LINE_AA);
        cv::putText(image, text, Point(24, 48), cv::FONT_HERSHEY_SIMPLEX, 1.1,
                    Scalar(0, 0, 255), 2, cv::LINE_AA);
    }

    Mat templ_;
    Mat templGray_;
    Mat templDescriptors_;
    vector<KeyPoint> templKeypoints_;
    cv::Ptr<cv::ORB> orb_;
};

}  // namespace

int main(int argc, char** argv) {
    const portfolio::Args args(argc, argv);
    if (args.has("help") || args.has("h")) {
        usage();
        return 0;
    }

    Options opt;
    opt.templatePath = args.get("template");
    opt.input = args.get("input");
    opt.output = args.get("output", "outputs/labels");
    opt.maxFeatures = args.getInt("max-features", opt.maxFeatures);
    opt.ratio = static_cast<float>(args.getDouble("ratio", opt.ratio));
    opt.ransacThreshold = args.getDouble("ransac", opt.ransacThreshold);
    opt.minInliers = args.getInt("min-inliers", opt.minInliers);
    opt.minScore = args.getDouble("min-score", opt.minScore);
    opt.gui = args.has("gui");
    opt.limit = args.getInt("limit", opt.limit);

    if (opt.templatePath.empty() || opt.input.empty()) {
        usage();
        return 1;
    }

    const Mat templateImage = cv::imread(opt.templatePath.string(), cv::IMREAD_COLOR);
    if (templateImage.empty()) {
        std::cerr << "Cannot read template: " << opt.templatePath << '\n';
        return 1;
    }

    OrbLocalizer localizer;
    std::string error;
    if (!localizer.init(templateImage, opt, error)) {
        std::cerr << error << '\n';
        return 1;
    }

    auto scenes = portfolio::collectImages(opt.input);
    if (scenes.empty()) {
        std::cerr << "No images found at: " << opt.input << '\n';
        return 1;
    }
    if (opt.limit > 0 && static_cast<int>(scenes.size()) > opt.limit) {
        scenes.resize(static_cast<size_t>(opt.limit));
    }
    if (!portfolio::ensureDirectory(opt.output)) return 1;

    std::ofstream csv(opt.output / "labels.csv");
    csv << "image,verdict,good_matches,inliers,inlier_ratio,reproj_error,quad_area_ratio,elapsed_ms\n";

    int okCount = 0;
    int ngCount = 0;
    double totalMs = 0.0;

    std::cout << "Template: " << opt.templatePath << " (" << templateImage.cols << " x "
              << templateImage.rows << ")\n";

    for (const auto& path : scenes) {
        const Mat scene = cv::imread(path.string(), cv::IMREAD_COLOR);
        if (scene.empty()) {
            std::cerr << "Cannot read image: " << path << '\n';
            continue;
        }

        portfolio::Stopwatch watch;
        const MatchResult result = localizer.locate(scene, opt);
        const double elapsed = watch.ms();
        totalMs += elapsed;

        if (result.ok) {
            ++okCount;
        } else {
            ++ngCount;
        }

        cv::imwrite((opt.output / (path.stem().string() + "_annotated.jpg")).string(),
                    result.annotated);
        if (!result.aligned.empty()) {
            cv::imwrite((opt.output / (path.stem().string() + "_aligned.png")).string(),
                        result.aligned);
        }

        csv << path.filename().string() << ',' << (result.ok ? "OK" : "NG") << ','
            << result.goodMatches << ',' << result.inliers << ','
            << portfolio::fixed(result.inlierRatio, 3) << ','
            << portfolio::fixed(result.reprojError, 2) << ','
            << portfolio::fixed(result.quadAreaRatio, 3) << ','
            << portfolio::fixed(elapsed, 2) << '\n';

        std::cout << path.filename().string() << ": " << (result.ok ? "OK" : "NG")
                  << "  matches=" << result.goodMatches << "  inliers=" << result.inliers
                  << "  ratio=" << portfolio::fixed(result.inlierRatio, 2)
                  << "  " << portfolio::fixed(elapsed, 1) << " ms\n";

        if (opt.gui) {
            cv::imshow("label localisation", result.annotated);
            cv::waitKey(1);
        }
    }

    csv.close();

    const int total = okCount + ngCount;
    std::cout << "\n===== Label localisation summary =====\n"
              << "Scenes      : " << total << '\n'
              << "OK / NG     : " << okCount << " / " << ngCount << '\n'
              << "Avg time    : "
              << portfolio::fixed(total > 0 ? totalMs / total : 0.0, 2) << " ms/image\n"
              << "Report      : " << (opt.output / "labels.csv") << '\n';

    if (opt.gui) {
        std::cout << "Press any key in the image window to exit.\n";
        cv::waitKey(0);
    }
    return 0;
}
