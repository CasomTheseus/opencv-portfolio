//
// Created by Theseus on 25-7-30.
//
#include <opencv2/opencv.hpp>
#include <iostream>
//#define RUN_MAIN
#ifdef RUN_MAIN
using namespace cv;

void beautify_face_region(Mat &image,const Rect &face_rect){
    if (face_rect.x < 0 || face_rect.y < 0 ||
        face_rect.x + face_rect.width > image.cols ||
        face_rect.y + face_rect.height > image.rows ||
        face_rect.width <= 0 || face_rect.height <= 0) {
        return;
    }

    Mat face_roi = image(face_rect);
    Mat smoothed;

    bilateralFilter(face_roi,smoothed,0,30,10);
    smoothed.copyTo(face_roi);
}

int main(int argc,char**argv){
    std::string pb_file_path = "~/Desktop/资料/Code/opencv480_quick_tutorial/opencv_face_detector_uint8.pb";
    std::string pbtxt_file_path = "~/Desktop/资料/Code/opencv480_quick_tutorial/opencv_face_detector.pbtxt";
    cv::dnn::Net net = cv::dnn::readNetFromTensorflow(pb_file_path,pbtxt_file_path);
    VideoCapture cap(0);
    cv::Mat frame;
    while(true){
        cap.read(frame);
        if(frame.empty()){
            break;
        }
        cv::Mat blob = cv::dnn::blobFromImage(frame,1.0,Size(300,300),cv::Scalar(104,177,123),false,false);
        net.setInput(blob);
        cv::Mat probs = net.forward();
        //1x1xNx7
        cv::Mat detectMat(probs.size[2],probs.size[3],CV_32F,probs.ptr<float>());
        std::vector <Rect>faces;
        for(int row = 0;row < detectMat.rows;row++){
            float conf = detectMat.at<float>(row,2);
            if(conf > 0.5){
                float x1 = detectMat.at<float>(row,3)*frame.cols;
                float y1 = detectMat.at<float>(row,4)*frame.rows;
                float x2 = detectMat.at<float>(row,5)*frame.cols;
                float y2 = detectMat.at<float>(row,6)*frame.rows;

                x1 = std::max(0.0f, std::min(x1, (float)(frame.cols - 1)));
                y1 = std::max(0.0f, std::min(y1, (float)(frame.rows - 1)));
                x2 = std::max(0.0f, std::min(x2, (float)(frame.cols - 1)));
                y2 = std::max(0.0f, std::min(y2, (float)(frame.rows - 1)));

                if (x2 <= x1 || y2 <= y1) {
                    continue;
                }
                cv::Rect box(x1,y1,x2-x1,y2-y1);
                faces.push_back(box);
                cv::rectangle(frame,box,cv::Scalar(0,255,0),2,8);
            }
        }
        for(const auto &face_rect : faces){
            beautify_face_region(frame,face_rect);
        }
        cv::imshow("OpenCV4.8 FACE DETECTION DEMONSTRATION",frame);
        char c = waitKey(1);
        if(c == 27){
            break;
        }
    }
    cv::waitKey(0);
    cv::destroyAllWindows();
    return 0;
}
#endif