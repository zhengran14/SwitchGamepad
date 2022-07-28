#ifndef VIDEOCAPTURE_H
#define VIDEOCAPTURE_H

#include <QObject>
#include <QCamera>
#include <QVideoWidget>
#include <QImageCapture>
#include <QAudioDevice>
#include <QAudioInput>
#include <QAudioOutput>

class VideoCapture : public QObject
{
    Q_OBJECT
public:
    enum PixelFormat
    {
        Format_Invalid,
        Format_ARGB8888,
        Format_ARGB8888_Premultiplied,
        Format_XRGB8888,
        Format_BGRA8888,
        Format_BGRA8888_Premultiplied,
        Format_BGRX8888,
        Format_ABGR8888,
        Format_XBGR8888,
        Format_RGBA8888,
        Format_RGBX8888,

        Format_AYUV,
        Format_AYUV_Premultiplied,
        Format_YUV420P,
        Format_YUV422P,
        Format_YV12,
        Format_UYVY,
        Format_YUYV,
        Format_NV12,
        Format_NV21,
        Format_IMC1,
        Format_IMC2,
        Format_IMC3,
        Format_IMC4,
        Format_Y8,
        Format_Y16,

        Format_P010,
        Format_P016,

        Format_SamplerExternalOES,
        Format_Jpeg,
        Format_SamplerRect,
    };
    Q_ENUM(PixelFormat)
    explicit VideoCapture(QObject *parent);
    ~VideoCapture();
//    void init(QLayout *layout);
    void open(int index, int cameraFormateIndex);
    void close();
    QStringList refreshCamera(QString defaultSearch, QString &defaultName);
    QStringList refreshAudioInput(QString defaultSearch, QString &defaultName);
    QStringList GetCameraFormats(int index, int &defaultIndex, const QStringList &defaultSearch);
    QStringList GetAudioInputFormats(int index, int &defaultIndex, const QStringList &defaultSearch);
    void moveViewfinder(QLayout *layout);
    void removeViewfinder();
    const QVideoWidget* getViewfinder();
    QImage *capture();

private slots:
    void imageAvailable(int id, const QVideoFrame &frame);

private:
    QCamera *camera = Q_NULLPTR;
    QAudioInput *audioInput = Q_NULLPTR;
    QAudioOutput *audioOutput = Q_NULLPTR;
    QMediaCaptureSession *mediaCaptureSession = Q_NULLPTR;
    QImageCapture *imageCapture = Q_NULLPTR;
    QVideoWidget *viewfinder = Q_NULLPTR;
    QList<QCameraDevice> cameraList;
    QList<QAudioDevice> audioInputList;
    QMetaEnum metaEnum;
    QImage *videoFrame = Q_NULLPTR;

signals:

};

#endif // VIDEOCAPTURE_H
