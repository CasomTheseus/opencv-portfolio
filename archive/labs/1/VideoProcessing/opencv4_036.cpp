//
// Created by Theseus on 2025/8/26.
//
//#define RUN_MAIN
#ifdef RUN_MAIN
#include<opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;
int main(int argc,char**argv){
    Mat model = imread("~/Desktop/资料/图片/sample.png");
    Mat src = imread("~/Desktop/资料/图片/target.png");
    if(src.empty()){
        printf("COULD NOT LOAD IMAGE!");
        return -1;
    }
    namedWindow("INPUT",WINDOW_AUTOSIZE);
    imshow("INPUT",src);
    imshow("SAMPLE",model);

    Mat model_hsv,image_hsv,roiHist;
    float h_range[] = {0,180},s_range[] = {0,255};
    const float*ranges[] = {h_range,s_range};
    cvtColor(model,model_hsv,COLOR_BGR2HSV);
    cvtColor(src,image_hsv,COLOR_BGR2HSV);

    int h_bins = 48,s_bins = 48;//数值越大越精确,一般取48左右
    int histSize[] = {h_bins,s_bins};
    int channels[] = {0,1};
    calcHist(&model_hsv,1,channels,Mat(),roiHist,2,histSize,ranges,true,false);
    normalize(roiHist,roiHist,0,255,NORM_MINMAX,-1,Mat());
    MatND backproj;
    calcBackProject(&image_hsv,1,channels,roiHist,backproj,ranges,1.0,true);
    imshow("BACKPROJECT DEMO",backproj);
    waitKey(0);
    destroyAllWindows();
    return 0;
}
#endif