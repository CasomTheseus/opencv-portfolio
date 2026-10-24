//
// Created by Theseus on 25-7-23.
//
#include "quick_opencv.h"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

//第一课:读取并显示图像
//第二课:图像色彩空间转换
void QuickDemo::color_space_Demo(Mat &image) {
    Mat gray,hsv;
    cvtColor(image,hsv,COLOR_BGR2HSV);
    //H:0~180;S,V也是0～255，H,S表示颜色，V表示亮度
    cvtColor(image,gray,COLOR_BGR2GRAY);
    //B,G,R都是0～255的取值范围
    imshow("HSV IMAGE",hsv);
    imshow("GRAY",gray);
    imwrite("~/Desktop/资料/opencv/Izawa_Hsv.png",hsv);
    imwrite("~/Desktop/资料/opencv/Izawa_Gray.png",gray);
}

//第三课:图像对象的创建与赋值
void QuickDemo::mat_creation_demo(Mat &image) {
    /*
    Mat m1,m2;
    m1 = image.clone();
    image.copyTo(m2);
     */

    //创建空白图像
    Mat m3 = Mat::ones(Size(400,400),CV_8UC3);//8*8的，8位的unsigned char(无符号的)X通道的Mat
    //获取图像的宽度
    m3 = Scalar(0,0,255);
    std::cout<<"Width:"<<m3.cols<<"Height:"<<m3.rows<<"Channels:"<<m3.channels()<<std::endl;
    std::cout<<m3<<std::endl;

    Mat m4;
    m3.copyTo(m4);
    m4 = Scalar(0,255,255);
    imshow("Image3",m3);
    imshow("Image4",m4);
}

//第四课:图像像素的访问和读写
void QuickDemo::pixel_visit_demo(Mat &image) {
    int w = image.cols;
    int h = image.rows;
    int dims = image.channels();

    /*
    for(int row = 0;row<h;row++){
        for(int col = 0;col<w;col++){
            if(dims == 1){
                int pv = image.at<uchar>(row,col);
                image.at<uchar>(row,col) = 255 - pv;
            }
            if(dims == 3){
                Vec3b bgr = image.at<Vec3b>(row,col);
                image.at<Vec3b>(row,col)[0] = 255 - bgr[0];
                image.at<Vec3b>(row,col)[1] = 255 - bgr[1];
                image.at<Vec3b>(row,col)[2] = 255 - bgr[2];
            }
        }
    }
     */

    for (int row = 0;row<h;row++) {
        uchar *current_row = image.ptr<uchar>(row);

        for (int col = 0;col<w;col++) {
            if (dims ==1) {
                //灰度图像
                current_row[0] = 255 - current_row[0];
                current_row++;

            }
            if (dims == 3) {//彩色图像
                current_row[0] = 255 - current_row[0];
                current_row[1] = 255 - current_row[1];
                current_row[2] = 255 - current_row[2];
                current_row+=3;
            }
        }
    }
    imshow("像素读写显示：",image);
}

//第五课:图像像素的算术操作
void QuickDemo::operators_demo(Mat &image) {
    Mat dst = Mat::zeros(image.size(),image.type());
    Mat m = Mat::zeros(image.size(),image.type());
    m = Scalar(50,50,50);

    //加法
    /*
    int w = image.cols;
    int h = image.rows;
    int dims = image.channels();
    for(int row = 0;row<h;row++){
        for(int col = 0;col <w;col++){
            Vec3b p1 = image.at<Vec3b>(row,col);
            Vec3b p2 = image.at<Vec3b>(row,col);
            dst.at<Vec3b>(row,col)[0] = saturate_cast<uchar>(p1[0] + p2[0]);
            dst.at<Vec3b>(row,col)[1] = saturate_cast<uchar>(p1[1] + p2[1]);
            dst.at<Vec3b>(row,col)[2] = saturate_cast<uchar>(p1[2] + p2[2]);
        }
    }
     */
    add(image,m,dst);
    /*四则运算的api
    subtract(image,m,dst);
    divide(image,m,dst);
    multiply(image,m,dst);
    */


    imshow("PLUS OPERATION",dst);
}

