#include "gamepad.h"

#include <QApplication>
//#undef slots
//#include <Python.h>
//#define slots Q_SLOTS
//#include "opencv2/core/core.hpp"
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

    QApplication a(argc, argv);
    Player p;
    Gamepad w;
    w.show();

    return a.exec();
}
