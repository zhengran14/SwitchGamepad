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
    explicit VideoCapture(QObject *parent);
    ~VideoCapture();
    void init(QLayout *layout);
    void open(int index);
    void close();
    QStringList refresh(QString defaultSearch, QString &defaultName);

private:
    QCamera *camera = Q_NULLPTR;
    QCameraViewfinder *viewfinder = Q_NULLPTR;
    QCameraImageCapture *imageCapture = Q_NULLPTR;
    QList<QCameraInfo> cameraList;

signals:

};

#endif // VIDEOCAPTURE_H
