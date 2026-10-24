//
// Created by Theseus on 2025/8/27.
//
//COLOR-BASED OBJECT TRACKING
//#define RUN_MAIN
#ifdef RUN_MAIN
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;
void harris_demo(Mat &image);
void shitomasi_demo(Mat &image);
void process_frame(Mat &image);
int main(int argc,char**argv){
    VideoCapture capture("~/Downloads/color_object.mp4");
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
        process_frame(frame);
        //imshow( "RESULT",frame);
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
void process_frame(Mat &image){
    Mat hsv,mask;
    cvtColor(image,hsv,COLOR_BGR2HSV);
    inRange(hsv,Scalar(0,43,46),Scalar(10,255,255),mask);
    imshow("mask",mask);
    Mat se = getStructuringElement(MORPH_RECT,Size(15,15));
    morphologyEx(mask,mask,MORPH_OPEN,se);
    imshow("RESULT",mask);
    vector<vector<Point>>contours;
    vector<Vec4i>hirearchy;
    findContours(mask,contours,hirearchy,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE,Point());
    int index = -1;
    double max_area = 0;

    for(size_t t = 0;t<contours.size();t++){
        double area = contourArea(contours[t]);
        double len = arcLength(contours[t],true);
        if(area>max_area){
            max_area = area;
            index = t;
        }
    }
    //进行轮廓拟合
    if(index>=0){
        RotatedRect rrt = minAreaRect(contours[index]);
        ellipse(image,rrt,Scalar(255,0,0),2,8);
        circle(image,rrt.center,4,Scalar(0,255,0),2,8,0);
    }
    imshow("COLOR OBJECT DETECTION",image);
}
#endif