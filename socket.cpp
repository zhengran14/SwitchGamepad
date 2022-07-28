#include "socket.h"
#include <QJsonObject>
#include <QJsonDocument>

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

//void Socket::wirte(QString str, bool isCompress)
//{
//    if (!this->isConnected || tcpSocket == Q_NULLPTR) {
//        return;
//    }
//    QByteArray buffer = str.toUtf8();
//    if (isCompress) {
//        buffer = qCompress(buffer,-1);
//    }
//    tcpSocket->write(buffer);
//}

void Socket::write(QJsonObject json, QString message, Utils::Operation operation, bool isCompress)
{
    if (!this->isConnected || tcpSocket == Q_NULLPTR) {
        return;
    }
    QJsonObject resultJson;
    resultJson.insert("message", message);
    resultJson.insert("status", true);
    resultJson.insert("operation", operation);
    resultJson.insert("data", json);
    QByteArray buffer = QString(QJsonDocument(resultJson).toJson(QJsonDocument::Compact)).toUtf8();
    if (isCompress) {
        buffer = qCompress(buffer,-1);
    }
    tcpSocket->write(buffer);
}

//QString Socket::read(bool isCompress)
//{
//    if (!this->isConnected || tcpSocket == Q_NULLPTR) {
//        return "";
//    }
//    QByteArray buffer = tcpSocket->readAll();
//    if (isCompress) {
//        buffer = qUncompress(buffer);
//    }
//    return QString::fromUtf8(buffer);
//}

bool Socket::read(QJsonObject &json, QString &message, Utils::Operation &operation, bool isCompress)
{
    if (!this->isConnected || tcpSocket == Q_NULLPTR) {
        json = QJsonObject();
        message = "";
        operation = Utils::UnknownOperation;
        return false;
    }
    QByteArray buffer = tcpSocket->readAll();
    if (isCompress) {
        QByteArray _buffer = qUncompress(buffer);
        if (!_buffer.isEmpty()) {
            buffer = _buffer;
        }
    }
    QJsonParseError resultJsonError;
    QJsonDocument resultJsonDocument = QJsonDocument::fromJson(buffer, &resultJsonError);
    if (resultJsonError.error == QJsonParseError::NoError) {
        if (resultJsonDocument.isObject()) {
            QJsonObject resultJson = resultJsonDocument.object();
            bool result = resultJson.contains("status") ? resultJson["status"].toBool(false) : false;
            QString _message = resultJson.contains("message") ? resultJson["message"].toString("") : "";
            QJsonObject _json = resultJson.contains("data") ? resultJson["data"].toObject() : QJsonObject();
            Utils::Operation _operation = resultJson.contains("operation") ? (Utils::Operation)(resultJson["operation"].toInt(Utils::UnknownOperation)) : Utils::UnknownOperation;
            if (result) {
                message = _message;
                json = _json;
                operation = _operation;
                return result;
            }
        }
    }
    json = QJsonObject();
    message = "";
    operation = Utils::UnknownOperation;
    return false;
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
        disconnect(tcpSocket, &QTcpSocket::disconnected, this, &Server::clientDisconnect);
        disconnect(tcpSocket, &QTcpSocket::readyRead, this, &Server::clientReadyRead);
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
    connect(tcpSocket, &QTcpSocket::readyRead, this, &Server::clientReadyRead);
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

void Server::clientReadyRead()
{
    emit receiveData();
}

Client::Client(QObject *parent) : Socket(parent)
{
    this->isConnected = false;
    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, &QTcpSocket::connected, this, &Client::connected);
    connect(tcpSocket, &QTcpSocket::disconnected, this, &Client::disconnected);
    connect(tcpSocket, &QTcpSocket::errorOccurred, this, &Client::errorOccurred);
    connect(tcpSocket, &QTcpSocket::readyRead, this, &Client::clientReadyRead);
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

void Client::errorOccurred(QAbstractSocket::SocketError /*socketError*/)
{
    this->disConnectServer();
    this->isConnected = false;
    emit connectError("");
}

void Client::clientReadyRead()
{
    emit receiveData();
}
