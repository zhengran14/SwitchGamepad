#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QSerialPort>

class SerialPort : public QObject
{
    Q_OBJECT
public:
//    static SerialPort* getInstance();
    explicit SerialPort(QObject *parent = nullptr);
    ~SerialPort();
    bool open(QString portName, int baudRate);
    void close();
    static QStringList getAllPortNames(QString defaultSearch, QString &defaultName);
    void sendData(QString data);

private:
    QSerialPort *serialPort = Q_NULLPTR;

signals:
    void openFailed();
};

#endif // SERIALPORT_H
