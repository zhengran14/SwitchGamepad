#include "scriptengineevaluation.h"
#include <QThread>
#include <QTime>
#include <QEventLoop>
#include <QDebug>
#include <QImage>
#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"
#include <utils.h>
#include <QTcpSocket>

ScriptEngineEvaluation::ScriptEngineEvaluation(QObject *parent) : QObject(parent)
{
    scriptEngine.globalObject().setProperty("gp", scriptEngine.newQObject(this, QScriptEngine::QtOwnership));
//    engine.evaluate("g_sp.open('asd', 123);");
//    QScriptValue sriptValue = scriptEngine.newFunction(sleep);
//    scriptEngine.globalObject().setProperty("myAdd", sriptValue);
}

ScriptEngineEvaluation::~ScriptEngineEvaluation()
{
    scriptEngine.abortEvaluation();
    scriptEngine.deleteLater();
}

void ScriptEngineEvaluation::evaluate(QString script)
{
    stop();
    needStop = false;
    scriptEngine.abortEvaluation();
    scriptEngine.clearExceptions();
    QScriptValue result = scriptEngine.evaluate(script);
    if (scriptEngine.hasUncaughtException()) {
        int line = scriptEngine.uncaughtExceptionLineNumber();
        emit hasException(tr("Uncaught exception at line ") + QString::number(line) + tr(": ") + result.toString());
    }
    emit finished();
}

void ScriptEngineEvaluation::stop()
{
    needStop = true;
}

void ScriptEngineEvaluation::cameraCaptured(QImage *videoFrame)
{
    this->videoFrame = videoFrame;
    emit hasCaptureCamera();
}

void ScriptEngineEvaluation::sleep(float sec)
{
    if (needStop) {
        scriptEngine.abortEvaluation();
        return;
    }
//    QThread::msleep((int)(time * 1000));

    for (int i = 1; i <= sec * 10; i++) {
        if (needStop) {
            scriptEngine.abortEvaluation();
            return;
        }
        QThread::msleep((int)(100));
    }

//    QTime timer = QTime::currentTime().addMSecs((int)(sec * 1000));
//    while (QTime::currentTime() < timer) {
//        if (needStop) {
//            scriptEngine.abortEvaluation();
//            return;
//        }
//        QThread::msleep((int)(100));
//    }
}

void ScriptEngineEvaluation::pressButton(QString string, float sec)
{
    if (needStop) {
        scriptEngine.abortEvaluation();
        return;
    }
    emit sendData(string);
    sleep(sec);
    if (needStop) {
        scriptEngine.abortEvaluation();
        return;
    }
    emit sendData("RELEASE");
}

bool ScriptEngineEvaluation::messageBox(QString title, QString content)
{
    QEventLoop* eventLoop = new QEventLoop();
    connect(this, &ScriptEngineEvaluation::messageBoxReturned, eventLoop, &QEventLoop::quit);
    emit messageBoxShow(title, content);
    eventLoop->exec();
    eventLoop->deleteLater();
    return this->messageBoxResult;
}

bool ScriptEngineEvaluation::judgeShinePokemon()
{
    bool result = false;
    QEventLoop* eventLoop = new QEventLoop();
    connect(this, &ScriptEngineEvaluation::hasCaptureCamera, eventLoop, &QEventLoop::quit);
    emit needCaptureCamera();
    eventLoop->exec();
    eventLoop->deleteLater();
//    qDebug() << (videoFrame == Q_NULLPTR ? QSize() : videoFrame->size());
//    videoFrame->save("123.jpg");
    if (videoFrame != Q_NULLPTR) {
        QImage img(":/res/shine_template.jpg");
        cv::Mat captureFrame = Utils::QImage2cvMat(*videoFrame);
        cv::Mat captureFrame2;
        cv::cvtColor(captureFrame, captureFrame2, cv::COLOR_BGR2RGB);
        cv::Mat shineTemplate = Utils::QImage2cvMat(img);
        cv::Mat shineTemplate2;
        cv::cvtColor(shineTemplate, shineTemplate2, cv::COLOR_BGR2RGB);
        cv::Mat dstImg;
        dstImg.create(captureFrame2.dims, captureFrame2.size, captureFrame2.type());
        cv::matchTemplate(captureFrame2, shineTemplate2, dstImg, 0);
        cv::Point minPoint;
        cv::Point maxPoint;
        double *minVal = 0;
        double *maxVal = 0;
        cv::minMaxLoc(dstImg, minVal, maxVal, &minPoint,&maxPoint);
        maxPoint = cv::Point(minPoint.x + shineTemplate2.cols, minPoint.y + shineTemplate2.rows);
        // 110 590 210 635
//        qDebug() << minPoint.x << minPoint.y << maxPoint.x << maxPoint.y;
        if (minPoint.x != 110 || minPoint.y != 590) {
            result = true;
        }
        dstImg.release();
        captureFrame.release();
        captureFrame2.release();
        shineTemplate.release();
        shineTemplate2.release();
    }
    return result;
}

