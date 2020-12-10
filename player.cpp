#include "player.h"
#include <QtAV>
#include <QWidget>
#include <QVBoxLayout>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QPushButton>

Player::Player(QWidget *parent) : QWidget(parent)
{
    this->setFixedSize(640, 480);
    QtAV::VideoOutput *m_vo;
    QtAV::AVPlayer *m_player;
    m_player = new QtAV::AVPlayer(this);
    m_vo = new QtAV::VideoOutput(this);
    m_player->setRenderer(m_vo);
    m_player->play("rtmp://rrabbit.xyz/live/mystream");
    QVBoxLayout *vl = new QVBoxLayout();
    this->setLayout(vl);
    vl->addWidget(m_vo->widget());
    QPushButton *btn = new QPushButton(this);
    vl->addWidget(btn);
    connect(btn, &QPushButton::clicked, this, [this]() {
        QTcpSocket* mp_clientSocket;
        mp_clientSocket = new QTcpSocket();
        mp_clientSocket->connectToHost("127.0.0.1", 7000);
        connect(mp_clientSocket, &QTcpSocket::connected, this, [this, mp_clientSocket]() {
            mp_clientSocket->write("123123123");
        });
    });
    this->show();


    QTcpServer* mp_TCPServer ;
    mp_TCPServer = new QTcpServer();
    mp_TCPServer->listen(QHostAddress::Any, 7000);
    connect(mp_TCPServer, &QTcpServer::newConnection, this, [this, mp_TCPServer]() {
        QTcpSocket *mp_TCPSocket = mp_TCPServer->nextPendingConnection();
        connect(mp_TCPSocket, &QTcpSocket::readyRead, this, [this, mp_TCPSocket]() {
            qDebug() << mp_TCPSocket->readAll();
        });
    });
}
