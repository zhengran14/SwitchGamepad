#ifndef VIDEOCAPTURE_H
#define VIDEOCAPTURE_H

#include <QObject>
#include <QCamera>
#include <QCameraViewfinder>
#include <QCameraImageCapture>

class VideoCapture : public QObject
{
    Q_OBJECT
public:
    enum PixelFormat
    {
        Format_Invalid,
        Format_ARGB32,
        Format_ARGB32_Premultiplied,
        Format_RGB32,
        Format_RGB24,
        Format_RGB565,
        Format_RGB555,
        Format_ARGB8565_Premultiplied,
        Format_BGRA32,
        Format_BGRA32_Premultiplied,
        Format_BGR32,
        Format_BGR24,
        Format_BGR565,
        Format_BGR555,
        Format_BGRA5658_Premultiplied,

        Format_AYUV444,
        Format_AYUV444_Premultiplied,
        Format_YUV444,
        Format_YUV420P,
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

        Format_Jpeg,

        Format_CameraRaw,
        Format_AdobeDng,
        Format_ABGR32, // ### Qt 6: reorder
        Format_YUV422P,

#ifndef Q_QDOC
        NPixelFormats,
#endif
        Format_User = 1000
    };
    Q_ENUM(PixelFormat)
    explicit VideoCapture(QObject *parent);
    ~VideoCapture();
//    void init(QLayout *layout);
    void open(int index, QString resolution, QString frameRateRange, QString pixelFormat);
    void close();
    QStringList refresh(QString defaultSearch, QString &defaultName);
    QStringList GetSupportedResolutions(int index);
    QStringList GetSupportedFrameRateRanges(int index);
    QStringList GetSupportedPixelFormats(int index);
    void moveViewfinder(QLayout *layout);
    QCameraViewfinder* getViewfinder();

private:
    QCamera *camera = Q_NULLPTR;
    QCameraViewfinder *viewfinder = Q_NULLPTR;
    QCameraImageCapture *imageCapture = Q_NULLPTR;
    QList<QCameraInfo> cameraList;
    QMetaEnum metaEnum;

signals:

};

#endif // VIDEOCAPTURE_H
