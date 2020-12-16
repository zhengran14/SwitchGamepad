#ifndef SOCKET_H
#define SOCKET_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = nullptr);

protected:
    void incomingConnection(qintptr socketDescriptor);
};

class Socket : public QObject
{
    Q_OBJECT
public:
    explicit Socket(QObject *parent = nullptr);
    ~Socket();
    void wirte(QString str);
    QString reed();

protected:
    bool isConnected = false;
    QTcpSocket *tcpSocket = Q_NULLPTR;
};

class Server : public Socket
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);
    ~Server();
    bool open(quint16 port);
    void close();

private slots:
    void newConnection();
    void clientDisconnect();

private:
    QTcpServer *tcpServer = Q_NULLPTR;
    QTcpSocket *tcpSocket = Q_NULLPTR;

signals:
    void clientNewConnectiton(QString str);
    void clientConnectionClosed(QString str);

};

class Client : public Socket
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr);
    ~Client();
    void connectServer(QString address, quint16 port);
    void disConnectServer();

private slots:
    void connected();
    void disconnected();
    void errorOccurred(QAbstractSocket::SocketError socketError);

private:

signals:
    void connectSuccess(QString str);
    void closeConnect(QString str);
    void connectError(QString str);

};

#endif // SOCKET_H
