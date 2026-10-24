//
// Created by Theseus on 2025/9/2.
//
//实践是最好的理论
//理论加实践才能学好一切学科
//内容概述：DNN模块：Deep Neural Network，支持自定义对象检测，支持人脸检测
//不建议用HAAR人脸检测
//SSD对象检测
//#define RUN_MAIN
#ifdef RUN_MAIN
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <iostream>
#include <fstream>

using namespace cv;
using namespace dnn;
using namespace std;

// 加载类别标签
vector<string> loadClassLabels(const string& filename) {
    vector<string> classes;
    ifstream ifs(filename);
    string line;
    while (getline(ifs, line)) {
        classes.push_back(line);
    }
    return classes;
}

int main() {
    // 1. 加载模型
    string modelConfig = "~/Desktop/opencv480_test/2/models/deploy.prototxt";
    string modelWeights = "~/Desktop/opencv480_test/2/models/mobilenet_iter_73000.caffemodel";
    string classFile = "~/Desktop/opencv480_test/2/models/classes.txt";

    Net net = readNetFromCaffe(modelConfig, modelWeights);
    net.setPreferableBackend(DNN_BACKEND_OPENCV);
    net.setPreferableTarget(DNN_TARGET_CPU);

    vector<string> classes = loadClassLabels(classFile);

    // 2. 加载图像
    Mat image = imread("~/Desktop/资料/opencv学习材料/dog.jpg");
    if (image.empty()) {
        cerr << "无法加载图像!" << endl;
        return -1;
    }

    // 3. 预处理
    Mat blob = blobFromImage(image, 0.007843, Size(300, 300), Scalar(127.5, 127.5, 127.5), false);
    net.setInput(blob);

    // 4. 推理
    Mat detection = net.forward();

    // 5. 后处理
    Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());

    for (int i = 0; i < detectionMat.rows; i++) {
        float confidence = detectionMat.at<float>(i, 2);

        if (confidence > 0.5) {  // 置信度阈值
            int classId = static_cast<int>(detectionMat.at<float>(i, 1));
            int left = static_cast<int>(detectionMat.at<float>(i, 3) * image.cols);
            int top = static_cast<int>(detectionMat.at<float>(i, 4) * image.rows);
            int right = static_cast<int>(detectionMat.at<float>(i, 5) * image.cols);
            int bottom = static_cast<int>(detectionMat.at<float>(i, 6) * image.rows);

            rectangle(image, Point(left, top), Point(right, bottom), Scalar(0, 255, 0), 2);
            string label = cv::format("%s: %.2f", classes[classId].c_str(), confidence);
            putText(image, label, Point(left, top - 10), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 2);
        }
    }
    imshow("SSD Detection", image);
    imwrite("~/Desktop/资料/opencv学习材料/SSD DETECTION.jpg",image);
    waitKey(0);

    return 0;
}
#endif