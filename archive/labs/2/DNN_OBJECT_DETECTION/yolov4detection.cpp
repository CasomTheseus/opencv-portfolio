//
// Created by Theseus on 2025/9/3.
//
//#define RUN_MAIN
#ifdef RUN_MAIN
#include <opencv2/opencv.hpp>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>
using namespace std;
using namespace cv;
using namespace cv::dnn;
void image_detection();
string yolov4_model = "FILEPATH";
string yolov4_config = "FILEPATH";
int main(int argc,char**argv){
    image_detection;
}

void image_detection(){
    Net net = readNetFromDarknet(yolov4_config,yolov4_model);
    net.setPreferableBackend(DNN_BACKEND_OPENCV);
    net.setPreferableTarget(DNN_TARGET_CPU);
    std::vector<String>outNames = net.getUnconnectedOutLayersNames();
    for (int i = 0;i<outNames.size();i++){
        printf("OUTPUT LAYER NAME:%s\n",outNames[i].c_str());
    }

    vector<string>classNamesVec;
    ifstream classNamesFile("FILEPATH.txt");
    if(classNamesFile.is_open()){
        string className = "";
        while(std::getline(classNamesFile,className))
            classNamesVec.push_back(className);
    }

    VideoCapture capture;
    capture.open("VIDEOPATH");
    Mat frame;
    //加载图像
    while(true){
        int64 start = getTickCount();
        capture.read(frame);
        Mat inputBlob = blobFromImage(frame,1/255,Size(416,416),Scalar(),true,false);
        net.setInput(inputBlob);

        //检测
        std::vector<Mat> outs;
        net.forward(outs,outNames);

        vector<Rect>boxes;
        vector<int>classIds;
        vector<float>confidences;
        for(size_t i = 0;i<outs.size();++i){
            //detected objects and C is a number of classes +4 where the first 4
            float* data = (float*)outs[i].data;
            for(int j = 0;j<outs[i].rows;++j,data+=outs[i].cols){
                Mat scores = outs[i].row(j).colRange(5,outs[i].cols);
                Point classIdPoint;
                double confidence;
                minMaxLoc(scores,0,&confidence,0,&classIdPoint);
                if(confidence > 0.5){
                    int centerX = (int)(data[0]*frame.cols);
                    int centerY = (int)(data[1]*frame.rows);
                    int width = (int)(data[2]*frame.cols);
                    int height = (int)(data[3]*frame.rows);
                    int left = centerX - width/2;
                    int top = centerY - height/2;

                    classIds.push_back(classIdPoint.x);
                    confidences.push_back((float)confidence);
                    boxes.push_back(Rect(left,top,width,height));
                }
            }
        }
        vector<int>indices;
        NMSBoxes(boxes,confidences,0.5,0.2,indices);
        for(size_t i = 0;i<indices.size();++i){
            int idx = indices[i];
            Rect box = boxes[idx];
            String className = classNamesVec[classIds[idx]];
            putText(frame,className.c_str(),box.tl(),FONT_HERSHEY_SIMPLEX,1.0,Scalar(255,0,0),2,8);
            rectangle(frame,box,Scalar(0,0,255),2,8,0);
        }
        float fps = getTickFrequency()/(getTickCount()-start);
        float time = (getTickCount()-start)/getTickFrequency();
        ostringstream ss;
        ss<<"FPS:"<<fps<<"detection time:"<<time*1000<<"ms";
        putText(frame,ss.str(),Point(20,20),0,0.5,Scalar(0,0,255));
        imshow("YOLOV4-Detections",frame);
        char c = waitKey(0);
        if(c ==27){
            destroyAllWindows();
            break;
        }
        return;
    }
}
#endif