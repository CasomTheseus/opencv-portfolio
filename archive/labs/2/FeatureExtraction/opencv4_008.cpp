//
// Created by Theseus on 2025/8/31.
//
//基于特征的文档对齐
//模版表单/文档，实现特征匹配与对齐
//#define RUN_MAIN
#ifdef RUN_MAIN
#include<opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;
int main(int argc,char**argv){
    Mat ref_img = imread("~/Desktop/资料/图片/54832EC019A3D773943A0BB06EB5D690.jpg");
    Mat img = imread("~/Desktop/资料/图片/A091D00542171755F5E9540FAFE195EB.jpg");
    imshow("MODEL",ref_img);
    auto orb = ORB::create(500);
    vector<KeyPoint>kypts_ref;
    vector<KeyPoint>kypts_img;
    Mat desc_book,desc_book_on_desk;
    orb->detectAndCompute(ref_img,Mat(),kypts_ref,desc_book);
    orb->detectAndCompute(img,Mat(),kypts_img,desc_book_on_desk);
    Mat result;
    auto bf_matcher = BFMatcher::create(NORM_HAMMING,false);
    vector<DMatch>matches;
    bf_matcher->match(desc_book,desc_book_on_desk,matches);
    float good_rate = 0.15f;
    int num_good_matches = matches.size()*good_rate;
    std::cout<<num_good_matches<<std::endl;
    std::sort(matches.begin(),matches.end());
    matches.erase(matches.begin()+num_good_matches,matches.end());
    drawMatches(ref_img,kypts_ref,img,kypts_img,matches,result);
    imshow("MATCH",result);

    //Extract location of good matches
    vector<Point2f>points1,points2;
    for(size_t t = 0;t<matches.size();t++){
        points1.push_back(kypts_img[matches[t].queryIdx].pt);
        points2.push_back(kypts_ref[matches[t].trainIdx].pt);
    }
    Mat h = findHomography(points1,points2,RANSAC);
    Mat aligned_doc;
    warpPerspective(img,aligned_doc,h,ref_img.size());
    imwrite("~/Desktop/资料/图片/RESULT.jpg",aligned_doc);
    waitKey(0);
    destroyAllWindows();
    return 0;
}
#endif