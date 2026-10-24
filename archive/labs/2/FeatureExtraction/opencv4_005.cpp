//
// Created by Theseus on 2025/8/29.
//
//FEATURE MATCH
//FLANN匹配，2009年发布的开源高维数据匹配算法库
//Fast Library for Approximate Nearest Neighbours
//#define RUN_MAIN
#ifdef RUN_MAIN
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;
int main(int argc,char**argv){
    Mat book = imread("~/Desktop/资料/图片/172AA955BDC04286ECCCE60C9B44D998.jpg");
    Mat book_on_desk = imread("~/Desktop/资料/图片/D17F4E328A719E8561680FEB7577AE68.jpg");
    imshow("BOOK",book);
    auto orb = ORB::create(500);
    vector<KeyPoint>kypts_book;
    vector<KeyPoint>kypts_book_on_desk;
    Mat desc_book,desc_book_on_desk;
    orb->detectAndCompute(book,Mat(),kypts_book,desc_book);
    orb->detectAndCompute(book_on_desk,Mat(),kypts_book_on_desk,desc_book_on_desk);
    Mat result;
    auto bf_matcher = BFMatcher::create(NORM_HAMMING,false);
    vector<DMatch>matches;
    bf_matcher->match(desc_book,desc_book_on_desk,matches);
    drawMatches(book,kypts_book,book_on_desk,kypts_book_on_desk,matches,result);
    imshow("VIOLENCE MATCH",result);
    waitKey(0);
    destroyAllWindows();
    return 0;
}
#endif