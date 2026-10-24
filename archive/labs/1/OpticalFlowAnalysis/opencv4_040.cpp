//
// Created by Theseus on 2025/8/27.
//
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
    Mat old_frame,old_gray;
    capture.read(old_frame);
    cvtColor(old_frame,old_gray,COLOR_BGR2GRAY);
    vector<Point2f>feature_pts;
    vector<Point2f>initPoints;
    double quality_level = 0.01;
    int minDistance = 10;
    goodFeaturesToTrack(old_gray,feature_pts,5000,quality_level,minDistance,Mat(),3,false);
    Mat frame,gray;
    vector<Point2f>pts[2];
    pts[0].insert(pts[0].end(),feature_pts.begin(),feature_pts.end());
    initPoints.insert(initPoints.end(),feature_pts.begin(),feature_pts.end());
    vector<uchar>status;
    vector<float>err;//定义停止条件
    TermCriteria criteria = TermCriteria(TermCriteria::COUNT+TermCriteria::EPS,30,0.01);
    while(true){
        bool ret = capture.read(frame);
        if(!ret)break;
        imshow("FRAME",frame);
        cvtColor(frame,gray,COLOR_BGR2GRAY);

        //calculate optical flow
        calcOpticalFlowPyrLK(old_gray,gray,pts[0],pts[1],status,err,Size(31,31),3,criteria,0);
        size_t i = 0,k = 0;
        for(i = 0;i<pts[1].size();i++){
            //距离与状态监测
            double dist = abs(pts[0][i].x-pts[1][i].x)+abs(pts[0][i].y-pts[1][i].y);
            if(status[i]&&dist>2){
                pts[0][k] = pts[0][i];
                initPoints[k] = initPoints[i];
                pts[1][k++] = pts[1][i];
                int b = rng.uniform(0,255);
                int g = rng.uniform(0,255);
                int r = rng.uniform(0,255);
                circle(frame,pts[1][i],2,Scalar(b,g,r),2,8,0);
                line(frame,pts[0][i],pts[1][i],Scalar(b,g,r),2,8,0);
            }
        }

        //update key points
        pts[0].resize(k);
        pts[1].resize(k);
        initPoints.resize(k);

        //绘制跟踪线
        draw_lines(frame,initPoints,pts[1]);

        //绘制跟踪
        imshow("KLT_DEMO",frame);

        char c = waitKey(50);
        if(c == 27){//ESC
            break;
        }

        //update to old
        std::swap(pts[1],pts[0]);
        cv::swap(old_gray,gray);

        //re_init
        if(pts[0].size()<40){
            goodFeaturesToTrack(old_gray,feature_pts,5000,quality_level,minDistance,Mat(),3,false);
            pts[0].insert(pts[0].end(),feature_pts.begin(),feature_pts.end());
            initPoints.insert(initPoints.end(),feature_pts.begin(),feature_pts.end());
        }
    }
    destroyAllWindows();
    capture.release();
    return 0;
}
void draw_lines(Mat &frame,vector<Point2f>pts1,vector<Point2f>pts2){
    vector<Scalar>lut;
    for(size_t t = 0;t<pts1.size();t++){
        int b = rng.uniform(0,255);
        int g = rng.uniform(0,255);
        int r = rng.uniform(0,255);
        lut.push_back(Scalar(b,g,r));
    }
    for(size_t t = 0;t<pts1.size();t++){
        line(frame,pts1[t],pts2[t],lut[t],2,8,0);
    }
}
#endif