//第六课:通过滚动条来动态调整图像的亮度等参数
Mat src,dst,m;

static void on_track(int b,void* userdata){
    Mat image= *((Mat*)userdata);
    Mat dst = Mat::zeros(image.size(),image.type());
    m = Mat::zeros(image.size(),image.type());
    m = Scalar(b,b,b);
    subtract(image,m,dst);
    imshow("LIGHTNESS SWITCH",dst);
}

void QuickDemo::tracking_bar_demo(Mat &image){
    namedWindow("LIGHTNESS SWITCH",WINDOW_AUTOSIZE);
    int lightness = 50;
    int max_value = 50;
    createTrackbar("ValueBar","LIGHTNESS AND CONTRAST SWITCH",
                   &lightness,max_value,on_track,(void*)(&image));
    createTrackbar("ContrastBar","LIGHTNESS AND CONTRAST SWITCH",
                   &lightness,max_value,on_track,(void*)(&image));
    on_track(50,&image);
}

//第七课：滚动条参数传递度
static void on_lightness(int b ,void*userdata) {
    Mat image = *((Mat*)userdata);
    Mat dst = Mat::zeros(image.size(),image.type());
    Mat m = Mat::zeros(image.size(),image.type());
    m = Scalar(b,b,b);
    addWeighted(image,1.0,m,0,b,dst);
    imshow("Lightness Switch",dst);
}

static void on_contrast(int b ,void*userdata) {
    Mat image = *((Mat*)userdata);
    Mat dst = Mat::zeros(image.size(),image.type());
    Mat m = Mat::zeros(image.size(),image.type());
    double contrast = b/200.0;
    addWeighted(image,1.0,m,0,b,dst);
    imshow("Lightness and Contrast Switch",dst);
}

//第八课:键盘响应
void QuickDemo::key_demo(Mat &image){
    Mat dst = Mat::zeros(image.size(),image.type());
    while(true){
        char c = waitKey(100);//如果处理视频时，必须将waitkey设置为1
        if(c == 27){
            break;
        }
        cout<<"Your key value is:"<<c<<endl;
        dst = Scalar(c,c,c);
        add(image,dst,dst);
        imshow("Add",dst);
    }
}

//第九课:opencv自带颜色表应用
void QuickDemo::color_style(Mat&image) {
    int colormap[] = {
            COLORMAP_AUTUMN,
            COLORMAP_BONE,
            COLORMAP_JET,
            COLORMAP_WINTER,
            COLORMAP_RAINBOW,
            COLORMAP_OCEAN,
            COLORMAP_SUMMER,
            COLORMAP_SPRING,
            COLORMAP_COOL,
            COLORMAP_PINK,
            COLORMAP_HOT,
            COLORMAP_PARULA,
            COLORMAP_MAGMA,
            COLORMAP_INFERNO,
            COLORMAP_PLASMA,
            COLORMAP_VIRIDIS,
            COLORMAP_CIVIDIS,
            COLORMAP_TWILIGHT,
            COLORMAP_TWILIGHT_SHIFTED
    };
    Mat dst;
    int index = 0;
    while (true) {
        int c = waitKey(400);
        if (c == 27) {
            break;
        }
        applyColorMap(image, dst, colormap[index % 19]);
        index++;
        imshow("ColorStyle",dst);
    }
}

//第十课:图像像素的逻辑操作
void QuickDemo::bitwise_demo(Mat&image){
    Mat m1 = Mat::zeros(Size(256,256),CV_8UC3);
    Mat m2 = Mat::zeros(Size(256,256),CV_8UC3);
    rectangle(m1,Rect(100,100,80,80),Scalar(255,255,0),-1,LINE_8,0);
    rectangle(m2,Rect(150,150,80,80),Scalar(0,255,255),-1,LINE_8,0);
    imshow("m1",m1);
    imshow("m2",m2);
    Mat dst = ~image;
    //bitwise_not(image,dst);
    bitwise_and(m1,m2,dst);
    imshow("Pixel Bitwise Operation",dst);

}

