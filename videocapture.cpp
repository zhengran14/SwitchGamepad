#include "videocapture.h"
#include <QCameraInfo>
#include <QLayout>
#include <QEventLoop>

VideoCapture::VideoCapture(QObject *parent)
    : QObject(parent)
    , viewfinder()
{
    metaEnum = QMetaEnum::fromType<PixelFormat>();
}

VideoCapture::~VideoCapture()
{
    close();
    viewfinder.deleteLater();
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
    cameraImageCapture = new QCameraImageCapture(camera);
    //设置采集目标
//    cameraImageCapture->setCaptureDestination(QCameraImageCapture::CaptureToFile);
    cameraImageCapture->setCaptureDestination(QCameraImageCapture::CaptureToBuffer);
//    cameraImageCapture->setBufferFormat(QVideoFrame::PixelFormat::Format_Jpeg);
//    QImageEncoderSettings imageEncoderSettings;
//    imageEncoderSettings.setResolution(1280,720);
//    cameraImageCapture->setEncodingSettings(imageEncoderSettings);
    connect(cameraImageCapture, &QCameraImageCapture::imageAvailable, this, &VideoCapture::imageAvailable);
    connect(cameraImageCapture,
            QOverload<int, QCameraImageCapture::Error, const QString &>::of(&QCameraImageCapture::error),
            [=](int id, QCameraImageCapture::Error error, const QString &errorString){
        qDebug() << errorString;
    });
    //设置采集模式
    camera->setCaptureMode(QCamera::CaptureStillImage);//将其采集为图片
//    camera->setCaptureMode(QCamera::CaptureMode::CaptureViewfinder);//将其采集到取景器中
//    camera->setCaptureMode(QCamera::CaptureViewfinder);

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
    camera->setViewfinder(&viewfinder);
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
    if (cameraImageCapture != Q_NULLPTR) {
        cameraImageCapture->deleteLater();
        cameraImageCapture = Q_NULLPTR;
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

QStringList VideoCapture::GetSupportedResolutions(int index, QString &defaultName, QString defaultSearch)
{
    QStringList list;
    QCamera _camera(cameraList[index]);
    _camera.load();
    for (QSize resolution : _camera.supportedViewfinderResolutions()) {
        QString res = (QString::number(resolution.width()) + "x" + QString::number(resolution.height()));
        list << res;
        if (defaultName.isEmpty() && !defaultSearch.isEmpty() && res.contains(defaultSearch)) {
            defaultName = res;
        }
    }
//    for (auto setting : _camera.supportedViewfinderSettings()) {
//        qDebug() << setting.resolution() << setting.minimumFrameRate() << setting.maximumFrameRate() << setting.pixelFormat() << setting.pixelAspectRatio();
//    }
    _camera.unload();
    _camera.deleteLater();
    return list;
}

QStringList VideoCapture::GetSupportedFrameRateRanges(int index, QString &defaultName, QString defaultSearch)
{
    QStringList list;
    QCamera _camera(cameraList[index]);
    _camera.load();
    for (QCamera::FrameRateRange frameRateRange : _camera.supportedViewfinderFrameRateRanges()) {
        QString frame = (QString::number(frameRateRange.minimumFrameRate) + "~" + QString::number(frameRateRange.maximumFrameRate));
        list << frame;
        if (defaultName.isEmpty() && !defaultSearch.isEmpty() && frame.contains(defaultSearch)) {
            defaultName = frame;
        }
    }
    _camera.unload();
    _camera.deleteLater();
    return list;
}

QStringList VideoCapture::GetSupportedPixelFormats(int index, QString &defaultName, QString defaultSearch)
{
    QStringList list;
    QCamera _camera(cameraList[index]);
    _camera.load();
    for (QVideoFrame::PixelFormat pixelFormat : _camera.supportedViewfinderPixelFormats()) {
        QString format = metaEnum.key(pixelFormat);
        list << format;
        if (defaultName.isEmpty() && !defaultSearch.isEmpty() && format.contains(defaultSearch)) {
            defaultName = format;
        }
    }
    _camera.unload();
    _camera.deleteLater();
    return list;
}

void VideoCapture::moveViewfinder(QLayout *layout)
{
    removeViewfinder();
    layout->addWidget(&viewfinder);
}

void VideoCapture::removeViewfinder()
{
    QWidget *parent = (QWidget*)viewfinder.parent();
    if (parent != Q_NULLPTR) {
        parent->layout()->removeWidget(&viewfinder);
    }
}

QCameraViewfinder *VideoCapture::getViewfinder()
{
    return &viewfinder;
}

QImage *VideoCapture::capture()
{
    if (cameraImageCapture != Q_NULLPTR && camera != Q_NULLPTR) {
        QEventLoop* eventLoop = new QEventLoop();
        camera->searchAndLock();
        connect(cameraImageCapture, &QCameraImageCapture::imageAvailable, eventLoop, &QEventLoop::quit);
        cameraImageCapture->capture();
        camera->unlock();
        eventLoop->exec();
        disconnect(cameraImageCapture, &QCameraImageCapture::imageAvailable, eventLoop, &QEventLoop::quit);
        eventLoop->deleteLater();
        return videoFrame;
    }
    return Q_NULLPTR;
}

void VideoCapture::imageAvailable(int id, const QVideoFrame &frame)
{
//    frame.image().save("123.jpg");
    if (videoFrame != Q_NULLPTR) {
        delete videoFrame;
        videoFrame = Q_NULLPTR;
    }
    videoFrame = new QImage(frame.image());
}
