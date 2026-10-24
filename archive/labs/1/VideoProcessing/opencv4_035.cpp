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
    VideoCapture capture;
    capture.open("~/Desktop/资料/图片/01.mp4");
    if(!capture.isOpened()){
        printf("COULD NOT LOAD VIDEO!");
    }
    namedWindow("INPUT",WINDOW_AUTOSIZE);

    Mat frame,hsv,lab,mask,result;
    while(true){
        bool ret = capture.read(frame);
        if(!ret)break;
        imshow("FRAME",frame);
        cvtColor(frame,hsv,COLOR_BGR2HSV);
        //cvtColor(frame,lab,COLOR_BGR2Lab);
        imshow("HSV",hsv);
        inRange(hsv,Scalar(35,43,36),Scalar(77,255,255),mask);//background的颜色
        bitwise_not(mask,mask);
        bitwise_and(frame,frame,result,mask);
        imshow("MASK",mask);
        imshow("RESULT",result);
        //imshow("LAB",lab);
        char c = waitKey(5);
        if(c ==27){
            break;
        }
    }
    destroyAllWindows();
    capture.release();
    return 0;



}
#endif