//第十一课:通道分离和合并
void QuickDemo::channels_demo(Mat& image) {
    std::vector<Mat> mv;
    split(image, mv);  // 分离通道：mv[0]=B, mv[1]=G, mv[2]=R

    /*// 创建全零矩阵（用于屏蔽其他通道）
    Mat zero = Mat::zeros(image.size(), CV_8UC1)

    // 显示蓝色通道（B通道保留，G和R置零）
    std::vector<Mat> blue_ch = {mv[0], zero, zero};
    Mat blue_img;
    merge(blue_ch, blue_img);
    imshow("Blue Channel", blue_img);

    // 显示绿色通道（G通道保留，B和R置零）
    std::vector<Mat> green_ch = {zero, mv[1], zero};
    Mat green_img;
    merge(green_ch, green_img);
    imshow("Green Channel", green_img);

    // 显示红色通道（R通道保留，B和G置零）
    std::vector<Mat> red_ch = {zero, zero, mv[2]};
    Mat red_img;
    merge(red_ch, red_img);
    imshow("Red Channel", red_img);
    */
    Mat dst;
    mv[0] = 0;
    merge(mv,dst);
    imshow("Red",dst);
    int from_to[] = {0,2,1,1,2,0};
    mixChannels(&image,1,&dst,1,from_to,3);
    imshow("ChannelsMixed",dst);
}

//第十二课:图像色彩空间转换
void QuickDemo::inrange_demo(Mat&image){
    Mat hsv;
    cvtColor(image,hsv,COLOR_BGR2HSV);
    Mat mask;
    inRange(hsv,Scalar(35,43,46),Scalar(77,255,255),mask);
    imshow("mask",mask);
    Mat redback = Mat::zeros(image.size(),image.type());
    redback = Scalar(40,40,200);
    bitwise_not(mask,mask);
    image.copyTo(redback,mask);
    imshow("roiRegion Fetch",redback);
}

//第十三课:图像像素值统计
void QuickDemo::pixel_statistic_demo(Mat&image){
    double minv,maxv;
    Point minLoc,maxLoc;
    std::vector<Mat> mv;
    split(image,mv);
    for(int i = 0;i<mv.size();i++){
        minMaxLoc(mv[i],&minv,&maxv,&minLoc,&maxLoc,Mat());
        //minMaxLoc的第一个参数必须是单通道的图像；面对多通道图像时先分离通道再使用minMaxLoc.
        std::cout<<"No."<<i<<" Channels:"<<"\tmin value:"<<minv<<"\tmax value:"<<maxv<<std::endl;
    }
    Mat mean,stddev;
    meanStdDev(image,mean,stddev);
    std::cout<<"means:\n"<<mean<<"\nstddev:\n"<<stddev<<std::endl;

}

//第十四课:图像几何形状绘制
//注意:opencv中，x和y的起点是左上角，终点为右下角
void QuickDemo::drawing_demo(Mat&image){
    cv::Size size = image.size();
    std::cout<<"height:"<<size.height<<"\twidth:"<<size.width<<endl;

    Rect rect;
    rect.x = 100;//x：从左往右
    rect.y = 100;//y：从上往下
    rect.width = 450;
    rect.height = 470;
    //rectangle(image,rect,Scalar(255,255,255),-1,8,0);

    //用白色圆挡脸并画红叉
    circle(image,Point(325,335),250,Scalar(255,255,255),-1,8,0);
    line(image,Point(100,100),Point(550,570),Scalar(0,0,255),20,8,0);
    line(image,Point(100,570),Point(550,100),Scalar(0,0,255),20,8,0);

    //另画一幅图并叠加在原图上
    Mat bg = Mat::zeros(image.size(),image.type());
    rectangle(bg,rect,Scalar(0,0,255),-1,8,0);
    circle(bg,Point(350,400),15,Scalar(255,0,0),-1,8,0);
    line(bg,Point(100,100),Point(350,400),Scalar(0,255,0),4,8,0);
    RotatedRect rrt;
    rrt.center = Point(200,200);
    rrt.size = Size(100,200);
    rrt.angle = 90.0;
    ellipse(bg,rrt,Scalar(0,255,255),2,8);
    Mat dst;
    addWeighted(image,0.8,bg,0.2,0,dst);
    imshow("Drawing Image",image);
}

