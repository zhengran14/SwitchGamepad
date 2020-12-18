#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QtAV>
#include <QLayout>

class Player : public QObject
{
    Q_OBJECT
public:
    explicit Player(QObject *parent = nullptr);
    ~Player();
    void play(QString path, QLayout *layout);
    void stop();
//    void moveVideoOutput(QLayout *layout);
//    QtAV::VideoOutput *getVideoOutput();

private:
    QtAV::VideoOutput *videoOutput = Q_NULLPTR;
    QtAV::AVPlayer *avPlayer = Q_NULLPTR;
    void removeVideoOutput();
    QString playPath;

signals:

};

#endif // PLAYER_H
