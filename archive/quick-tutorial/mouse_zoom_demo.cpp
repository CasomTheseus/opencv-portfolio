//
// Created by Theseus on 25-7-29.
//
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
/*
class QuickDemo {
private:
    double scale = 1.0;  // 当前缩放比例
    const double scale_step = 0.1;  // 每次滚轮的缩放步长
    Mat original_image;  // 存储原始图像

public:
    // 鼠标滚轮回调函数
    static void onMouse(int event, int x, int y, int flags, void* userdata) {
        QuickDemo* self = static_cast<QuickDemo*>(userdata);

        if (event == EVENT_MOUSEWHEEL) {
            // 获取滚轮方向（Windows和Mac可能有差异）
            int wheel_direction = getMouseWheelDelta(flags);

            // 调整缩放比例（限制范围）
            if (wheel_direction > 0) {
                self->scale = std::min(self->scale + self->scale_step, 5.0);  // 放大上限5倍
            } else {
                self->scale = std::max(self->scale - self->scale_step, 0.2);  // 缩小下限0.2倍
            }

            // 实时更新显示
            self->updateZoomedImage(x, y);
        }
    }

    // 辅助函数：获取滚轮方向（兼容不同平台）
    static int getMouseWheelDelta(int flags) {
#ifdef _WIN32
        return -(short)((flags >> 16) & 0xFFFF);  // Windows
#else
        return (flags > 0) ? 1 : -1;  // Mac/Linux简化处理
#endif
    }

    // 更新缩放后的图像显示
    void updateZoomedImage(int center_x, int center_y) {
        if (original_image.empty()) return;

        Mat zoomed;
        Size new_size(original_image.cols * scale, original_image.rows * scale);

        // 进行缩放（使用高质量插值）
        resize(original_image, zoomed, new_size, 0, 0, INTER_LANCZOS4);

        // 计算显示区域（以鼠标位置为中心）
        Rect display_roi(
                std::max(0, int(center_x * scale - original_image.cols/2)),
                std::max(0, int(center_y * scale - original_image.rows/2)),
                original_image.cols, original_image.rows
        );

        // 确保ROI不越界
        display_roi = display_roi & Rect(0, 0, zoomed.cols, zoomed.rows);

        // 显示缩放后的区域
        imshow("Zoomable Image", zoomed(display_roi));
    }

    // 主功能函数
    void mouse_zoom_demo(Mat& image) {
        original_image = image.clone();
        namedWindow("Zoomable Image", WINDOW_AUTOSIZE);
        setMouseCallback("Zoomable Image", onMouse, this);
        imshow("Zoomable Image", image);
    }
};
*/