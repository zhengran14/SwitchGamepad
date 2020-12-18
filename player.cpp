#include "player.h"

Player::Player(QObject *parent) : QObject(parent)
{
    avPlayer = new QtAV::AVPlayer(this);
    videoOutput = new QtAV::VideoOutput(this);
    avPlayer->setRenderer(videoOutput);
}

Player::~Player()
{
    avPlayer->stop();
    avPlayer->deleteLater();
    videoOutput->deleteLater();
}

void Player::play(QString path)
{
    // rtmp://rrabbit.xyz/live/mystream
    // rtmp://58.200.131.2:1935/livetv/hunantv
    avPlayer->play(path);
}

void Player::stop()
{
    avPlayer->stop();
}
