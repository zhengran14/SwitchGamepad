#include "serialport.h"
#include <QSerialPortInfo>

//Q_GLOBAL_STATIC(SerialPort, p_serialPort)

//SerialPort *SerialPort::getInstance()
//{
//    return p_serialPort;
//}

SerialPort::SerialPort(QObject *parent) : QObject(parent)
{

}

SerialPort::~SerialPort()
{
    close();
}

bool SerialPort::open(QString portName, int baudRate)
{
    close();
    serialPort = new QSerialPort();
    serialPort->setPortName(portName);
    if(!serialPort->open(QIODevice::ReadWrite)) {
        emit openFailed();
        close();
        return false;
    }
    serialPort->setBaudRate(baudRate, QSerialPort::AllDirections); //设置波特率和读写方向
    serialPort->setDataBits(QSerialPort::Data8); //数据位为8位
    serialPort->setFlowControl(QSerialPort::NoFlowControl); //无流控制
    serialPort->setParity(QSerialPort::NoParity); //无校验位
    serialPort->setStopBits(QSerialPort::OneStop); //一位停止位
    return true;
}

void SerialPort::close()
{
    if(serialPort != Q_NULLPTR) {
        if (serialPort->isOpen()) {
            serialPort->clear();
            serialPort->close();
        }
        serialPort->deleteLater();
        serialPort = Q_NULLPTR;
    }
}

QStringList SerialPort::getAllPortNames(QString defaultSearch, QString &defaultName)
{
    defaultName = "";
    QStringList portNames;
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        portNames << info.portName();
        if (defaultName.isEmpty() && info.portName().contains(defaultSearch)) {
            defaultName = info.portName();
        }
    }
    return portNames;
}

void SerialPort::sendData(QString data)
{
    if(serialPort != Q_NULLPTR && serialPort->isOpen()) {
        data += "\r\n";
        serialPort->write(data.toUtf8());
    }
}