bool ScriptEngineEvaluation::judgeShinePokemon2()
{
    bool result = false;
    QEventLoop* eventLoop = new QEventLoop();
    connect(this, &ScriptEngineEvaluation::hasCaptureCamera, eventLoop, &QEventLoop::quit);
    emit needCaptureCamera();
    eventLoop->exec();
    eventLoop->deleteLater();
//    qDebug() << (videoFrame == Q_NULLPTR ? QSize() : videoFrame->size());
//    videoFrame->save("123.jpg");
    if (videoFrame != Q_NULLPTR) {
        QImage img(":/res/shine_template2.jpg");
        cv::Mat captureFrame = Utils::QImage2cvMat(*videoFrame);
        cv::Mat captureFrame2;
        cv::cvtColor(captureFrame, captureFrame2, cv::COLOR_BGR2RGB);
        cv::Mat shineTemplate = Utils::QImage2cvMat(img);
        cv::Mat shineTemplate2;
        cv::cvtColor(shineTemplate, shineTemplate2, cv::COLOR_BGR2RGB);
        cv::Mat dstImg;
        dstImg.create(captureFrame2.dims, captureFrame2.size, captureFrame2.type());
        cv::matchTemplate(captureFrame2, shineTemplate2, dstImg, 0);
        cv::Point minPoint;
        cv::Point maxPoint;
        double *minVal = 0;
        double *maxVal = 0;
        cv::minMaxLoc(dstImg, minVal, maxVal, &minPoint,&maxPoint);
        maxPoint = cv::Point(minPoint.x + shineTemplate2.cols, minPoint.y + shineTemplate2.rows);
        // 110 590 210 635
//        qDebug() << minPoint.x << minPoint.y << maxPoint.x << maxPoint.y;
        if (minPoint.x != 110 || minPoint.y != 590) {
            result = true;
        }
        dstImg.release();
        captureFrame.release();
        captureFrame2.release();
        shineTemplate.release();
        shineTemplate2.release();
    }
    return result;
}

bool ScriptEngineEvaluation::judgeShinePokemonCustome(QString path)
{
    bool result = false;
    QEventLoop* eventLoop = new QEventLoop();
    connect(this, &ScriptEngineEvaluation::hasCaptureCamera, eventLoop, &QEventLoop::quit);
    emit needCaptureCamera();
    eventLoop->exec();
    eventLoop->deleteLater();
//    qDebug() << (videoFrame == Q_NULLPTR ? QSize() : videoFrame->size());
//    videoFrame->save("123.jpg");
    if (videoFrame != Q_NULLPTR) {
        QImage img(path);
        cv::Mat captureFrame = Utils::QImage2cvMat(*videoFrame);
        cv::Mat captureFrame2;
        cv::cvtColor(captureFrame, captureFrame2, cv::COLOR_BGR2RGB);
        cv::Mat shineTemplate = Utils::QImage2cvMat(img);
        cv::Mat shineTemplate2;
        cv::cvtColor(shineTemplate, shineTemplate2, cv::COLOR_BGR2RGB);
        cv::Mat dstImg;
        dstImg.create(captureFrame2.dims, captureFrame2.size, captureFrame2.type());
        cv::matchTemplate(captureFrame2, shineTemplate2, dstImg, 0);
        cv::Point minPoint;
        cv::Point maxPoint;
        double *minVal = 0;
        double *maxVal = 0;
        cv::minMaxLoc(dstImg, minVal, maxVal, &minPoint,&maxPoint);
        maxPoint = cv::Point(minPoint.x + shineTemplate2.cols, minPoint.y + shineTemplate2.rows);
        // 110 590 210 635
//        qDebug() << minPoint.x << minPoint.y << maxPoint.x << maxPoint.y;
        if (minPoint.x != 110 || minPoint.y != 590) {
            result = true;
        }
        dstImg.release();
        captureFrame.release();
        captureFrame2.release();
        shineTemplate.release();
        shineTemplate2.release();
    }
    return result;
}

