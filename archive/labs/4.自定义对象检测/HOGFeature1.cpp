//
// Created by Theseus on 2025/9/22.
//
//#define RUN_MAIN
#ifdef RUN_MAIN
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace cv::ml;
using namespace std;

string positive_dir = "D:/images/train_data/elec_watch/positive";
string negative_dir = "D:/images/train_data/elec_watch/negative";
void get_hog_descriptor(Mat &image, vector<float> &desc);
void generate_dataset(Mat &trainData, Mat &labels);
void svm_train(Mat &trainData, Mat &labels);
int main(int argc, char** argv) {
    // read data and generate dataset
    //Mat trainData = Mat::zeros(Size(3780, 26), CV_32FC1);
    //Mat labels = Mat::zeros(Size(1, 26), CV_32SC1);
    //generate_dataset(trainData, labels);

    // SVM train, and save model
    // svm_train(trainData, labels);

    // load model
    Ptr<SVM> svm = SVM::load("D:/images/train_data/elec_watch/hog_elec.xml");

    //  detect custom object
    Mat test = imread("D:/images/train_data/elec_watch/test/scene_02.jpg");
    resize(test, test, Size(0, 0), 0.2, 0.2);
    imshow("input", test);
    Rect winRect;
    winRect.width = 64;
    winRect.height = 128;
    int sum_x = 0;
    int sum_y = 0;
    int count = 0;

    // 开窗检测....
    for (int row = 64; row < test.rows-64; row += 4) {
        for (int col = 32; col < test.cols-32; col += 4) {
            winRect.x = col - 32;
            winRect.y = row - 64;
            vector<float> fv;
            get_hog_descriptor(test, fv);
            Mat one_row = Mat::zeros(Size(fv.size(), 1), CV_32FC1);
            for (int i = 0; i < fv.size(); i++) {
                one_row.at<float>(0, i) = fv[i];
            }
            float result = svm->predict(one_row);
            if (result > 0) {
                // rectangle(test, winRect, Scalar(0, 0, 255), 1, 8, 0);
                count += 1;
                sum_x += winRect.x;
                sum_y += winRect.y;
            }
        }
    }

    // 显示box
    winRect.x = sum_x / count;
    winRect.y = sum_y / count;
    rectangle(test, winRect, Scalar(255, 0, 0), 2, 8, 0);
    imshow("object detection result", test);
    imwrite("D:/case02.png", test);
    waitKey(0);
    return 0;
}

void get_hog_descriptor(Mat &image, vector<float> &desc) {
    HOGDescriptor hog;
    int h = image.rows;
    int w = image.cols;
    float rate = 64.0 / w;
    Mat img, gray;
    resize(image, img, Size(64, int(rate*h)));
    cvtColor(img, gray, COLOR_BGR2GRAY);
    Mat result = Mat::zeros(Size(64, 128), CV_8UC1);
    result = Scalar(127);
    Rect roi;
    roi.x = 0;
    roi.width = 64;
    roi.y = (128 - gray.rows) / 2;
    roi.height = gray.rows;
    gray.copyTo(result(roi));
    hog.compute(result, desc, Size(8, 8), Size(0, 0));
    // printf("desc len : %d \n", desc.size());
}
void generate_dataset(Mat &trainData, Mat &labels) {
    vector<string> images;
    glob(positive_dir, images);
    int pos_num = images.size();
    for (int i = 0; i < images.size(); i++) {
        Mat image = imread(images[i].c_str());
        vector<float> fv;
        get_hog_descriptor(image, fv);
        for (int j = 0; j < fv.size(); j++) {
            trainData.at<float>(i, j) = fv[j];
        }
        labels.at<int>(i, 0) = 1;
    }
    images.clear();
    glob(negative_dir, images);
    for (int i = 0; i < images.size(); i++) {
        Mat image = imread(images[i].c_str());
        vector<float> fv;
        get_hog_descriptor(image, fv);
        for (int j = 0; j < fv.size(); j++) {
            trainData.at<float>(i+ pos_num, j) = fv[j];
        }
        labels.at<int>(i+ pos_num, 0) = -1;
    }
}
void svm_train(Mat &trainData, Mat &labels) {
    printf("\n start SVM training... \n");
    Ptr<SVM> svm = SVM::create();
    svm->setC(2.67);
    svm->setType(SVM::C_SVC);
    svm->setKernel(SVM::LINEAR);
    svm->setGamma(5.383);
    svm->train(trainData, ROW_SAMPLE, labels);
    clog << "....[Done]" << endl;
    printf("end train...\n");

    // save xml
    svm->save("D:/images/train_data/elec_watch/hog_elec.xml");
}
#endif