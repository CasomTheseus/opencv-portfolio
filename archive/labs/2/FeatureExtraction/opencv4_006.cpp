//
// Created by Theseus on 2025/8/29.
//
//单应性矩阵和透视变换
//可以在图片中识别出物体并将其摆正
//#define RUN_MAIN
#ifdef RUN_MAIN
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;
int main(int argc,char**argv){
    Mat image = imread("~/Desktop/资料/图片/2DB8185373164B075B8E0C0B5D0466D3.jpg");
    Mat gray,binary;
    cvtColor(image,gray,COLOR_BGR2GRAY);
    threshold(gray,binary,0,255,THRESH_BINARY|THRESH_OTSU);

    vector<vector<Point>>contours;
    vector<Vec4i>hirearchy;
    findContours(binary,contours,hirearchy,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE);
    int index = -1;
    double max = -1;
    for(int i = 0;i<contours.size();i++){
        double area = contourArea(contours[i]);
        if(area>max){
            max = area;
            index = i;
        }
    }
    drawContours(image,contours,index,Scalar(0,255,0),2,8);

    Mat approxCurves;
    vector<Point2f>srcPts;
    approxPolyDP(contours[index],approxCurves,200,true);
    for(int i = 0;i<contours.size();i++){
        Vec2i pt = approxCurves.at<Vec2i>(i,0);
        srcPts.push_back(Point2f(pt[0],pt[1]));
        circle(image,Point(pt[0],pt[1]),12,Scalar(0,0,255),2,8,0);
    }
    vector<Point2f>dstPts;
    dstPts.push_back(Point2f(0,0));
    dstPts.push_back(Point2f(600,0));
    dstPts.push_back(Point2f(600,900));
    dstPts.push_back(Point2f(0,900));
    imshow("CONTOURS",image);
    //imwrite("~/Desktop/资料/图片/result.jpg",image);

    Mat h = findHomography(srcPts,dstPts);
    Mat dst;
    warpPerspective(image,dst,h,Size(600,900));
    waitKey(0);
    destroyAllWindows();
    return 0;
}
#endif