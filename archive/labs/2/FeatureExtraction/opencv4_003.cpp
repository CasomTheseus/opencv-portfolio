//
// Created by Theseus on 2025/8/29.
//
//ORB FEATURE DESCRIPTOR
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
    Mat desc_orb;
    orb->compute(image,kypts,desc_orb);
    imshow("KEYPOINTS DETECTION",result);
    std::cout<<desc_orb.rows<<"x"<<desc_orb.cols<<std::endl;
    waitKey(0);
    destroyAllWindows();
    return 0;
}
#endif