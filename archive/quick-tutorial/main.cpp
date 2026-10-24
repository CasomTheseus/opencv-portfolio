#include <opencv2/opencv.hpp>
#include "quick_opencv.h"
#include <iostream>
#define RUN_MAIN
#ifdef RUN_MAIN
using namespace cv;
using namespace std;

int main(int argc,char** argv) {

    Mat src = imread("~/Desktop/资料/图片/b_85b59cd6e30508cee6f588d3792771ec.jpg");
    if(src.empty()){
        printf("COULD NOT LOAD IMAGE!!!\n");
        return -1;
    }
    namedWindow("INPUT_WINDOW",WINDOW_AUTOSIZE);
    imshow("INPUT_WINDOW",src);
    QuickDemo qd;
    qd.bifilter_demo(src);

    waitKey(0);
    destroyAllWindows();
    return 0;
}

#endif