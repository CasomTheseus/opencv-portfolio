//
// Created by Theseus on 2025/8/26.
//
//#define RUN_MAIN
#ifdef RUN_MAIN
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;
int main(int argc,char**argv){
    Mat src = imread("~/Desktop/资料/图片/cross.png");
    imshow("INPUT",src);
    Mat gray,binary;
    cvtColor(src,gray,COLOR_BGR2GRAY);

    threshold(gray,binary,0,255,THRESH_BINARY_INV|THRESH_OTSU);
    imshow("BINARY",binary);

    //Mat hothat;
    //morphologyEx(binary,hothat,MORPH_TOPHAT,k);
    //imshow("TOP HAT DEMO",hothat);

    //morphologyEx(binary,hothat,MORPH_BLACKHAT,k);
    //imshow("BLACK HAT DEMO",hothat);
    Mat hitmiss;
    Mat k = getStructuringElement(MORPH_CROSS,Size(15,15)/*关键参数*/,Point(-1,-1));
    morphologyEx(binary,hitmiss,MORPH_HITMISS,k);
    imshow("HIT AND MISS DEMO",hitmiss);
    waitKey(0);
    destroyAllWindows();
    return 0;
}

#endif