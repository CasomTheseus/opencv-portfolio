//
// Created by Theseus on 2025/8/29.
//
//ORB_KEYPOINTS_DETECTOR
//#define RUN_MAIN
#ifdef RUN_MAIN
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;
int main(int argc,char**argv){
    Mat image = imread("~/Desktop/资料/图片/building.jpg");
    imshow("INPUT",image);
    auto orb = ORB::create(500);
    vector<KeyPoint>kypts;
    orb->detect(image,kypts);
    Mat result;
    drawKeypoints(image,kypts,result,Scalar::all(-1),DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    imshow("KEYPOINTS DETECTION",result);
    waitKey(0);
    destroyAllWindows();
    return 0;
}
#endif