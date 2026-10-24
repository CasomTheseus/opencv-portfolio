//
// Created by Theseus on 2025/8/29.
//
//SIFT DESCRIPTOR EXTRACTION
//#define RUN_MAIN
#ifdef RUN_MAIN
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;
int main(int argc,char**argv){
    Mat image = imread("~/Desktop/资料/截屏/截屏2025-08-29 15.42.12.png");
    imshow("INPUT",image);
    auto sift = SIFT::create(500);
    vector<KeyPoint>kypts;
    sift->detect(image,kypts);
    Mat result;
    drawKeypoints(image,kypts,result,Scalar::all(-1),DrawMatchesFlags::DEFAULT);
    for(size_t t=0;t<kypts.size();t++){
        std::cout<<"PT:"<<kypts[t].pt<<"ANGLE:"<<kypts[t].angle<<"SIZE:"<<kypts[t].size<<std::endl;
    }
    Mat desc_orb;
    sift->compute(image,kypts,desc_orb);
    imshow("KEYPOINTS DETECTION",result);
    std::cout<<desc_orb.rows<<"x"<<desc_orb.cols<<std::endl;
    waitKey(0);
    destroyAllWindows();
    return 0;
}
#endif