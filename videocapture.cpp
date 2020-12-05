#include "videocapture.h"
#include <QCameraInfo>
#include <QLayout>

VideoCapture::VideoCapture(QObject *parent) : QObject(parent)
{
}

VideoCapture::~VideoCapture()
{
    close();
    if (viewfinder != Q_NULLPTR) {
        viewfinder->deleteLater();
        viewfinder = Q_NULLPTR;
    }
}

void VideoCapture::init(QLayout *layout)
{
    viewfinder = new QCameraViewfinder(layout->parentWidget());
    layout->addWidget(viewfinder);
}

void VideoCapture::open(int index)
{
    close();
    if (cameraList.count() <= 0) {
        return;
    }
    camera = new QCamera(cameraList[index], this->parent());  //分配内存空间
    imageCapture = new QCameraImageCapture(camera);
    //设置采集目标
    imageCapture->setCaptureDestination(QCameraImageCapture::CaptureToFile);
    //设置采集模式
    camera->setCaptureMode(QCamera::CaptureStillImage);//将其采集为图片
    camera->setCaptureMode(QCamera::CaptureMode::CaptureViewfinder);//将其采集到取景器中
    //设置取景器
    camera->setViewfinder(viewfinder);
    //开启相机
    camera->start();
}

void VideoCapture::close()
{
    if (camera != Q_NULLPTR) {
        camera->stop();
        camera->deleteLater();
        camera = Q_NULLPTR;
    }
    if (imageCapture != Q_NULLPTR) {
        imageCapture->deleteLater();
        imageCapture = Q_NULLPTR;
    }
}

QStringList VideoCapture::refresh(QString defaultSearch, QString &defaultName)
{
    defaultName = "";
    cameraList.clear();
    cameraList = QCameraInfo::availableCameras();
    QStringList cameraDes;
    for (const QCameraInfo &cameraInfo : cameraList) {
        if (defaultName.isEmpty() && cameraInfo.description().contains(defaultSearch)) {
            defaultName = cameraInfo.description();
        }
        cameraDes << cameraInfo.description();
    }
    return cameraDes;
}
