#include "gamepad.h"

#include <QApplication>
//#undef slots
//#include <Python.h>
//#define slots Q_SLOTS
//#include "opencv2/core/core.hpp"
//#include "opencv2/opencv.hpp"
//#include "opencv2/highgui/highgui.hpp"
//#include <utils.h>
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

//    QImage img(":/res/shine_template.jpg");
//    cv::Mat captureFrame = cv::imread("/Users/rabbit/Downloads/capture test.jpg");
//    cv::Mat shineTemplate = Utils::QImage2cvMat(img);
//    cv::Mat shineTemplate2;
//    cv::cvtColor(shineTemplate, shineTemplate2, cv::COLOR_BGR2RGB);
//    cv::Mat dstImg;
//    dstImg.create(captureFrame.dims, captureFrame.size, captureFrame.type());
//    cv::matchTemplate(captureFrame, shineTemplate2, dstImg, 0);
//    cv::Point minPoint;
//    cv::Point maxPoint;
//    double *minVal = 0;
//    double *maxVal = 0;
//    cv::minMaxLoc(dstImg, minVal, maxVal, &minPoint,&maxPoint);
//    maxPoint = cv::Point(minPoint.x + shineTemplate2.cols, minPoint.y + shineTemplate2.rows);
//    cv::rectangle(captureFrame, minPoint, maxPoint, cv::Scalar(0,255,0), 2, 8);
//    cv::imshow("123", captureFrame);
//    dstImg.release();
//    captureFrame.release();
//    shineTemplate2.release();
//    shineTemplate.release();

    QApplication a(argc, argv);
    Player p;
    Gamepad w;
    w.show();

    return a.exec();
}
