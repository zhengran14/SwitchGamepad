#include "socket.h"

TcpServer::TcpServer(QObject *parent) : QTcpServer(parent)
{
}

void TcpServer::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << "123";
    QTcpServer::incomingConnection(socketDescriptor);
}

Socket::Socket(QObject *parent) : QObject(parent)
{
}

Socket::~Socket()
{
}

void Socket::wirte(QString str)
{
    if (!this->isConnected) {
        return;
    }
    tcpSocket->write(str.toUtf8());
}

QString Socket::reed()
{
    if (!this->isConnected) {
        return "";
    }
    return QString::fromUtf8(tcpSocket->readAll());
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

bool Server::open(quint16 port)
{
    if (tcpServer->isListening()) {
        return false;
    }
    return tcpServer->listen(QHostAddress::Any, port);
}

void Server::close()
{
    if (tcpSocket != Q_NULLPTR) {
        tcpSocket->close();
        tcpSocket->deleteLater();
        tcpSocket = Q_NULLPTR;
    }
    tcpServer->close();
    this->isConnected = false;
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
    connect(tcpSocket, &QTcpSocket::disconnected, this, &Server::clientDisconnect);
    tcpServer->pauseAccepting();
    if (tcpSocket->state() == QTcpSocket::ConnectedState) {
        this->isConnected = true;
        emit clientNewConnectiton("");
    }
}

void Server::clientDisconnect()
{
    if (tcpSocket != Q_NULLPTR) {
        tcpSocket->close();
        tcpSocket->deleteLater();
        tcpSocket = Q_NULLPTR;
    }
    tcpServer->resumeAccepting();
    this->isConnected = false;
    emit clientConnectionClosed("");
}

Client::Client(QObject *parent) : Socket(parent)
{
    this->isConnected = false;
    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, &QTcpSocket::connected, this, &Client::connected);
    connect(tcpSocket, &QTcpSocket::disconnected, this, &Client::disconnected);
    connect(tcpSocket, &QTcpSocket::errorOccurred, this, &Client::errorOccurred);
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

void Client::disConnectServer()
{
    tcpSocket->close();
    this->isConnected = false;
}

void Client::connected()
{
    if (this->isConnected) {
        return;
    }
    if (tcpSocket->state() == QTcpSocket::ConnectedState) {
        this->isConnected = true;
        emit connectSuccess("");
    }
}

void Client::disconnected()
{
    emit closeConnect("");
}

void Client::errorOccurred(QAbstractSocket::SocketError socketError)
{
    this->disConnectServer();
    this->isConnected = false;
    emit connectError("");
}
