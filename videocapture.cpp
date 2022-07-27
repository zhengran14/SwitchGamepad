#include "videocapture.h"
#include "QtMultimedia/qmediacapturesession.h"
#include <QCameraDevice>
#include <QMediaDevices>
#include <QLayout>
#include <QEventLoop>
#include <QImageCapture>

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
    mediaCaptureSession = new QMediaCaptureSession(this->parent());
    mediaCaptureSession->setCamera(camera);
    //设置取景器
    mediaCaptureSession->setVideoOutput(&viewfinder);
    //设置截图
    imageCapture = new QImageCapture(this->parent());
    mediaCaptureSession->setImageCapture(imageCapture);
    connect(imageCapture, &QImageCapture::imageAvailable, this, &VideoCapture::imageAvailable);
    connect(imageCapture,
            QOverload<int, QImageCapture::Error, const QString &>::of(&QImageCapture::errorOccurred),
            [=](int id, QImageCapture::Error error, const QString &errorString){
        qDebug() << errorString;
    });

    // need to adapt
//    QCameraFormat viewfinderSettings;
//    if (!resolution.isEmpty()) {
//        if (resolution.contains("default_")) {
//            resolution = resolution.remove("default_");
//        }
//        QStringList _resolution = resolution.split("x");
//        if (_resolution.count() == 2) {
//            viewfinderSettings.setResolution(_resolution[0].toInt(), _resolution[1].toInt());
//        }
//    }
//    if (!pixelFormat.isEmpty()) {
//        PixelFormat _pixelFormat = (PixelFormat)pixelFormat.toInt();
//        if (!QString(metaEnum.key(_pixelFormat)).isEmpty() && _pixelFormat != PixelFormat::Format_Invalid) {
//            viewfinderSettings.setPixelFormat((QVideoFrame::PixelFormat)_pixelFormat);
//        }
//    }
//    if (!frameRateRange.isEmpty()) {
//        if (frameRateRange.contains("default_")) {
//            frameRateRange = frameRateRange.remove("default_");
//        }
//        QStringList _frameRateRange = frameRateRange.split("~");
//        if (_frameRateRange.count() == 2) {
//            viewfinderSettings.setMinimumFrameRate(_frameRateRange[0].toDouble());
//            viewfinderSettings.setMaximumFrameRate(_frameRateRange[1].toDouble());
//        }
//    }
//    camera->setViewfinderSettings(viewfinderSettings);

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
    if (mediaCaptureSession != Q_NULLPTR) {
        mediaCaptureSession->deleteLater();
        mediaCaptureSession = Q_NULLPTR;
    }
}

QStringList VideoCapture::refresh(QString defaultSearch, QString &defaultName)
{
    defaultName = "";
    cameraList.clear();
    cameraList = QMediaDevices::videoInputs();
    QStringList cameraDes;
    for (const QCameraDevice &cameraInfo : cameraList) {
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
    QList<QCameraFormat> cameraFormats = cameraList[index].videoFormats();
    for (QCameraFormat cameraFormat : cameraFormats) {
        QString res = (QString::number(cameraFormat.resolution().width()) + "x" + QString::number(cameraFormat.resolution().height()));
        list << res;
        if (defaultName.isEmpty() && !defaultSearch.isEmpty() && res.contains(defaultSearch)) {
            defaultName = res;
        }
    }
//    for (auto setting : _camera.supportedViewfinderSettings()) {
//        qDebug() << setting.resolution() << setting.minimumFrameRate() << setting.maximumFrameRate() << setting.pixelFormat() << setting.pixelAspectRatio();
//    }
    if (list.empty()) {
        list << "" << "default_640x260" << "default_1280x720" << "default_1920x1080";
        defaultName = "default_1280x720";
    }
    return list;
}

QStringList VideoCapture::GetSupportedFrameRateRanges(int index, QString &defaultName, QString defaultSearch)
{
    QStringList list;
    QList<QCameraFormat> cameraFormats = cameraList[index].videoFormats();
    for (QCameraFormat cameraFormat : cameraFormats) {
        QString frame = (QString::number(cameraFormat.minFrameRate()) + "~" + QString::number(cameraFormat.maxFrameRate()));
        list << frame;
        if (defaultName.isEmpty() && !defaultSearch.isEmpty() && frame.contains(defaultSearch)) {
            defaultName = frame;
        }
    }
    if (list.empty()) {
        list << "" << "default_10~10" << "default_15~15" << "default_25~25" << "default_30~30" << "default_50~50" << "default_60~60";
        defaultName = "default_30~30";
    }
    return list;
}

QStringList VideoCapture::GetSupportedPixelFormats(int index, QString &defaultName, QString defaultSearch)
{
    QStringList list;
    QList<QCameraFormat> cameraFormats = cameraList[index].videoFormats();
    for (QCameraFormat cameraFormat : cameraFormats) {
        QString format = metaEnum.key(cameraFormat.pixelFormat());
        list << format;
        if (defaultName.isEmpty() && !defaultSearch.isEmpty() && format.contains(defaultSearch)) {
            defaultName = format;
        }
    }
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

QVideoWidget *VideoCapture::getViewfinder()
{
    return &viewfinder;
}

QImage *VideoCapture::capture()
{
    if (mediaCaptureSession != Q_NULLPTR && imageCapture != Q_NULLPTR && camera != Q_NULLPTR) {
        QEventLoop* eventLoop = new QEventLoop();
        connect(imageCapture, &QImageCapture::imageAvailable, eventLoop, &QEventLoop::quit);
        imageCapture->capture();
        eventLoop->exec();
        disconnect(imageCapture, &QImageCapture::imageAvailable, eventLoop, &QEventLoop::quit);
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
    videoFrame = new QImage(frame.toImage());
}
