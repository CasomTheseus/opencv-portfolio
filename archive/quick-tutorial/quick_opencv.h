//
// Created by Theseus on 25-7-23.
//

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;
class QuickDemo {
    public:
        double scale = 1.0;
        const double scale_step = 0.1;
        void color_space_Demo(Mat&image);
        void mat_creation_demo(Mat&image);
        void pixel_visit_demo(Mat&image);
        void operators_demo(Mat&image);
        void tracking_bar_demo(Mat&image);
        void key_demo(Mat&image);
        void color_style(Mat&image);
        void bitwise_demo(Mat&image);
        void channels_demo(Mat&image);
        void inrange_demo(Mat&image);
        void pixel_statistic_demo(Mat&image);
        void drawing_demo(Mat&image);
        void random_drawing(Mat&image);
        void polyline_drawing_demo(Mat&image);
        void mouse_drawing_demo(Mat&image);
        void norm_demo(Mat&image);
        void resize_demo(Mat&image);
        void flip_demo(Mat&image);
        void rotate_demo(Mat&image);
        void video_demo(Mat&image);
        void updateZoomedImage(int center_x, int center_y);
        void mouse_zoom_demo(Mat&image);
        static void onMouse(int event, int x, int y, int flags, void* userdata);
        void showHistogram(Mat&image);
        void histogram_2d_demo(Mat&image);
        void histogram_eq_demo(Mat &image);
        void blur_demo(Mat &image);
        void gaussian_blur_demo(Mat&image);
        void bifilter_demo(Mat&image);
};


