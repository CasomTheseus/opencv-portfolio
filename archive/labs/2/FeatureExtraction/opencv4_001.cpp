//
// Created by Theseus on 2025/8/29.
//
//#define RUN_MAIN
#ifdef RUN_MAIN
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;
int main(int argc,char**argv){
    Mat src = imread("~/Desktop/资料/图片/building.jpg");
    imshow("INPUT",src);
    Mat gray;
    cvtColor(src,gray,COLOR_BGR2GRAY);
    vector<Point>corners;
    goodFeaturesToTrack(gray,corners,400,0.015,10);
    for(size_t t = 0;t<corners.size();t++){
        circle(src,corners[t],2,Scalar(0,0,255),2,8,0);
    }
    imshow("CORNERS DETECTION",src);
    waitKey(0);
    destroyAllWindows();
    return 0;
}
#endif