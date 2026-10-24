//
// Created by Theseus on 2025/8/26.
//
//#define RUN_MAIN
#ifdef RUN_MAIN
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;
RNG rng(12345);
int main(int argc,char**argv){
    Mat src = imread("~/Downloads/小案例素材/case6.jpg");
    namedWindow("INPUT",WINDOW_AUTOSIZE);
    imshow("INPUT",src);

    //二值化
    GaussianBlur(src,src,Size(3,3),0);
    Mat gray,binary;
    cvtColor(src,gray,COLOR_BGR2GRAY);
    threshold(gray,binary,0,255,THRESH_BINARY|THRESH_OTSU);

    //闭操作
    Mat se = getStructuringElement(MORPH_RECT,Size(15,15),Point(-1,-1));
    morphologyEx(binary,binary,MORPH_CLOSE,se);
    imshow("BINARY",binary);

    //轮廓发现
    int height = binary.rows;
    int width = binary.cols;
    vector<vector<Point>>contours;
    vector<Vec4i>hirearchy;
    findContours(binary,contours,hirearchy,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE,Point());
    double max_area = -1;
    int cindex = -1;
    for(size_t t =0;t<contours.size();t++){
        Rect rect = boundingRect(contours[t]);
        if(rect.height>=height||rect.width>=width){
            continue;
        }
        double area = contourArea(contours[t]);
        double len = arcLength(contours[t],true);
        if(area>max_area){
            max_area = area;
            cindex = t;
        }
    }
    drawContours(src,contours,cindex,Scalar(0,0,255),2,8);
    Mat pts;
    approxPolyDP(contours[cindex],pts,4,true);
    Mat result = Mat::zeros(src.size(),src.type());
    drawContours(result,contours,cindex,Scalar(0,0,255),2,8);
    for(int i = 0;i<pts.rows;i++){
        Vec2i pt = pts.at<Vec2i>(i,0);
        circle(src,Point(pt[0],pt[1]),2,Scalar(0,255,0),2,8,0);
        circle(result,Point(pt[0],pt[1]),2,Scalar(0,255,0),2,8,0);
    }
    imshow("FIND MAX CONTOURS",src);
    imshow("RESULT CONTOURS ANALYSIS",result);
    printf("AREA:%.2f\n",max_area);
    waitKey(0);
    destroyAllWindows();
    return 0;
}
#endif