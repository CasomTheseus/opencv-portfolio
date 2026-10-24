//
// Created by Theseus on 2025/8/25.
//
//#define RUN_MAIN
#ifdef RUN_MAIN
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;
int main(int argc,char**argv){
    Mat src = imread("~/Desktop/资料/图片/fill.png");
    imshow("INPUT",src);

    Mat gray,binary,dst;
    cvtColor(src,gray,COLOR_BGR2GRAY);
    threshold(gray,binary,0,255,THRESH_BINARY_INV|THRESH_OTSU);
    Mat kernel = getStructuringElement(MORPH_RECT,Size(15,1),Point(-1,-1));
    morphologyEx(binary,dst,MORPH_OPEN,kernel,Point(-1,-1),1);
    imshow("OPEN DEMO",dst);
    waitKey(0);
    destroyAllWindows();
    return 0;
}
#endif