//第十五课:随机数与随机颜色
void QuickDemo::random_drawing(Mat&image){
    Mat canvas = Mat::zeros(Size(1024,1024),image.type());
    int w = canvas.cols;
    int h = canvas.rows;
    RNG rng(12345);
    while(true){
        int c = waitKey(100);
        if (c == 27)
                break;
        int x1 = rng.uniform(0,w);
        int y1 = rng.uniform(0,h);
        int x2 = rng.uniform(0,w);
        int y2 = rng.uniform(0,h);
        int b = rng.uniform(0,255);
        int g = rng.uniform(0,255);
        int r = rng.uniform(0,255);
        canvas = Scalar(0,0,0);
        line(canvas,Point(x1,y1),Point(x2,y2),Scalar(b,g,r),1,LINE_AA,0);
        imshow("Random drawing show",canvas);
    }
}

//第十六课:多边形绘制与填充
void QuickDemo::polyline_drawing_demo(Mat&image){
    Mat canvas = Mat::zeros(Size(512,512),CV_8UC3);
    Point p1(100,100);
    Point p2(350,100);
    Point p3(450,280);
    Point p4(320,450);
    Point p5(80,400);
    std::vector<Point>pts;
    pts.push_back(p1);
    pts.push_back(p2);
    pts.push_back(p3);
    pts.push_back(p4);
    pts.push_back(p5);
    //polylines(canvas,pts,true,Scalar(0,0,255),2,8,0);
    //fillPoly(canvas,pts,Scalar(114,114,114),8,0);
    std::vector<std::vector<Point>>contours;
    contours.push_back(pts);
    drawContours(canvas,contours,-1,Scalar(255,0,0),-1);
    imshow("Polylines Drawing",canvas);
}

//第十七课:鼠标操作与响应
Point sp(-1,-1);
Point ep(-1,-1);
Mat temp;
static void on_draw(int event,int x,int y,int flags,void *userdata) {
    Mat image = *(Mat *) userdata;
    if (event == EVENT_LBUTTONDOWN) {
        sp.x = x;
        sp.y = y;
        std::cout << "start point" << sp << std::endl;
    } else if (event == EVENT_LBUTTONUP) {
        ep.x = x;
        ep.y = y;
        int dx = ep.x - sp.x;
        int dy = ep.y - sp.y;
        if (dx > 0 && dy > 0) {
            Rect box(sp.x, sp.y, dx, dy);
            imshow("ROI Region",image(box));
            rectangle(image, box, Scalar(0, 0, 255), 2, 8, 0);
            imshow("Mouse Drawing", image);
            //RESET,ready for next drawing
            sp.x = -1;
            sp.y = -1;
        }
    } else if (event == EVENT_MOUSEMOVE) {
        if (sp.x > 0 && sp.y > 0) {
            ep.x = x;
            ep.y = y;
            int dx = ep.x - sp.x;
            int dy = ep.y - sp.y;
            if (dx > 0 && dy > 0) {
                Rect box(sp.x, sp.y, dx, dy);
                temp.copyTo(image);
                rectangle(image, box, Scalar(0, 0, 255), 2, 8, 0);
                imshow("Mouse Drawing", image);
            }
        }
    }
}

void QuickDemo::mouse_drawing_demo(Mat&image) {
    namedWindow("Mouse Drawing", WINDOW_AUTOSIZE);
    setMouseCallback("Mouse Drawing", on_draw, (void *) (&image));
    imshow("Mouse drawing", image);
    temp = image.clone();
}

