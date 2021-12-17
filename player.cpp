#include "player.h"
#include <QWidget>

Player::Player(QObject *parent)
    : QObject(parent)
{
}

Player::~Player()
{
    stop();
}

void Player::play(QString path, QLayout *layout)
{
    if (videoOutput != Q_NULLPTR || avPlayer != Q_NULLPTR) {
        stop();
    }
    this->playPath = path;
    videoOutput = new QtAV::VideoOutput(this);
    avPlayer = new QtAV::AVPlayer(this);
    avPlayer->setRenderer(videoOutput);
    layout->addWidget(videoOutput->widget());
    avPlayer->play(path);
}

void Player::stop()
{
    if (avPlayer != Q_NULLPTR) {
        avPlayer->stop();
        avPlayer->deleteLater();
        avPlayer = Q_NULLPTR;
    }
    if (videoOutput != Q_NULLPTR) {
        removeVideoOutput();
        videoOutput->deleteLater();
        videoOutput = Q_NULLPTR;
    }
}

void Player::pause(bool pause)
{
    if (avPlayer != Q_NULLPTR) {
        if (pause) {
            avPlayer->pause();
        }
        else {
            avPlayer->play();
        }
    }
}

//void Player::moveVideoOutput(QLayout *layout)
//{
//    if (videoOutput != Q_NULLPTR) {
//        stop();
//        play(this->playPath);
//        layout->addWidget(videoOutput->widget());
//    }
//}

void Player::removeVideoOutput()
{
    if (videoOutput != Q_NULLPTR) {
        QWidget *parent = (QWidget*)videoOutput->widget()->parent();
        if (parent != Q_NULLPTR) {
            parent->layout()->removeWidget(videoOutput->widget());
        }
    }
}

//QtAV::VideoOutput *Player::getVideoOutput()
//{
//    return &videoOutput;
//}
