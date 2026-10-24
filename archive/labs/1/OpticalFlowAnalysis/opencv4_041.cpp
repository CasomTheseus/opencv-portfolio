//
// Created by Theseus on 2025/8/28.
//
//birthday special code!!!
//稠密光流分析与跟踪
//#define RUN_MAIN
#ifdef RUN_MAIN
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;
RNG rng(12345);
void draw_lines(Mat &frame,vector<Point2f>pts1,vector<Point2f>pts2);
int main(int argc,char**argv){
    VideoCapture capture("~/Downloads/IMG_7098.mp4");
    if(!capture.isOpened()){
        printf("COULD NOT LOAD VIDEO!");
        return -1;
    }
    namedWindow("FRAME",WINDOW_AUTOSIZE);
    Mat frame,preframe;
    Mat gray,pregray;
    Mat bgr;
    capture.read(preframe);
    cvtColor(preframe,pregray,COLOR_BGR2GRAY);
    Mat hsv = Mat::zeros(preframe.size(),preframe.type());
    Mat mag = Mat::zeros(hsv.size(),CV_32FC1);
    Mat ang = Mat::zeros(hsv.size(),CV_32FC1);
    Mat xpts = Mat::zeros(hsv.size(),CV_32FC1);
    Mat ypts = Mat::zeros(hsv.size(),CV_32FC1);
    Mat_<Point2f>flow;
    vector<Mat>mv;
    split(hsv,mv);
    while(true){
        bool ret = capture.read(frame);
        if(!ret)break;
        cvtColor(frame,gray,COLOR_BGR2GRAY);
        calcOpticalFlowFarneback(pregray,gray,flow,0.5,3,15,3,5,1.2,0);
        for(int row = 0;row<flow.rows;row++){
            for(int col = 0;col<flow.cols;col++){
                const Point2f &flow_xy = flow.at<Point2f>(row,col);
                xpts.at<float>(row,col) = flow_xy.x;
                ypts.at<float>(row,col) = flow_xy.y;
            }
        }
        cartToPolar(xpts,ypts,mag,ang);
        ang = ang*180/CV_PI/2.0;
        normalize(mag,mag,0,255,NORM_MINMAX);
        convertScaleAbs(mag,mag);
        convertScaleAbs(ang,ang);
        mv[0] = ang;
        mv[1] = Scalar(255);
        mv[2] = mag;
        merge(mv,hsv);
        cvtColor(hsv,bgr,COLOR_HSV2BGR);
        imshow("FRAME",frame);
        imshow("RESULT",bgr);
        char c = waitKey(1);
        if(c == 27){
            break;
        }
    }
    destroyAllWindows();
    capture.release();
}
#endif