#include "videocapture.h"
#include <QCameraInfo>
#include <QLayout>

VideoCapture::VideoCapture(QObject *parent) : QObject(parent)
{
    metaEnum = QMetaEnum::fromType<PixelFormat>();
    viewfinder = new QCameraViewfinder();
}

VideoCapture::~VideoCapture()
{
    close();
    if (viewfinder != Q_NULLPTR) {
        viewfinder->deleteLater();
        viewfinder = Q_NULLPTR;
    }
}

//void VideoCapture::init(QLayout *layout)
//{
////    viewfinder = new QCameraViewfinder(layout->parentWidget());
////    layout->addWidget(viewfinder);
//}

void VideoCapture::open(int index, QString resolution, QString frameRateRange, QString pixelFormat)
{
    close();
    if (cameraList.count() <= 0) {
        return;
    }
    camera = new QCamera(cameraList[index], this->parent());  //分配内存空间
//    imageCapture = new QCameraImageCapture(camera);
    //设置采集目标
//    imageCapture->setCaptureDestination(QCameraImageCapture::CaptureToFile);
    //设置采集模式
//    camera->setCaptureMode(QCamera::CaptureStillImage);//将其采集为图片
//    camera->setCaptureMode(QCamera::CaptureMode::CaptureViewfinder);//将其采集到取景器中
    camera->setCaptureMode(QCamera::CaptureViewfinder);

    QCameraViewfinderSettings viewfinderSettings;
    if (!resolution.isEmpty()) {
        QStringList _resolution = resolution.split("x");
        if (_resolution.count() == 2) {
            viewfinderSettings.setResolution(_resolution[0].toInt(), _resolution[1].toInt());
        }
    }
    if (!pixelFormat.isEmpty()) {
        PixelFormat _pixelFormat = (PixelFormat)pixelFormat.toInt();
        if (!QString(metaEnum.key(_pixelFormat)).isEmpty() && _pixelFormat != PixelFormat::Format_Invalid) {
            viewfinderSettings.setPixelFormat((QVideoFrame::PixelFormat)_pixelFormat);
        }
    }
    if (!frameRateRange.isEmpty()) {
        QStringList _frameRateRange = frameRateRange.split("~");
        if (_frameRateRange.count() == 2) {
            viewfinderSettings.setMinimumFrameRate(_frameRateRange[0].toDouble());
            viewfinderSettings.setMaximumFrameRate(_frameRateRange[1].toDouble());
        }
    }
    camera->setViewfinderSettings(viewfinderSettings);

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

QStringList VideoCapture::GetSupportedResolutions(int index)
{
    QStringList list;
    QCamera _camera(cameraList[index]);
    _camera.load();
    for (QSize resolution : _camera.supportedViewfinderResolutions()) {
        list << (QString::number(resolution.width()) + "x" + QString::number(resolution.height()));

    }
//    for (auto setting : _camera.supportedViewfinderSettings()) {
//        qDebug() << setting.resolution() << setting.minimumFrameRate() << setting.maximumFrameRate() << setting.pixelFormat() << setting.pixelAspectRatio();
//    }
    _camera.unload();
    _camera.deleteLater();
    return list;
}

QStringList VideoCapture::GetSupportedFrameRateRanges(int index)
{
    QStringList list;
    QCamera _camera(cameraList[index]);
    _camera.load();
    for (QCamera::FrameRateRange frameRateRange : _camera.supportedViewfinderFrameRateRanges()) {
        list << (QString::number(frameRateRange.minimumFrameRate) + "~" + QString::number(frameRateRange.maximumFrameRate));

    }
    _camera.unload();
    _camera.deleteLater();
    return list;
}

QStringList VideoCapture::GetSupportedPixelFormats(int index)
{
    QStringList list;
    QCamera _camera(cameraList[index]);
    _camera.load();
    for (QVideoFrame::PixelFormat pixelFormat : _camera.supportedViewfinderPixelFormats()) {
        list << metaEnum.key(pixelFormat);
    }
    _camera.unload();
    _camera.deleteLater();
    return list;
}

void VideoCapture::moveViewfinder(QLayout *layout)
{
    if (viewfinder != Q_NULLPTR) {
        QWidget *parent = (QWidget*)viewfinder->parent();
        if (parent != Q_NULLPTR) {
            parent->layout()->removeWidget(viewfinder);
        }
        layout->addWidget(viewfinder);
    }
}

QCameraViewfinder *VideoCapture::getViewfinder()
{
    return viewfinder;
}