//第十八课:图像像素类型转换与归一化
//用于改变数据的数值范围，用于图像增强，特征提取，深度学习等内容
//我们只知道，opencv是很难的。但是具体有多难呢，我学了一年，也没有搞清楚。
void QuickDemo::norm_demo(Mat&image){
    Mat dst;
    std::cout<<image.type()<<std::endl;
    image.convertTo(image,CV_32F);
    std::cout<<image.type()<<std::endl;
    normalize(image,dst,1.0,0,NORM_MINMAX);
    std::cout<<dst.type()<<std::endl;
    imshow("Image Data Normalize",dst);
}

//第十九课:图像放缩与插值
//用于几何变换，透视变换，插值计算新像素等等内容
void QuickDemo::resize_demo(Mat&image){
    Mat zoomin,zoomout;
    int h = image.rows;
    int w = image.cols;
    resize(image,zoomin,Size(w/2,h/2),0,0,INTER_LINEAR);
    imshow("Zoomin",zoomin);
    resize(image,zoomout,Size(w*1.5,h*1.5),0,0,INTER_LINEAR);
    imshow("Zoomout",zoomout);
}

//第二十课:图像翻转
void QuickDemo::flip_demo(Mat&image){
    Mat dst;
    flip(image,dst,-1);
    //0为上下翻转，-1是180度翻转，1是左右翻转
    imshow("Image flip",dst);
}

//第二十一课:图像旋转
void QuickDemo::rotate_demo(Mat&image){
    Mat dst,M;
    int w = image.cols;
    int h = image.rows;
    M = getRotationMatrix2D(Point2f(w / 2,h / 2),45,1.0);
    double cos = abs(M.at<double>(0,0));
    double sin = abs(M.at<double>(0,1));
    int nw = cos*w + sin*h;
    int nh = sin*w + cos*h;
    M.at<double>(0,2) +=(nw/2-w/2);
    M.at<double>(1,2) +=(nh/2-h/2);
    warpAffine(image,dst,M,Size(nw,nh),INTER_LINEAR,0,Scalar(0,255,255));
    imshow("Rotation Demo", dst);
}

//第二十二及二十三课:视频文件摄像头使用及视频处理与保存
void QuickDemo::video_demo(Mat &image){
    VideoCapture capture("~/Desktop/资料/截屏/录屏2026-04-21 18.16.17.mov");
    int frame_width = capture.get(CAP_PROP_FRAME_WIDTH);
    int frame_height = capture.get(CAP_PROP_FRAME_HEIGHT);
    int count = capture.get(CAP_PROP_FRAME_COUNT);
    double fps = capture.get(CAP_PROP_FPS);
    std::cout<<"frame width:"<<frame_width<<std::endl;
    std::cout<<"frame height:"<<frame_height<<std::endl;
    std::cout<<"Number of frames:"<<count<<std::endl;
    std::cout<<"fps"<<fps<<std::endl;
    VideoWriter writer("~/Desktop/资料/截屏/test.mov",capture.get(CAP_PROP_FOURCC),fps,Size(frame_width,frame_height),true);
    Mat frame;
    while(true){
        capture.read(frame);
        flip(frame,frame,1);
        if(frame.empty()){
            break;
        }
        writer.write(frame);
        imshow("frame",frame);
        color_space_Demo(frame);
        int c = waitKey(10);
        if(c==27){
            break;
        }
    }
}

Mat original_image;  // 存储原始图像
void QuickDemo::onMouse(int event, int x, int y, int flags, void* userdata) {
        QuickDemo* self = static_cast<QuickDemo*>(userdata);

        if (event == EVENT_MOUSEWHEEL) {
            // 获取滚轮方向（Windows和Mac可能有差异）
            int wheel_direction = getMouseWheelDelta(flags);

            // 调整缩放比例（限制范围）
            if (wheel_direction > 0) {
                self->scale = std::min(self->scale + self->scale_step, 5.0);  // 放大上限5倍
            } else {
                self->scale = std::max(self->scale - self->scale_step, 0.2);  // 缩小下限0.2倍
            }

            // 实时更新显示
            self->updateZoomedImage(x, y);
        }
    }

    // 辅助函数：获取滚轮方向（兼容不同平台）
