//
// Created by Theseus on 2025/8/26.
//
//HARRIS AND SHI-TOMASI CORNER DETECTION
//#define RUN_MAIN
#ifdef RUN_MAIN
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;
void harris_demo(Mat &image);
void shitomasi_demo(Mat &image);
int main(int argc,char**argv){
    VideoCapture capture("~/Downloads/bike.avi");
    if(!capture.isOpened()){
        printf("COULD NOT LOAD THE VIDEO!");
        return -1;
    }
    namedWindow("FRAME",WINDOW_AUTOSIZE);
    Mat frame;
    while(true){
        bool ret = capture.read(frame);
        if(!ret)break;
        imshow("FRAME",frame);
        shitomasi_demo(frame);
        imshow( "RESULT",frame);
        char c = waitKey(5);
        if(c==27){
            break;
        }
    }
    destroyAllWindows();
    capture.release();
    return 0;
}
void harris_demo(Mat &image){
    Mat gray;
    cvtColor(image,gray,COLOR_BGR2GRAY);
    Mat dst;
    double k = 0.04;
    int blocksize = 2;
    int ksize = 3;
    cornerHarris(gray,dst,blocksize,ksize,k);
    Mat dst_norm = Mat::zeros(dst.size(),dst.type());
    normalize(dst,dst_norm,0,255,NORM_MINMAX,-1,Mat());
    convertScaleAbs(dst_norm,dst_norm);

    //draw corners
    RNG rng(12345);
    for(int row = 0;row<image.rows;row++){
        for(int col = 0;col<image.cols;col++){
            int rsp = dst_norm.at<uchar>(row,col);
            if(rsp>200){
                int b = rng.uniform(0,255);
                int g = rng.uniform(0,255);
                int r = rng.uniform(0,255);
                circle(image,Point(col,row),5,Scalar(b,g,r),2,8,0);

            }
        }
    }
}
void shitomasi_demo(Mat &image){
    Mat gray,dst;
    cvtColor(image,gray,COLOR_BGR2GRAY);
    vector<Point2f>corners;
    double quality_level = 0.01;
    RNG rng(12345);
    goodFeaturesToTrack(gray,corners,200,quality_level,3,Mat(),3,false);
    for(int i = 0;i<corners.size();i++){
        int b = rng.uniform(0,255);
        int g = rng.uniform(0,255);
        int r = rng.uniform(0,255);
        circle(image,corners[i],5,Scalar(b,g,r),2,8,0);
    }
}
#endif