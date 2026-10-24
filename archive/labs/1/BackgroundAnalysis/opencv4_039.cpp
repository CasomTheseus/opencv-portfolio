//
// Created by Theseus on 2025/8/27.
//
//#define RUN_MAIN
#ifdef RUN_MAIN
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;
void process_frame(Mat &image);
void process2(Mat &image);
auto pMOG2 = createBackgroundSubtractorMOG2(500,1000/*对移动更不敏感*/,false);
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
        process2(frame);
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
void process2(Mat &image){
    Mat mask,bg_image;
    pMOG2->apply(image,mask);
    //形态学操作
    Mat se = getStructuringElement(MORPH_RECT,Size(1,5),Point(-1,-1));
    morphologyEx(mask,mask,MORPH_OPEN,se);
    imshow("MASK",mask);
    vector<vector<Point>>contours;
    vector<Vec4i>hirearchy;
    findContours(mask,contours,hirearchy,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE,Point());
    int index = -1;
    double max_area = 0;

    for(size_t t = 0;t<contours.size();t++){
        double area = contourArea(contours[t]);
        double len = arcLength(contours[t],true);
        if(area<100){
            continue;
        }
        Rect box = boundingRect(contours[t]);
        //rectangle(image,box,Scalar(0,0,255),2,8,0);
        RotatedRect rrt = minAreaRect(contours[t]);
        circle(image,rrt.center,2,Scalar(255,0,0),2,8,0);
        ellipse(image,rrt,Scalar(0,0,255),2,8);
    }
    imshow("RESULT",image);

}
void process_frame(Mat &image){
    Mat mask,bg_image;
    pMOG2->apply(image,mask);
    pMOG2->getBackgroundImage(bg_image);
    imshow("BACKGROUND",bg_image);
    imshow("BACKGROUND BASED TRACKING",mask);
}
#endif