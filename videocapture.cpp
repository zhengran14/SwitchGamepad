#include "videocapture.h"
#include "QtMultimedia/qmediacapturesession.h"
#include <QCameraDevice>
#include <QMediaDevices>
#include <QLayout>
#include <QEventLoop>
#include <QImageCapture>

VideoCapture::VideoCapture(QObject *parent)
    : QObject(parent)
{
    metaEnum = QMetaEnum::fromType<PixelFormat>();
//    viewfinder = new QVideoWidget();
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

void VideoCapture::open(int index, int cameraFormateIndex)
{
    close();
    if (cameraList.count() <= 0 || index < 0 || index >= cameraList.count()) {
        return;
    }
    camera = new QCamera(cameraList[index], this->parent());
    audioInput = new QAudioInput(audioInputList[0], this->parent());
    audioOutput = new QAudioOutput(QMediaDevices::defaultAudioOutput(), this->parent());
    QList<QCameraFormat> cameraFormats = cameraList[index].videoFormats();
    if (cameraFormats.length() > 0 && cameraFormateIndex >= 0 && cameraFormateIndex < cameraFormats.length()) {
        camera->setCameraFormat(cameraFormats[cameraFormateIndex]);
    }
    mediaCaptureSession = new QMediaCaptureSession(this->parent());
    mediaCaptureSession->setCamera(camera);
    mediaCaptureSession->setAudioInput(audioInput);
    mediaCaptureSession->setAudioOutput(audioOutput);
    //设置取景器
    mediaCaptureSession->setVideoOutput(viewfinder);
    //设置截图
    imageCapture = new QImageCapture(this->parent());
    mediaCaptureSession->setImageCapture(imageCapture);
    connect(imageCapture, &QImageCapture::imageAvailable, this, &VideoCapture::imageAvailable);
    connect(imageCapture,
            QOverload<int, QImageCapture::Error, const QString &>::of(&QImageCapture::errorOccurred),
            [=](int /*id*/, QImageCapture::Error /*error*/, const QString &errorString){
        qDebug() << errorString;
    });

    //开启
    camera->start();
}

void VideoCapture::close()
{
    if (camera != Q_NULLPTR) {
        camera->stop();
        camera->deleteLater();
        camera = Q_NULLPTR;
    }
    if (audioInput != Q_NULLPTR) {
        audioInput->deleteLater();
        audioInput = Q_NULLPTR;
    }
    if (audioOutput != Q_NULLPTR) {
        audioOutput->deleteLater();
        audioOutput = Q_NULLPTR;
    }
    if (imageCapture != Q_NULLPTR) {
        imageCapture->deleteLater();
        imageCapture = Q_NULLPTR;
    }
    QWidget *viewfinderParent = Q_NULLPTR;
    if (viewfinder != Q_NULLPTR) {
        viewfinderParent = (QWidget*)viewfinder->parent();
        removeViewfinder();
        viewfinder->deleteLater();
    }
    viewfinder = new QVideoWidget();
    if (viewfinderParent != Q_NULLPTR) {
        moveViewfinder(viewfinderParent->layout());
    }
    if (mediaCaptureSession != Q_NULLPTR) {
        mediaCaptureSession->deleteLater();
        mediaCaptureSession = Q_NULLPTR;
    }
}

QStringList VideoCapture::refreshCamera(QString defaultSearch, QString &defaultName)
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

QStringList VideoCapture::refreshAudioInput(QString defaultSearch, QString &defaultName)
{
    defaultName = "";
    audioInputList.clear();
    audioInputList = QMediaDevices::audioInputs();
    QStringList audioInputDes;
    for (const QAudioDevice &audioInfo : audioInputList) {
        if (defaultName.isEmpty() && audioInfo.description().contains(defaultSearch)) {
            defaultName = audioInfo.description();
        }
        audioInputDes << audioInfo.description();
    }
    return audioInputDes;
}

QStringList VideoCapture::GetCameraFormats(int index, int &defaultIndex, const QStringList &defaultSearch)
{
    QStringList list;
    QList<QCameraFormat> cameraFormats = cameraList[index].videoFormats();
    for (int i = 0; i < cameraFormats.length(); i++) {
        QCameraFormat cameraFormat = cameraFormats[i];
        QString res = (QString::number(cameraFormat.resolution().width()) + "x" + QString::number(cameraFormat.resolution().height()));
        QString frame = (QString::number(cameraFormat.minFrameRate()) + "~" + QString::number(cameraFormat.maxFrameRate()));
        QString format = metaEnum.key(cameraFormat.pixelFormat());
        QString str = "resolution: " + res + ", frame: " + frame + ", pixel format: " + format;
        list << str;
        if (defaultIndex < 0 && defaultSearch.length() > 0) {
            bool allFind = true;
            for (const QString &_defaultSearch : defaultSearch) {
                if (!str.contains(_defaultSearch)) {
                    allFind = false;
                    break;
                }
            }
            if (allFind) {
                defaultIndex = i;
            }
        }
    }
    return list;
}

QStringList VideoCapture::GetAudioInputFormats(int index, int &defaultIndex, const QStringList &defaultSearch)
{
}

void VideoCapture::moveViewfinder(QLayout *layout)
{
    removeViewfinder();
    if (viewfinder != Q_NULLPTR) {
        layout->addWidget(viewfinder);
    }
}

void VideoCapture::removeViewfinder()
{
    if (viewfinder != Q_NULLPTR) {
        QWidget *parent = (QWidget*)viewfinder->parent();
        if (parent != Q_NULLPTR) {
            parent->layout()->removeWidget(viewfinder);
        }
    }
}

const QVideoWidget *VideoCapture::getViewfinder()
{
    return viewfinder;
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

void VideoCapture::imageAvailable(int /*id*/, const QVideoFrame &frame)
{
//    frame.image().save("123.jpg");
    if (videoFrame != Q_NULLPTR) {
        delete videoFrame;
        videoFrame = Q_NULLPTR;
    }
    videoFrame = new QImage(frame.toImage());
}
