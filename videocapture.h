#ifndef VIDEOCAPTURE_H
#define VIDEOCAPTURE_H

#include <QObject>
#include <QCamera>
#include <QVideoWidget>
#include <QImageCapture>

class VideoCapture : public QObject
{
    Q_OBJECT
public:
    enum PixelFormat
    {
        Format_Invalid = 0,
        Format_ARGB8888 = 1,
        Format_ARGB8888_Premultiplied = 2,
        Format_XRGB8888 = 3,
        Format_BGRA8888 = 4,
        Format_BGRA8888_Premultiplied = 5,
        Format_ABGR8888 = 7,
        Format_XBGR8888 = 8,
        Format_RGBA8888 = 9,
        Format_BGRX8888 = 6,
        Format_RGBX8888 = 10,
        Format_AYUV = 11,
        Format_AYUV_Premultiplied = 12,
        Format_YUV420P = 13,
        Format_YUV422P = 14,
        Format_YV12 = 15,
        Format_UYVY = 16,
        Format_YUYV = 17,
        Format_NV12 = 18,
        Format_NV21 = 19,
        Format_IMC1 = 20,
        Format_IMC2 = 21,
        Format_IMC3 = 22,
        Format_IMC4 = 23,
        Format_P010 = 26,
        Format_P016 = 27,
        Format_Y8 = 24,
        Format_Y16 = 25,
        Format_Jpeg = 29,
        Format_SamplerExternalOES = 28,
        Format_SamplerRect = 30,
        Format_YUV420P10 = 31,

#ifndef Q_QDOC
        NPixelFormats,
#endif
        Format_User = 1000
    };
    Q_ENUM(PixelFormat)
    explicit VideoCapture(QObject *parent);
    ~VideoCapture();
//    void init(QLayout *layout);
    void open(int index, int cameraFormateIndex);
    void close();
    QStringList refresh(QString defaultSearch, QString &defaultName);
    QStringList GetCameraFormats(int index, int &defaultIndex, const QStringList &defaultSearch);
    void moveViewfinder(QLayout *layout);
    void removeViewfinder();
    const QVideoWidget* getViewfinder();
    QImage *capture();

private slots:
    void imageAvailable(int id, const QVideoFrame &frame);

private:
    QCamera *camera = Q_NULLPTR;
    QMediaCaptureSession *mediaCaptureSession = Q_NULLPTR;
    QImageCapture *imageCapture = Q_NULLPTR;
    QVideoWidget *viewfinder = Q_NULLPTR;
    QList<QCameraDevice> cameraList;
    QMetaEnum metaEnum;
    QImage *videoFrame = Q_NULLPTR;

signals:

};

#endif // VIDEOCAPTURE_H
