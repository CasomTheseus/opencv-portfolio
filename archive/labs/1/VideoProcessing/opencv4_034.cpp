//
// Created by Theseus on 2025/8/26.
//
//#define RUN_MAIN
#ifdef RUN_MAIN
#include<opencv2/opencv.hpp>
#include<iostream>
using namespace cv;
using namespace std;
int main(int argc,char**argv){
    VideoCapture capture;
    capture.open("~/Downloads/video.avi");
    if(!capture.isOpened()){
        printf("COULD NOT OPEN THE VIDEO!");
    }
    namedWindow("FRAME",WINDOW_AUTOSIZE);
    int fps = capture.get(CAP_PROP_FPS);
    int width = capture.get(CAP_PROP_FRAME_WIDTH);
    int height = capture.get(CAP_PROP_FRAME_HEIGHT);
    int num_of_frames = capture.get(CAP_PROP_FRAME_COUNT);
    int type = capture.get(CAP_PROP_FOURCC);
    printf("frame size(width = %d,height = %d),FPS:%d,frames:%d\n",width,height,fps,num_of_frames);

    Mat frame;
    VideoWriter writer("~/Desktop/资料/图片/test.mp4",type,fps,Size(width,height),true);

    while(true){
        bool ret = capture.read(frame);
        if(!ret)break;
        imshow("FRAME",frame);
        writer.write(frame);
        char c = waitKey(5);
        if (c==27){//ESC
            break;
        }
    }
    destroyAllWindows();
    capture.release();
    writer.release();
    return 0;

}
#endif