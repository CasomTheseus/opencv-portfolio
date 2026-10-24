//
// Created by Theseus on 2025/8/25.
//
//#define RUN_MAIN
#ifdef RUN_MAIN
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;
RNG rng(12345);
void ccl_stats_demo(Mat &image);
void fit_circle_demo(Mat &image){
    //二值化
    GaussianBlur(image,image,Size(3,3),0);
    Mat gray,binary;
    cvtColor(image,gray,COLOR_BGR2GRAY);
    threshold(gray,binary,0,255,THRESH_BINARY|THRESH_OTSU);
    //轮廓发现
    imshow("BINARY",binary);
    vector<vector<Point>>contours;
    vector<Vec4i>hirearchy;
    findContours(binary,contours,hirearchy,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE,Point());

    //拟合圆或者椭圆
    for(size_t t = 0;t<contours .size();t++) {
        //drawContours(image,contours,t,Scalar(0,0,255),2,8);
        RotatedRect rrt = fitEllipse(contours[t]);
        float w = rrt.size.width;
        float h = rrt.size.height;
        Point center = rrt.center;
        circle(image,center,3,Scalar(255,0,0),2,8,0);
        ellipse(image,rrt,Scalar(0,255,0),2,8);
    }
    imshow("fit result",image);
}
int main(int argc,char**argv){
    Mat src = imread("~/Desktop/资料/截屏/截屏2025-08-25 11.11.07.png");
    if(src.empty()){
        printf("COULD NOT LOAD IMAGE!");
        return -1;
    }
    /*
    //二值化
    GaussianBlur(src,src,Size(3,3),0);
    Mat gray,binary;
    cvtColor(src,gray,COLOR_BGR2GRAY);
    threshold(gray,binary,0,255,THRESH_BINARY|THRESH_OTSU);
    //轮廓发现
    imshow("BINARY",binary);
    vector<vector<Point>>contours;
    vector<Vec4i>hirearchy;
    findContours(binary,contours,hirearchy,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE,Point());

    //多边形逼近演示程序
    for(size_t t = 0;t<contours .size();t++) {
        Moments mm = moments(contours[t]);
        double cx = mm.m10/mm.m00;
        double cy = mm.m01/mm.m00;
        circle(src,Point(cx,cy),3,Scalar(255,0,0),2,8,0);

        double area = contourArea(contours[t]);
        double clen = arcLength(contours[t],true);

        Mat result;
        approxPolyDP(contours[t],result,4,true);
        printf("CORNERS:%d\narea:%.2f,length:%.2f\n",result.rows,area,clen);
        if(result.rows == 6){
            putText(src,"poly",Point(cx,cy-10),FONT_HERSHEY_PLAIN,1.0,Scalar(0,0,255),1,8);
        }
        if(result.rows == 4){
            putText(src,"rectangle",Point(cx,cy-10),FONT_HERSHEY_PLAIN,1.0,Scalar(0,255,255),1,8);
        }
        if(result.rows == 3){
            putText(src,"triangle",Point(cx,cy-10),FONT_HERSHEY_PLAIN,1.0,Scalar(255,0,255),1,8);
        }
        if(result.rows > 10){
            putText(src,"circle",Point(cx,cy-10),FONT_HERSHEY_PLAIN,1.0,Scalar(255,255,0),1,8);
        }
    }
    imshow("FIND CONTOURS DEMO",src);
     */
    fit_circle_demo(src);
    waitKey(0);
    destroyAllWindows();
    return 0;
}

#endif