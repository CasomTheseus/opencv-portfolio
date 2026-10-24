// Small shared helpers for the three portfolio projects.
#pragma once

#include <opencv2/opencv.hpp>

#include <algorithm>
#include <chrono>
#include <cctype>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace portfolio {

namespace fs = std::filesystem;

// ---------------------------------------------------------------------------
// Minimal argument parser: --key value, --key=value, --flag
// Boolean flags must be listed in isFlagName(); everything else takes a value.
// ---------------------------------------------------------------------------
inline bool isFlagName(const std::string& key) {
    static const std::set<std::string> flags = {"gui", "help", "h", "verbose", "no-video"};
    return flags.count(key) > 0;
}

class Args {
public:
    Args(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            const std::string token = argv[i];
            if (token.rfind("--", 0) != 0) {
                positional_.push_back(token);
                continue;
            }
            std::string key = token.substr(2);
            std::string value;
            const auto eq = key.find('=');
            if (eq != std::string::npos) {
                value = key.substr(eq + 1);
                key = key.substr(0, eq);
            } else if (!isFlagName(key) && i + 1 < argc && std::string(argv[i + 1]).rfind("--", 0) != 0) {
                value = argv[++i];
            }
            if (value.empty()) {
                flags_.insert(key);
            } else {
                values_[key] = value;
            }
        }
    }

    bool has(const std::string& key) const {
        return values_.count(key) > 0 || flags_.count(key) > 0;
    }

    std::string get(const std::string& key, const std::string& fallback = "") const {
        const auto it = values_.find(key);
        return it == values_.end() ? fallback : it->second;
    }

    int getInt(const std::string& key, int fallback) const {
        const auto it = values_.find(key);
        if (it == values_.end()) return fallback;
        try {
            return std::stoi(it->second);
        } catch (...) {
            return fallback;
        }
    }

    double getDouble(const std::string& key, double fallback) const {
        const auto it = values_.find(key);
        if (it == values_.end()) return fallback;
        try {
            return std::stod(it->second);
        } catch (...) {
            return fallback;
        }
    }

    const std::vector<std::string>& positional() const { return positional_; }

private:
    std::map<std::string, std::string> values_;
    std::set<std::string> flags_;
    std::vector<std::string> positional_;
};

// ---------------------------------------------------------------------------
// Filesystem / timing / drawing helpers
// ---------------------------------------------------------------------------
inline bool ensureDirectory(const fs::path& dir) {
    std::error_code ec;
    fs::create_directories(dir, ec);
    if (ec) {
        std::cerr << "Cannot create directory " << dir << ": " << ec.message() << '\n';
        return false;
    }
    return true;
}

inline std::vector<fs::path> collectImages(const fs::path& input) {
    std::vector<fs::path> files;
    std::error_code ec;
    if (fs::is_regular_file(input, ec)) {
        files.push_back(input);
        return files;
    }
    if (!fs::is_directory(input, ec)) {
        return files;
    }
    for (const auto& entry : fs::directory_iterator(input, ec)) {
        if (!entry.is_regular_file()) continue;
        std::string ext = entry.path().extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        if (ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".bmp" || ext == ".tif" || ext == ".tiff") {
            files.push_back(entry.path());
        }
    }
    std::sort(files.begin(), files.end());
    return files;
}

inline std::string lowerExtension(const fs::path& path) {
    std::string ext = path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return ext;
}

class Stopwatch {
public:
    Stopwatch() : start_(std::chrono::steady_clock::now()) {}

    void reset() { start_ = std::chrono::steady_clock::now(); }

    double ms() const {
        const auto now = std::chrono::steady_clock::now();
        return std::chrono::duration<double, std::milli>(now - start_).count();
    }

private:
    std::chrono::steady_clock::time_point start_;
};

inline std::string fixed(double value, int precision = 2) {
    std::ostringstream os;
    os << std::fixed << std::setprecision(precision) << value;
    return os.str();
}

inline void drawBadge(cv::Mat& image, const cv::Rect& box, const cv::Scalar& color,
                      const std::string& text, double scale = 0.55) {
    int baseline = 0;
    const cv::Size textSize =
        cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, scale, 1, &baseline);
    int x = std::clamp(box.x, 0, std::max(0, image.cols - textSize.width - 12));
    int y = box.y - textSize.height - 10;
    if (y < 0) y = box.y + box.height + 4;
    y = std::clamp(y, 0, std::max(0, image.rows - textSize.height - 10));
    const int w = std::min(textSize.width + 12, image.cols - x);
    const int h = std::min(textSize.height + 10, image.rows - y);
    if (w <= 0 || h <= 0) return;
    const cv::Rect label(x, y, w, h);
    cv::rectangle(image, label, color, cv::FILLED);
    cv::putText(image, text, cv::Point(x + 6, y + textSize.height + 3),
                cv::FONT_HERSHEY_SIMPLEX, scale, cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
}

inline cv::Scalar okColor() { return cv::Scalar(46, 160, 67); }
inline cv::Scalar ngColor() { return cv::Scalar(40, 40, 210); }
inline cv::Scalar warnColor() { return cv::Scalar(0, 165, 255); }

}  // namespace portfolio
