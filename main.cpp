#include "gamepad.h"

#include <QApplication>
//#undef slots
//#include <Python.h>
//#define slots Q_SLOTS
//#include "opencv2/core/core.hpp"
//#include "opencv2/opencv.hpp"
//#include "opencv2/highgui/highgui.hpp"
#include <player.h>

int main(int argc, char *argv[])
{
//    cv::VideoCapture videoCapture = cv::VideoCapture(0);
//    videoCapture.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
//    videoCapture.set(cv::CAP_PROP_FRAME_HEIGHT, 720);
//    videoCapture.set(cv::CAP_PROP_FPS, 30);
//    cv::Mat videoFrame;
//    while(true){
//        videoCapture >> videoFrame;
//        if(videoFrame.empty()){
//            break;
//        }
//        imshow("video demo", videoFrame);
//        cv::waitKey(33);
//    }

//    videoCapture.release();
//    cv::destroyAllWindows();

//    cv::Mat img1 = cv::imread("/Users/rabbit/Downloads/capture test.jpg");
//    cv::Mat img2 = cv::imread("/Users/rabbit/Downloads/capture.jpg");
//    cv::Mat dstImg;
//    dstImg.create(img1.dims,img1.size,img1.type());
//    cv::matchTemplate(img1, img2, dstImg, 0);
//    cv::Point minPoint;
//    cv::Point maxPoint;
//    double *minVal = 0;
//    double *maxVal = 0;
//    cv::minMaxLoc(dstImg, minVal, maxVal, &minPoint,&maxPoint);
//    cv::rectangle(img1, minPoint, cv::Point(minPoint.x + img2.cols, minPoint.y + img2.rows),  	cv::Scalar(0,255,0), 2, 8);
//    cv::imshow("123", img1);

    QApplication a(argc, argv);
    Player p;
    Gamepad w;
    w.show();

    return a.exec();
}
