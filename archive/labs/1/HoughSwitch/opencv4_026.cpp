//
// Created by Theseus on 2025/8/25.
//
//#define RUN_MAIN
#ifdef RUN_MAIN
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <iostream>

using namespace std;
using namespace cv;

RNG rng(12345);
void ccl_stats_demo(Mat &image);
void contour_info(Mat &image,vector<vector<Point>>&contours){
    //二值化
    Mat dst;
    GaussianBlur(image,dst,Size(3,3),0);
    Mat gray,binary;
    cvtColor(dst,gray,COLOR_BGR2GRAY);//转化成灰度图像
    threshold(gray,binary,0,255,THRESH_BINARY|THRESH_OTSU);

    //轮廓发现
    vector<Vec4i>hirearchy;
    findContours(binary,contours,hirearchy,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE,Point());
}
int main(int argc,char**argv){
    Mat src1 = imread("~/Desktop/资料/截屏/截屏2025-08-25 09.49.55.png");
    Mat src2 = imread("~/Desktop/资料/截屏/截屏2025-08-25 09.50.03.png");
    if(src1.empty()||src2.empty()){
        printf("COULD NOT LOAD IMAGE!");
        return -1;
    }
    imshow("INPUT1",src1);
    imshow("INPUT2",src2);
    vector<vector<Point>>contours1;
    vector<vector<Point>>contours2;
    contour_info(src1,contours1);
    contour_info(src2,contours2);

    Moments mm2 = moments(contours2[0]);
    Mat hu2;
    HuMoments(mm2,hu2);
    for(size_t t = 0;t<contours1.size();t++) {
        Moments mm = moments(contours1[t]);
        Mat hu;
        HuMoments(mm, hu);
        double dist = matchShapes(hu, hu2, CONTOURS_MATCH_I1, 0);
        if (dist < 1.0) {
            printf("MATCHED DISTANCE VALUE:%.2f\n", dist);
            drawContours(src1, contours1, t, Scalar(0, 0, 255), 2, 8);
        }
    }
        imshow("MATCH CONTOURS DEMO",src1);

        waitKey(0);
        destroyAllWindows();
        return 0;
}
#endif

