#ifndef SOCKET_H
#define SOCKET_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class Socket : public QObject
{
    Q_OBJECT
public:
    explicit Socket(QObject *parent = nullptr);

protected:
    bool isConnected = false;
};

class Server : public Socket
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);
    ~Server();
    void open(quint16 port);

private slots:
    void newConnection();

private:
    QTcpServer *tcpServer = Q_NULLPTR;
    QTcpSocket *tcpSocket = Q_NULLPTR;

signals:

};

class Client : public Socket
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr);
    ~Client();
    void connectServer(QString address, quint16 port);

private slots:
    void connected();

private:
    QTcpSocket *tcpSocket = Q_NULLPTR;

signals:

};

#endif // SOCKET_H
