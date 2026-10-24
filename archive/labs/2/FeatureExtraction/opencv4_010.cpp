//
// Created by Theseus on 2025/9/2.
//
//工业级实战案例：基于特征的条码标签定位和有无判定
//#define RUN_MAIN
#ifdef RUN_MAIN
#include <opencv2/opencv.hpp>
#include <iostream>
#include "../头文件/ORBDetector.h"
using namespace std;
using namespace cv;
int main(int argc,char**argv){
    Mat refImg = imread("FILEPATH");
    ORBDetector orb_detector;
    orb_detector.initORB(refImg);
    vector<std::string>files;
    glob("EXAMPLE FILEPATH",files);
    cv::Mat temp;
    for(auto file:files){
    std::cout<<file<<std::endl;
    cv::Mat image = imread(file);
    int64 start = getTickCount();
    bool OK = orb_detector.detect_and_analysis(image,temp);
    double ct = (getTickCount()-start)/getTickFrequency();
    printf("DECODE TIME:%.5f ms\n",ct*1000);
    std:cout<<"LABELS:"<<(OK == true)<<std::endl;
    imshow("TEMP",temp);
    waitKey(0);
    }
}
#endif