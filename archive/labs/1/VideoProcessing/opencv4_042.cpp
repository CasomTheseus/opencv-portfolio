//
// Created by Theseus on 2025/8/28.
//
//birthday special code!!!
//基于均值迁移的视频分析
//均值迁移算法原理
//MeanShift/CA-MeanShift原理
//#define RUN_MAIN
#ifdef RUN_MAIN
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;
/*
int main(){
    // 测试深度转换是否正常工作
    Mat image_8u = imread("~/Desktop/资料/截屏/coins.jpg", IMREAD_COLOR);  // 默认CV_8U
    Mat image_32f;
    image_8u.convertTo(image_32f, CV_32F);  // 转换为32位浮点

    cout << "转换前深度: " << image_8u.depth() << " (CV_8U)" << endl;
    cout << "转换后深度: " << image_32f.depth() << " (CV_32F)" << endl;

// 验证转换是否正确
    if (image_32f.depth() == CV_32F) {
        cout << "✅ 深度转换正常" << endl;
    } else {
        cout << "❌ 深度转换异常" << endl;
    }
}
*/
int main(int argc,char**argv){
    VideoCapture capture("~/Downloads/color_object.mp4");
    if(!capture.isOpened()){
        printf("COULD NOT LOAD VIDEO!");
        return -1;
    }
    namedWindow("CAMSHIFT DEMO",WINDOW_AUTOSIZE);
    Mat frame,hue,mask,hist,backproj;
    Mat hsv;
    capture.read(frame);
    bool init = true;
    Rect trackWindow;
    int hsize = 16;
    float hranges[] = {0,180};
    const float* ranges = hranges;
    Rect selection = selectROI("CAMSHIFT DEMO",frame,true,false);
    while(true){
        bool ret = capture.read(frame);
        if(!ret)break;
        cvtColor(frame,hsv,COLOR_BGR2HSV);
        inRange(hsv,Scalar(26,43,46),Scalar(34,255,255),mask);
        int ch[] = {0,0};
        hue.create(hsv.size(),hsv.depth());
        mixChannels(&hsv,1,&hue,1,ch,1);
        if(init){
            Mat roi(hue,selection),maskroi(mask,selection);
            calcHist(&roi,1,0,maskroi,hist,1,&hsize,&ranges);
            normalize(hist,hist,0,255,NORM_MINMAX);
            trackWindow = selection;
            init = false;
        }

        //meanshift
        calcBackProject(&hue,1,0,hist,backproj,&ranges);
        backproj &= mask;
        //meanShift(backproj,trackWindow,TermCriteria(TermCriteria::COUNT|TermCriteria::EPS,10,1));
        cv::RotatedRect rrt = CamShift(backproj,trackWindow,TermCriteria(TermCriteria::COUNT|TermCriteria::EPS,10,1));
        //rectangle(frame,trackWindow,Scalar(0,0,255),3,LINE_AA);
        ellipse(frame,rrt,cv::Scalar(255,0,0),2,8);

        imshow("CAMSHIFT DEMO",frame);

        char c = waitKey(1);
        if(c ==27){
            break;
        }
    }
    capture.release();
    destroyAllWindows();
    return 0;
}
#endif
