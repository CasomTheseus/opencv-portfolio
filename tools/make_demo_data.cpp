// Reproducible demo data generator.
//
//   --what labels  : label template + synthetic scenes (OK / OK / missing)
//   --what defect  : synthetic part array with three known defects
//   --what video   : synthetic conveyor video for the analytics demo
//   --what all     : everything above
//
// All images are generated with OpenCV only: no external assets, no personal
// data, and the ground truth is known.

#include "portfolio/common.hpp"

namespace fs = std::filesystem;
using cv::Mat;
using cv::Point;
using cv::Point2f;
using cv::Rect;
using cv::Scalar;
using cv::Size;
using std::vector;

namespace {

struct Options {
    std::string what = "all";
    fs::path out{"assets"};
    int width = 640;
    int height = 480;
    int fps = 30;
    int frames = 240;
};

void usage() {
    std::cout
        << "Usage: make_demo_data [--what labels|defect|video|all] [--out <dir>] [options]\n\n"
        << "  --what <kind>    labels | defect | video | all (default: all)\n"
        << "  --out <dir>      Output root (default: assets)\n"
        << "  --width <px>     Video width (default: 640)\n"
        << "  --height <px>    Video height (default: 480)\n"
        << "  --fps <n>        Video fps (default: 30)\n"
        << "  --frames <n>     Video frame count (default: 240)\n"
        << "  --help           Show this help\n";
}

void putTextShadow(Mat& image, const std::string& text, Point origin, double scale,
                   Scalar color, int thickness = 2) {
    cv::putText(image, text, origin, cv::FONT_HERSHEY_SIMPLEX, scale, Scalar(20, 20, 20),
                thickness + 2, cv::LINE_AA);
    cv::putText(image, text, origin, cv::FONT_HERSHEY_SIMPLEX, scale, color, thickness,
                cv::LINE_AA);
}

Mat makeLabelTemplate() {
    const int w = 720;
    const int h = 440;
    Mat image(h, w, CV_8UC3, Scalar(248, 248, 242));

    // Header band.
    cv::rectangle(image, Rect(0, 0, w, 86), Scalar(150, 82, 38), cv::FILLED);
    cv::putText(image, "OPENCV PORTFOLIO", Point(28, 44), cv::FONT_HERSHEY_DUPLEX, 1.0,
                Scalar(255, 255, 255), 2, cv::LINE_AA);
    cv::putText(image, "INDUSTRIAL LABEL / DEMO DATA", Point(28, 72),
                cv::FONT_HERSHEY_SIMPLEX, 0.55, Scalar(235, 235, 235), 1, cv::LINE_AA);

    // Border and text block.
    cv::rectangle(image, Rect(5, 5, w - 10, h - 10), Scalar(45, 45, 45), 3);
    putTextShadow(image, "LOT: 2026-10", Point(36, 190), 0.9, Scalar(40, 40, 40));
    putTextShadow(image, "SN:  CV-0001", Point(36, 235), 0.9, Scalar(40, 40, 40));
    putTextShadow(image, "QTY: 12 PCS", Point(36, 280), 0.9, Scalar(40, 40, 40));
    putTextShadow(image, "MADE FOR DEMO", Point(36, 318), 0.62, Scalar(120, 70, 30));

    // Barcode-like strip.
    cv::RNG rng(2026);
    int x = 92;
    while (x < 500) {
        const int barWidth = rng.uniform(2, 9);
        cv::rectangle(image, Rect(x, 340, barWidth, 76), Scalar(25, 25, 25), cv::FILLED);
        x += barWidth + rng.uniform(2, 7);
    }

    // QR-like block: high-frequency corners are excellent ORB features.
    const int cell = 8;
    const int qrSize = 20;
    const int qrX = 520;
    const int qrY = 118;
    cv::rectangle(image, Rect(qrX - 10, qrY - 10, qrSize * cell + 20, qrSize * cell + 20),
                  Scalar(245, 245, 245), cv::FILLED);
    for (int row = 0; row < qrSize; ++row) {
        for (int col = 0; col < qrSize; ++col) {
            if (rng.uniform(0, 2) == 0) {
                cv::rectangle(image,
                              Rect(qrX + col * cell, qrY + row * cell, cell, cell),
                              Scalar(20, 20, 20), cv::FILLED);
            }
        }
    }
    // Finder patterns.
    for (const Point& corner : {Point(qrX, qrY), Point(qrX + 10 * cell, qrY),
                                Point(qrX, qrY + 10 * cell)}) {
        cv::rectangle(image, Rect(corner.x, corner.y, 8 * cell, 8 * cell), Scalar(20, 20, 20),
                      cv::FILLED);
        cv::rectangle(image, Rect(corner.x + 2 * cell, corner.y + 2 * cell, 4 * cell, 4 * cell),
                      Scalar(245, 245, 245), cv::FILLED);
    }

    // Fiducial corners.
    for (const Point& corner : {Point(18, 104), Point(w - 58, 104), Point(18, h - 58),
                                Point(w - 58, h - 58)}) {
        cv::rectangle(image, Rect(corner.x, corner.y, 40, 40), Scalar(30, 30, 30), cv::FILLED);
        cv::rectangle(image, Rect(corner.x + 10, corner.y + 10, 20, 20), Scalar(230, 230, 230),
                      cv::FILLED);
    }

    // Small colour patch for scale/orientation cues.
    cv::circle(image, Point(650, 330), 26, Scalar(60, 160, 230), cv::FILLED, cv::LINE_AA);
    cv::line(image, Point(600, 380), Point(690, 380), Scalar(40, 120, 60), 5, cv::LINE_AA);

    return image;
}

Mat makeLabelScene(const Mat& templ, int variant, bool withLabel) {
    const int width = 1280;
    const int height = 960;
    Mat scene(height, width, CV_8UC3, Scalar(52, 56, 62));

    // Soft vertical gradient.
    for (int y = 0; y < height; ++y) {
        const int shade = static_cast<int>(18.0 * y / height);
        scene.row(y).setTo(Scalar(52 + shade, 56 + shade, 62 + shade));
    }

    // Clutter: deterministic rectangles and lines, all low-saturation so ORB
    // focuses on the label geometry.
    cv::RNG rng(9100 + variant);
    for (int i = 0; i < 70; ++i) {
        const int x = rng.uniform(-60, width);
        const int y = rng.uniform(-60, height);
        const int w = rng.uniform(40, 260);
        const int h = rng.uniform(20, 160);
        const int tone = rng.uniform(65, 115);
        const Scalar color(tone, tone + rng.uniform(-6, 7), tone + rng.uniform(-4, 9));
        cv::rectangle(scene, Rect(x, y, w, h), color, cv::FILLED);
    }
    for (int i = 0; i < 40; ++i) {
        const Point a(rng.uniform(0, width), rng.uniform(0, height));
        const Point b(a.x + rng.uniform(-160, 160), a.y + rng.uniform(-80, 80));
        cv::line(scene, a, b, Scalar(80, 84, 90), 1, cv::LINE_AA);
    }

    if (withLabel) {
        vector<Point2f> destination;
        if (variant == 0) {
            destination = {{310.0f, 210.0f}, {990.0f, 240.0f}, {960.0f, 700.0f}, {330.0f, 670.0f}};
        } else {
            destination = {{600.0f, 140.0f}, {1170.0f, 230.0f}, {1120.0f, 620.0f}, {620.0f, 540.0f}};
        }
        const vector<Point2f> source = {
            {0.0f, 0.0f},
            {static_cast<float>(templ.cols), 0.0f},
            {static_cast<float>(templ.cols), static_cast<float>(templ.rows)},
            {0.0f, static_cast<float>(templ.rows)}};

        const Mat homography = cv::getPerspectiveTransform(source, destination);
        Mat warped;
        Mat mask;
        cv::warpPerspective(templ, warped, homography, scene.size(), cv::INTER_LINEAR,
                            cv::BORDER_CONSTANT, Scalar(0, 0, 0));
        Mat maskSource(templ.size(), CV_8UC1, Scalar(255));
        cv::warpPerspective(maskSource, mask, homography, scene.size(),
                            cv::INTER_NEAREST, cv::BORDER_CONSTANT, Scalar(0));
        warped.copyTo(scene, mask);
    }

    cv::GaussianBlur(scene, scene, Size(3, 3), 0.0);
    Mat noise(scene.size(), CV_8UC3);
    cv::randu(noise, Scalar(0, 0, 0), Scalar(12, 12, 12));
    cv::add(scene, noise, scene);
    return scene;
}

void generateLabels(const fs::path& root) {
    const fs::path labelDir = root / "labels";
    portfolio::ensureDirectory(labelDir);

    const Mat templ = makeLabelTemplate();
    cv::imwrite((root / "label_template.png").string(), templ);

    const vector<int> jpegQuality = {cv::IMWRITE_JPEG_QUALITY, 95};
    cv::imwrite((labelDir / "label_ok_01.jpg").string(), makeLabelScene(templ, 0, true),
                jpegQuality);
    cv::imwrite((labelDir / "label_ok_02.jpg").string(), makeLabelScene(templ, 1, true),
                jpegQuality);
    cv::imwrite((labelDir / "label_missing.jpg").string(), makeLabelScene(templ, 2, false),
                jpegQuality);

    std::cout << "Labels written to " << root << "/ and " << labelDir << '\n';
}

void drawPart(Mat& canvas, const Rect& part, int defect) {
    // Shadow.
    cv::rectangle(canvas, Rect(part.x + 6, part.y + 8, part.width, part.height),
                  Scalar(24, 27, 30), cv::FILLED);
    // Body.
    cv::rectangle(canvas, part, Scalar(176, 180, 184), cv::FILLED);
    cv::rectangle(canvas, part, Scalar(120, 124, 128), 2);

    // Brushed-metal texture, identical for every part so the template matches.
    for (int y = part.y + 8; y < part.y + part.height - 8; y += 7) {
        cv::line(canvas, Point(part.x + 6, y), Point(part.x + part.width - 6, y),
                 Scalar(166, 170, 174), 1, cv::LINE_AA);
    }

    // Four bolt holes and a central pocket: the part's "identity".
    for (const Point& hole : {Point(part.x + 26, part.y + 26),
                              Point(part.x + part.width - 26, part.y + 26),
                              Point(part.x + 26, part.y + part.height - 26),
                              Point(part.x + part.width - 26, part.y + part.height - 26)}) {
        cv::circle(canvas, hole, 9, Scalar(36, 39, 43), cv::FILLED, cv::LINE_AA);
    }
    cv::rectangle(canvas, Rect(part.x + 55, part.y + 74, 120, 92), Scalar(36, 39, 43),
                  cv::FILLED);

    if (defect == 1) {
        // Chipped top-right corner.
        vector<Point> chip = {{part.x + part.width - 52, part.y},
                              {part.x + part.width, part.y},
                              {part.x + part.width, part.y + 52}};
        cv::fillPoly(canvas, vector<vector<Point>>{chip}, Scalar(32, 36, 40), cv::LINE_AA);
    } else if (defect == 2) {
        // Deep scratch across the surface.
        cv::line(canvas, Point(part.x + 34, part.y + 214), Point(part.x + 196, part.y + 126),
                 Scalar(58, 62, 66), 5, cv::LINE_AA);
    } else if (defect == 3) {
        // Missing bolt hole: fill it with body colour.
        cv::circle(canvas, Point(part.x + part.width - 26, part.y + part.height - 26), 9,
                   Scalar(176, 180, 184), cv::FILLED, cv::LINE_AA);
    }
}

void generateDefectParts(const fs::path& root) {
    const int width = 1280;
    const int height = 720;
    Mat canvas(height, width, CV_8UC3, Scalar(32, 36, 40));

    // Conveyor belt texture.
    for (int y = 0; y < height; y += 60) {
        cv::line(canvas, Point(0, y), Point(width, y), Scalar(40, 44, 48), 1);
    }

    const int partWidth = 230;
    const int partHeight = 240;
    const int xStep = 300;
    const int yStep = 380;

    // 0-based index -> defect code: 2 = chip, 5 = scratch, 7 = missing hole.
    const int defects[8] = {0, 0, 1, 0, 0, 2, 0, 3};
    for (int index = 0; index < 8; ++index) {
        const int row = index / 4;
        const int col = index % 4;
        const Rect part(60 + col * xStep, 50 + row * yStep, partWidth, partHeight);
        drawPart(canvas, part, defects[index]);
    }

    Mat noise(canvas.size(), CV_8UC3);
    cv::randu(noise, Scalar(0, 0, 0), Scalar(9, 9, 9));
    cv::add(canvas, noise, canvas);

    portfolio::ensureDirectory(root);
    cv::imwrite((root / "defect_parts.png").string(), canvas);

    std::ofstream truth(root / "defect_parts_ground_truth.csv");
    truth << "part,row,col,defect\n";
    for (int index = 0; index < 8; ++index) {
        const char* label = "OK";
        if (defects[index] == 1) label = "chip";
        if (defects[index] == 2) label = "scratch";
        if (defects[index] == 3) label = "missing_hole";
        truth << (index + 1) << ',' << (index / 4) << ',' << (index % 4) << ',' << label
              << '\n';
    }
    std::cout << "Defect image written to " << (root / "defect_parts.png") << '\n';
}

void generateVideo(const fs::path& root, const Options& opt) {
    const fs::path videoDir = root / "generated";
    portfolio::ensureDirectory(videoDir);
    const fs::path videoPath = videoDir / "demo_video.mp4";

    cv::VideoWriter writer;
    writer.open(videoPath.string(), cv::VideoWriter::fourcc('m', 'p', '4', 'v'),
                static_cast<double>(opt.fps), Size(opt.width, opt.height));
    if (!writer.isOpened()) {
        writer.open(videoPath.string(), cv::VideoWriter::fourcc('a', 'v', 'c', '1'),
                    static_cast<double>(opt.fps), Size(opt.width, opt.height));
    }
    if (!writer.isOpened()) {
        std::cerr << "Cannot open video writer for " << videoPath << '\n';
        return;
    }

    for (int frameIndex = 0; frameIndex < opt.frames; ++frameIndex) {
        Mat frame(opt.height, opt.width, CV_8UC3, Scalar(30, 34, 40));

        for (int x = 0; x < opt.width; x += 40) {
            cv::line(frame, Point(x, 0), Point(x, opt.height), Scalar(50, 55, 62), 1);
        }
        for (int y = 0; y < opt.height; y += 40) {
            cv::line(frame, Point(0, y), Point(opt.width, y), Scalar(50, 55, 62), 1);
        }
        cv::putText(frame, "SYNTHETIC DEMO", Point(opt.width - 170, opt.height - 18),
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, Scalar(95, 100, 110), 1, cv::LINE_AA);

        const double phase = 2.0 * CV_PI * frameIndex / 120.0;
        const int redX = 80 + static_cast<int>(480.0 * (0.5 + 0.5 * std::sin(phase)));
        const int redY = 160 + static_cast<int>(22.0 * std::sin(phase * 2.0));
        const int denom = std::max(1, opt.frames);
        const int greenX = 600 - (frameIndex * 560) / denom;
        const int greenY = 330;
        const int blueX = 40 + (frameIndex * 560) / denom;
        const int blueY = 235;

        cv::circle(frame, Point(redX, redY), 26, Scalar(40, 40, 230), cv::FILLED, cv::LINE_AA);
        cv::circle(frame, Point(greenX, greenY), 22, Scalar(50, 210, 60), cv::FILLED,
                   cv::LINE_AA);
        cv::rectangle(frame, Rect(blueX, blueY, 52, 52), Scalar(220, 120, 40), cv::FILLED);

        cv::putText(frame, "frame " + std::to_string(frameIndex), Point(18, opt.height - 18),
                    cv::FONT_HERSHEY_SIMPLEX, 0.55, Scalar(120, 125, 135), 1, cv::LINE_AA);

        Mat noise(frame.size(), CV_8UC3);
        cv::randu(noise, Scalar(0, 0, 0), Scalar(10, 10, 10));
        cv::add(frame, noise, frame);
        writer.write(frame);
    }
    writer.release();
    std::cout << "Video written to " << videoPath << " (" << opt.frames << " frames)\n";
}

}  // namespace

int main(int argc, char** argv) {
    const portfolio::Args args(argc, argv);
    if (args.has("help") || args.has("h")) {
        usage();
        return 0;
    }

    Options opt;
    opt.what = args.get("what", opt.what);
    opt.out = args.get("out", opt.out.string());
    opt.width = args.getInt("width", opt.width);
    opt.height = args.getInt("height", opt.height);
    opt.fps = args.getInt("fps", opt.fps);
    opt.frames = args.getInt("frames", opt.frames);

    if (opt.what != "labels" && opt.what != "defect" && opt.what != "video" &&
        opt.what != "all") {
        std::cerr << "Unknown --what: " << opt.what << '\n';
        usage();
        return 1;
    }
    if (opt.width < 160 || opt.height < 120 || opt.frames < 1 || opt.fps < 1) {
        std::cerr << "Invalid video parameters.\n";
        return 1;
    }

    if (opt.what == "labels" || opt.what == "all") generateLabels(opt.out);
    if (opt.what == "defect" || opt.what == "all") generateDefectParts(opt.out);
    if (opt.what == "video" || opt.what == "all") generateVideo(opt.out, opt);

    return 0;
}
