#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QtAV>

class Player : public QObject
{
    Q_OBJECT
public:
    explicit Player(QObject *parent = nullptr);
    ~Player();
    void play(QString path);
    void stop();

private:
    QtAV::VideoOutput *videoOutput = Q_NULLPTR;
    QtAV::AVPlayer *avPlayer = Q_NULLPTR;

signals:

};

#endif // PLAYER_H