int getMouseWheelDelta(int flags) {
#ifdef _WIN32
        return -(short)((flags >> 16) & 0xFFFF);  // Windows
#else
        return (flags > 0) ? 1 : -1;  // Mac/Linux简化处理
#endif
    }

void QuickDemo::updateZoomedImage(int center_x, int center_y) {
        if (original_image.empty()) return;

        Mat zoomed;
        Size new_size(original_image.cols * scale, original_image.rows * scale);

        // 进行缩放（使用高质量插值）
        resize(original_image, zoomed, new_size, 0, 0, INTER_LANCZOS4);

        // 计算显示区域（以鼠标位置为中心）
        Rect display_roi(
                std::max(0, int(center_x * scale - original_image.cols/2)),
                std::max(0, int(center_y * scale - original_image.rows/2)),
                original_image.cols, original_image.rows
        );

        // 确保ROI不越界
        display_roi = display_roi & Rect(0, 0, zoomed.cols, zoomed.rows);

        // 显示缩放后的区域
        imshow("Zoomable Image", zoomed(display_roi));
    }

void QuickDemo::mouse_zoom_demo(Mat&image) {
        original_image = image.clone();
        namedWindow("Zoomable Image", WINDOW_AUTOSIZE);
        setMouseCallback("Zoomable Image", onMouse, this);
        imshow("Zoomable Image", image);
}

//Standard Definition   SD标清
//High Definition   HD高清
//Full High Definition  FHD全高清
//Ultra High Definition UHD超高清

//第二十四课:图像直方图
void QuickDemo::showHistogram(Mat&image){
    //三个通道分离
    std:: vector <Mat>bgr_plane;
    split(image,bgr_plane);
    //定义参数变量
    const int channels[1] = {0};
    const int bins[1] = {256};
    float hranges[2] = {0,255};
    const float* ranges[1] = {hranges};
    Mat b_hist;
    Mat g_hist;
    Mat r_hist;
    //计算Blue,Green,Red通道的直方图
    calcHist(&bgr_plane[0],1,0,Mat(),b_hist,1,bins,ranges);
    calcHist(&bgr_plane[1],1,0,Mat(),g_hist,1,bins,ranges);
    calcHist(&bgr_plane[2],1,0,Mat(),r_hist,1,bins,ranges);
    //显示直方图
    int hist_w = 512;
    int hist_h = 400;
    int bin_w = cvRound((double)hist_w/bins[0]);
    Mat histImage = Mat::zeros(hist_h,hist_w,CV_8UC3);
    //归一化直方图的数据
    normalize(b_hist,b_hist,0,histImage.rows,NORM_MINMAX,-1,Mat());
    normalize(g_hist,g_hist,0,histImage.rows,NORM_MINMAX,-1,Mat());
    normalize(r_hist,r_hist,0,histImage.rows,NORM_MINMAX,-1,Mat());
    //绘制直方图曲线
    for (int i = 1;i<bins[0];i++){
        line(histImage,Point(bin_w*(i-1),hist_h-cvRound(b_hist.at<float>(i-1))),
             Point(bin_w*(i),hist_h-cvRound(b_hist.at<float>(i))),Scalar(255,0,0),2,8,0);
        line(histImage,Point(bin_w*(i-1),hist_h-cvRound(b_hist.at<float>(i-1))),
             Point(bin_w*(i),hist_h-cvRound(g_hist.at<float>(i))),Scalar(0,255,0),2,8,0);
        line(histImage,Point(bin_w*(i-1),hist_h-cvRound(b_hist.at<float>(i-1))),
             Point(bin_w*(i),hist_h-cvRound(r_hist.at<float>(i))),Scalar(0,0,255),2,8,0);
    }
    namedWindow("Histogram Demo",WINDOW_AUTOSIZE);
    imshow("Histogram Demo",histImage);
}

