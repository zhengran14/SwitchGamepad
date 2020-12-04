#include "gamepad.h"

#include <QApplication>
//#undef slots
//#include <Python.h>
//#define slots Q_SLOTS
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

int main(int argc, char *argv[])
{

    cv::Mat image=cv::imread("/Users/rabbit/Downloads/IMG_2444.JPG");
    cv::namedWindow("aaa");
    cv::imshow("myimage",image);

    QApplication a(argc, argv);
    Gamepad w;
    w.show();
    return a.exec();
}
