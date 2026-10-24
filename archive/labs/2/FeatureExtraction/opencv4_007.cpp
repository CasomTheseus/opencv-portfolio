//
// Created by Theseus on 2025/8/31.
//
//基于特征的对象检测
//可以在图片中寻找出需要的物体
//#define RUN_MAIN
#ifdef RUN_MAIN
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;
int main(int argc,char**argv){
    Mat book = imread("~/Desktop/资料/opencv学习材料/22.png");
    Mat book_on_desk = imread("~/Desktop/资料/opencv学习材料/55.jpg");
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
    float good_rate = 0.15f;
    int num_good_matches = matches.size()*good_rate;
    std::cout<<num_good_matches<<std::endl;
    std::sort(matches.begin(),matches.end());
    matches.erase(matches.begin()+num_good_matches,matches.end());
    drawMatches(book,kypts_book,book_on_desk,kypts_book_on_desk,matches,result);
    vector<Point2f>obj_pts;
    vector<Point2f>scene_pts;
    for(size_t t = 0;t<matches.size();t++){
        obj_pts.push_back(kypts_book[matches[t].queryIdx].pt);
        scene_pts.push_back(kypts_book_on_desk[matches[t].trainIdx].pt);
    }
    Mat h = findHomography(obj_pts,scene_pts,RANSAC);
    vector<Point2f>srcPts;
    srcPts.push_back(Point2f(0,0));
    srcPts.push_back(Point2f(book.cols,0));
    srcPts.push_back(Point2f(book.cols,book.rows));
    srcPts.push_back(Point2f(0,book.rows));

    std::vector<Point2f>dstPts(4);
    perspectiveTransform(srcPts,dstPts,h);

    for(int i =0;i<4;i++){
        line(book_on_desk,dstPts[i],dstPts[(i+1)%4],Scalar(0,0,255),2,8,0);
    }
    imshow("VIOLENCE MATCH",result);
    namedWindow("OBJECT DETECTION",WINDOW_FREERATIO);
    imshow("OBJECT DETECTION",book_on_desk);
    imwrite("~/Desktop/资料/opencv学习材料/object_detection_result1.jpg",book_on_desk);
    waitKey(0);
    destroyAllWindows();
    return 0;
}
#endif
