//
// Created by Theseus on 2025/8/25.
//
//#define RUN_MAIN
#ifdef RUN_MAIN
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;
int main(int argc,char** argv){
    Mat src = imread("~/Desktop/资料/截屏/coins.jpg");
    Mat gray;
    cvtColor(src,gray,COLOR_BGR2GRAY);
    GaussianBlur(gray,gray,Size(9,9),2,2);
    imshow("GRAY",gray);
    vector<Vec3f>circles;
    double minDist = 10;
    double min_radius = 20;
    double max_radius = 100;
    HoughCircles(gray,circles,HOUGH_GRADIENT,2,minDist,100,100,min_radius,max_radius);
    for(size_t t = 0;t<circles.size();t++){
        Point center(circles[t][0],circles[t][1]);
        int radius = round(circles[t][2]);
        //绘制圆
        circle(src,center,radius,Scalar(0,0,255),2,8,0);
        circle(src,center,2,Scalar(0,0,255),2,8,0);
    }
    imshow("HOUGH CIRCLE DEMO",src);
    waitKey(0);
    destroyAllWindows();
    return 0;
}
#endif