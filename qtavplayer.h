#ifndef QTAVPLAYER_H
#define QTAVPLAYER_H

#include <QObject>
#ifdef USE_QTAV
#include <QtAV>
#endif
#include <QLayout>

class Player : public QObject
{
    Q_OBJECT
public:
    explicit Player(QObject *parent = nullptr);
    ~Player();
    void play(QString path, QLayout *layout);
    void stop();
    void pause(bool pause);
//    void moveVideoOutput(QLayout *layout);
//    QtAV::VideoOutput *getVideoOutput();

private:
#ifdef USE_QTAV
    QtAV::VideoOutput *videoOutput = Q_NULLPTR;
    QtAV::AVPlayer *avPlayer = Q_NULLPTR;
#endif
    void removeVideoOutput();
    QString playPath;

signals:

};

#endif // QTAVPLAYER_H