//第二十五课:二维直方图
void QuickDemo::histogram_2d_demo(Mat& image) {
    // 1. 转换为HSV颜色空间（Hue色调, Saturation饱和度）
    Mat hsv;
    cvtColor(image, hsv, COLOR_BGR2HSV);

    // 2. 设置2D直方图参数
    const int h_bins = 30;  // 色调分30个区间
    const int s_bins = 32;  // 饱和度分32个区间
    const int hist_bins[] = {h_bins, s_bins};

    // 值范围（H:0-180, S:0-256）
    const float h_range[] = {0, 180};
    const float s_range[] = {0, 256};
    const float* ranges[] = {h_range, s_range};

    // 使用前两个通道（H和S通道）
    const int channels[] = {0, 1};

    // 3. 计算2D直方图
    Mat hs_hist;
    calcHist(&hsv,
             1,          // 单张图像
             channels,   // 使用的通道
             Mat(),      // 不使用掩膜
             hs_hist,    // 输出直方图
             2,          // 2D直方图
             hist_bins,  // 每个维度的bin数量
             ranges,     // 值范围
             true,       // 直方图均匀化
             false);     // 不累积

    // 4. 归一化直方图（0-255）
    double max_val = 0;
    minMaxLoc(hs_hist, 0, &max_val, 0, 0);

    // 5. 可视化直方图
    const int scale = 10;  // 每个bin的像素大小
    Mat hist_image = Mat::zeros(s_bins * scale, h_bins * scale, CV_8UC3);

    for (int h = 0; h < h_bins; h++) {
        for (int s = 0; s < s_bins; s++) {
            float bin_val = hs_hist.at<float>(h, s);
            int intensity = cvRound(bin_val * 255 / max_val);  // 归一化到0-255
            // 绘制矩形表示bin值
            rectangle(hist_image,
                      Point(h * scale, s * scale),
                      Point((h + 1) * scale - 1, (s + 1) * scale - 1),
                      Scalar::all(intensity),  // 灰度颜色
                      FILLED);                // 实心矩形
        }
    }

    // 6. 显示和保存结果
    applyColorMap(hist_image,hist_image,COLORMAP_JET);//将灰度图像转换成彩色，更加直观
    imshow("H-S 2D Histogram", hist_image);

    // 添加可选的保存路径检查
    /*
    std::string save_path = "~/Desktop/opencv/hist2d_image.png";
    if (imwrite(save_path, hist_image)) {
        std::cout << "Histogram saved to: " << save_path << std::endl;
    } else {
        std::cerr << "Failed to save histogram!" << std::endl;
    }
     */
}

//第二十六课:直方图均衡化
void QuickDemo::histogram_eq_demo(Mat &image) {
    //关键api ：equalizeHist(src,dst)，只支持单通道图像。
    Mat gray;
    cvtColor(image,gray,COLOR_BGR2GRAY);
    imshow("GRAY IMAGE",gray);

    Mat dst;
    equalizeHist(gray,dst);
    imshow("HISTOGRAM EQUALIZATION",dst);
}

//第二十七课:图像卷积操作
void QuickDemo::blur_demo(Mat &image){
    Mat dst;
    blur(image,dst,Size(31,31),Point(-1,-1),BORDER_DEFAULT);
    imshow("BLUR IMAGE",dst);
}

//第二十八课:高斯模糊
void QuickDemo::gaussian_blur_demo(Mat&image){
    //核心函数：GaussianBlur(),sigma()
    Mat dst;
    GaussianBlur(image,dst,Size(0,0),1);
    imshow("GAUSSIANBLUR DEMONSTRATION",dst);
}

//第二十九课:高斯双边模糊
void QuickDemo::bifilter_demo(Mat&image){
    Mat dst;
    bilateralFilter(image,dst,0,50,30);
    //sigmacolor:10～20极轻度去噪,30~50自然磨皮,60~80明显磨皮,100~150重度磨皮,200+卡通效果
    //sigmaspace:3～5局部微调，8～12适度平滑，15～20大范围平滑，30+全局平滑
    imshow("BiBlUR DEMONSTRATION",dst);
}