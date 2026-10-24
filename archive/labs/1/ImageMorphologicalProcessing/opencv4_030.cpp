//
// Created by Theseus on 2025/8/25.
//
//EROSION AND DILATION
//#define RUN_MAIN
#ifdef RUN_MAIN
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;
void erode_dilate_demo();
int main(int argc,char**argv){
    erode_dilate_demo();
    waitKey(0);
    destroyAllWindows();
    return 0;
}
void erode_dilate_demo(){
    Mat src = imread("~/Desktop/资料/图片/OUTPUT_IMAGE1.jpg");
    imshow("INPUT",src);
    /*
    Mat gray,binary;
    cvtColor(src,gray,COLOR_BGR2GRAY);
    threshold(gray,binary,0,255,THRESH_BINARY|THRESH_OTSU);
    */
    Mat dst1,dst2;
    Mat kernel = getStructuringElement(MORPH_RECT,Size(3,3),Point(-1,-1));
    erode(src,dst1,kernel);
    dilate(src,dst2,kernel);
    imshow("ERODE",dst1);
    imshow("DILATE",dst2);
    //cv::imwrite("~/Desktop/资料/图片/OUTPUT_IMAGE2.jpg",dst2);
}
#endif
