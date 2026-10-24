//
// Created by Theseus on 2025/9/2.
//

#ifndef OPENCV480_TEST_ORBDETECTOR_H
#define OPENCV480_TEST_ORBDETECTOR_H
#pragma once
#include<opencv2/opencv.hpp>
class ORBDetector{
public:
    ORBDetector(void);
    ~ORBDetector(void);
    void initORB(cv::Mat &refImg);
    bool detect_and_analysis(cv::Mat &image,cv::Mat &aligned);
private:
    cv::Ptr<cv::ORB>orb = cv::ORB::create(500);
    std::vector<cv::KeyPoint>tpl_kps;
    cv::Mat tpl_descriptors;
    cv::Mat tpl;
};
#endif //OPENCV480_TEST_ORBDETECTOR_H