void ScriptEngineEvaluation::capture(QString path)
{
    bool result = false;
    QEventLoop* eventLoop = new QEventLoop();
    connect(this, &ScriptEngineEvaluation::hasCaptureCamera, eventLoop, &QEventLoop::quit);
    emit needCaptureCamera();
    eventLoop->exec();
    eventLoop->deleteLater();
//    qDebug() << (videoFrame == Q_NULLPTR ? QSize() : videoFrame->size());
//    videoFrame->save("123.jpg");
    if (videoFrame != Q_NULLPTR) {
        videoFrame->save(path);
    }
    //    return result;
}

void ScriptEngineEvaluation::captureCut(QString path)
{
    bool result = false;
    QEventLoop* eventLoop = new QEventLoop();
    connect(this, &ScriptEngineEvaluation::hasCaptureCamera, eventLoop, &QEventLoop::quit);
    emit needCaptureCamera();
    eventLoop->exec();
    eventLoop->deleteLater();
//    qDebug() << (videoFrame == Q_NULLPTR ? QSize() : videoFrame->size());
//    videoFrame->save("123.jpg");
    if (videoFrame != Q_NULLPTR) {
        QImage newCut = videoFrame->copy(110, 590, 100, 45);
        newCut.save(path);
    }
    //    return result;
}

QString ScriptEngineEvaluation::judgeShinePokemonTest(QString path)
{
    QString result = "";
    QEventLoop* eventLoop = new QEventLoop();
    connect(this, &ScriptEngineEvaluation::hasCaptureCamera, eventLoop, &QEventLoop::quit);
    emit needCaptureCamera();
    eventLoop->exec();
    eventLoop->deleteLater();
//    qDebug() << (videoFrame == Q_NULLPTR ? QSize() : videoFrame->size());
//    videoFrame->save("123.jpg");
    if (videoFrame != Q_NULLPTR) {
        QImage img(path);
        cv::Mat captureFrame = Utils::QImage2cvMat(*videoFrame);
        cv::Mat captureFrame2;
        cv::cvtColor(captureFrame, captureFrame2, cv::COLOR_BGR2RGB);
        cv::Mat shineTemplate = Utils::QImage2cvMat(img);
        cv::Mat shineTemplate2;
        cv::cvtColor(shineTemplate, shineTemplate2, cv::COLOR_BGR2RGB);
        cv::Mat dstImg;
        dstImg.create(captureFrame2.dims, captureFrame2.size, captureFrame2.type());
        cv::matchTemplate(captureFrame2, shineTemplate2, dstImg, 0);
        cv::Point minPoint;
        cv::Point maxPoint;
        double *minVal = 0;
        double *maxVal = 0;
        cv::minMaxLoc(dstImg, minVal, maxVal, &minPoint,&maxPoint);
        maxPoint = cv::Point(minPoint.x + shineTemplate2.cols, minPoint.y + shineTemplate2.rows);
        // 110 590 210 635
//        qDebug() << minPoint.x << minPoint.y << maxPoint.x << maxPoint.y;
        result = QString("%1, %2").arg(minPoint.x).arg(minPoint.y);
        dstImg.release();
        captureFrame.release();
        captureFrame2.release();
        shineTemplate.release();
        shineTemplate2.release();
    }
    return result;
}

