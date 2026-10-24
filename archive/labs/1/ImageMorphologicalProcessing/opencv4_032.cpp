//
// Created by Theseus on 2025/8/25.
//
//#define RUN_MAIN
#ifdef RUN_MAIN
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;
int main(int argc,char**argv){
    Mat src = imread("~/Desktop/资料/图片/building.jpg");

    Mat gray,binary;
    cvtColor(src,gray,COLOR_BGR2GRAY);
    imshow("INPUT",gray);
    Mat basic_grad,inter_grad,exter_grad;
    Mat kernel = getStructuringElement(MORPH_RECT,Size(3,3),Point(-1,-1));
    morphologyEx(gray,basic_grad,MORPH_GRADIENT, kernel,Point(-1,-1),1);
    imshow("BASIC GRADIENT",basic_grad);

    Mat dst1,dst2;
    erode(gray,dst1,kernel);
    dilate(gray,dst2,kernel);
    //内梯度和外梯度
    subtract(gray,dst1,inter_grad);
    subtract(dst2,gray,exter_grad);
    imshow("INTERNAL GRADIENT",inter_grad);
    imshow("EXTERNAL GRADIENT",exter_grad);

    threshold(basic_grad,binary,0,255,THRESH_BINARY|THRESH_OTSU);
    imshow("BINARY",binary);

    waitKey(0);
    destroyAllWindows();
    return 0;
}
#endif