#include "socket.h"

Socket::Socket(QObject *parent) : QObject(parent)
{
    //    QTcpServer* mp_TCPServer ;
    //    mp_TCPServer = new QTcpServer();
    //    mp_TCPServer->listen(QHostAddress::Any, 7000);
    //    connect(mp_TCPServer, &QTcpServer::newConnection, this, [this, mp_TCPServer]() {
    //        QTcpSocket *mp_TCPSocket = mp_TCPServer->nextPendingConnection();
    //        connect(mp_TCPSocket, &QTcpSocket::readyRead, this, [this, mp_TCPSocket]() {
    //            qDebug() << mp_TCPSocket->readAll();
    //        });
        //    });


    //    QTcpSocket* mp_clientSocket;
    //            mp_clientSocket = new QTcpSocket();
    //            mp_clientSocket->connectToHost("127.0.0.1", 7000);
    //            connect(mp_clientSocket, &QTcpSocket::connected, this, [this, mp_clientSocket]() {
    //                mp_clientSocket->write("123123123");
        //            });
}

Server::Server(QObject *parent) : Socket(parent)
{
    this->isConnected = false;
    tcpServer = new QTcpServer(this);
    tcpServer->setMaxPendingConnections(1);
    connect(tcpServer, &QTcpServer::newConnection, this, &Server::newConnection);
}

Server::~Server()
{
    if (tcpSocket != Q_NULLPTR) {
        tcpSocket->close();
        tcpSocket->deleteLater();
        tcpSocket = Q_NULLPTR;
    }
    if (tcpServer != Q_NULLPTR) {
        tcpServer->close();
        tcpServer->deleteLater();
        tcpServer = Q_NULLPTR;
    }
}

void Server::open(quint16 port)
{
    if (this->isConnected) {
        return;
    }
    tcpServer->listen(QHostAddress::Any, port);
}

void Server::newConnection()
{
    if (this->isConnected) {
        return;
    }
    if (tcpSocket != Q_NULLPTR) {
        return;
    }
    tcpSocket = tcpServer->nextPendingConnection();
    tcpServer->pauseAccepting();
    if (tcpSocket->state() == QTcpSocket::ConnectedState) {
        this->isConnected = true;
    }
}

Client::Client(QObject *parent) : Socket(parent)
{
    this->isConnected = false;
    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, &QTcpSocket::connected, this, &Client::connected);
}

Client::~Client()
{
    if (tcpSocket != Q_NULLPTR) {
        tcpSocket->close();
        tcpSocket->deleteLater();
        tcpSocket = Q_NULLPTR;
    }
}

void Client::connectServer(QString address, quint16 port)
{
    if (this->isConnected) {
        return;
    }
    tcpSocket->connectToHost(address, port);
}

void Client::connected()
{
    if (this->isConnected) {
        return;
    }
    if (tcpSocket->state() == QTcpSocket::ConnectedState) {
        this->isConnected = true;
    }
}