void ScriptEngineEvaluation::statusText(QString text)
{
    emit setStatusText(text);
}

void ScriptEngineEvaluation::mail(QString username, QString password, QString receiver, QString subject, QString content)
{

    QTcpSocket clientsocket;
//    QByteArray username = "@qq.com";
//    QByteArray password = "";
//    QByteArray recvaddr = "@qq.com";
    QByteArray mailfrom = "mail from:<";
    QByteArray rcptto = "rcpt to:<";
    QByteArray prefrom = "from:";
    QByteArray preto = "to:";
    QByteArray presubject ="subject:";
//    QString subject = "test from qt";                //主题
//    QString content = "test from qt";                //发送内容
    QByteArray recvdata;            //接收到的数据
    QByteArray usernametmp = username.toUtf8();
    QByteArray recvaddrtmp = receiver.toUtf8();
    clientsocket.connectToHost("smtp.qq.com", 25, QTcpSocket::ReadWrite);
    clientsocket.waitForConnected(1000);
    clientsocket.waitForReadyRead(1000);
    recvdata = clientsocket.readAll();
//    qDebug() << recvdata;
    clientsocket.write("HELO smtp.qq.com\r\n");
    clientsocket.waitForReadyRead(1000);
    recvdata = clientsocket.readAll();
//    qDebug() << recvdata;
    clientsocket.write("AUTH LOGIN\r\n");
    clientsocket.waitForReadyRead(1000);
    recvdata = clientsocket.readAll();
//    qDebug() << recvdata;
//    qDebug() << "username:" << username;
    clientsocket.write(username.toUtf8().toBase64().append("\r\n"));
    clientsocket.waitForReadyRead(1000);
    recvdata = clientsocket.readAll();
//    qDebug() << recvdata;
//    qDebug() << "password:" << password;
    clientsocket.write(password.toUtf8().toBase64().append("\r\n"));
    clientsocket.waitForReadyRead(1000);
    recvdata = clientsocket.readAll();
//    qDebug() << recvdata;
    clientsocket.write(mailfrom.append(usernametmp.append(">\r\n")));
    clientsocket.waitForReadyRead(1000);
    recvdata = clientsocket.readAll();
//    qDebug() << recvdata;
    //发送邮箱
    //qDebug() << "mail from:"<<mailfrom.append(usernametmp.append(">\r\n"));
    clientsocket.write(rcptto.append(recvaddrtmp.append(">\r\n")));
    clientsocket.waitForReadyRead(1000);
    recvdata = clientsocket.readAll();
//    qDebug() << recvdata;
    //接收邮箱
    //qDebug() << "rcp to:"<<rcptto.append(recvaddrtmp.append(">\r\n"));
    //data表示开始传输数据
    clientsocket.write("data\r\n");
    clientsocket.waitForReadyRead(1000);
    recvdata = clientsocket.readAll();
//    qDebug() << recvdata;
    usernametmp = username.toUtf8();
    recvaddrtmp = receiver.toUtf8();
    clientsocket.write(prefrom.append(usernametmp.append("\r\n")));
    clientsocket.write(preto.append(recvaddrtmp.append("\r\n")));
    clientsocket.write(presubject.append(subject.toLocal8Bit().append("\r\n")));
    clientsocket.write("\r\n");
    clientsocket.write(content.toLocal8Bit().append("\r\n"));
    clientsocket.write(".\r\n");
    clientsocket.waitForReadyRead(1000);
    recvdata = clientsocket.readAll();
//    qDebug() << recvdata;
    clientsocket.write("quit\r\n");
    clientsocket.waitForReadyRead(1000);
    recvdata = clientsocket.readAll();
//    qDebug() << recvdata;
    clientsocket.deleteLater();
}

void ScriptEngineEvaluation::messageBoxReturn(bool result)
{
    this->messageBoxResult = result;
    emit messageBoxReturned();
}
