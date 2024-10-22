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
#include <QTimer>
#include <QMetaType>
#include <tesseract/baseapi.h>
#ifndef TESSERACT_V5
#include <tesseract/strngs.h>
#endif
#include <leptonica/allheaders.h>
#include "setting.h"

ScriptEngineEvaluation::ScriptEngineEvaluation(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<cv::Point>("cv::Point");
    QJSEngine::setObjectOwnership(this, QJSEngine::CppOwnership);
//    QJSValue jsMetaObject = scriptEngine.newQMetaObject(&ScriptEngineEvaluation::staticMetaObject);
//    scriptEngine.globalObject().setProperty("gp", jsMetaObject);
    scriptEngine.globalObject().setProperty("gp", scriptEngine.newQObject(this));
//    engine.evaluate("g_sp.open('asd', 123);");
//    QScriptValue sriptValue = scriptEngine.newFunction(sleep);
//    scriptEngine.globalObject().setProperty("myAdd", sriptValue);
}

ScriptEngineEvaluation::~ScriptEngineEvaluation()
{
    abortscriptEngineEvaluation();
    scriptEngine.deleteLater();
}

void ScriptEngineEvaluation::evaluate(QString script)
{
    stop();
    needStop = false;
    scriptEngine.setInterrupted(false);
    QJSValue result = scriptEngine.evaluate(script);
    if (result.isError()) {
        emit hasException(tr("Uncaught exception at line ") + QString::number(result.property("lineNumber").toInt()) + tr(": ") + result.toString());
    }
    emit finished();
}

void ScriptEngineEvaluation::stop()
{
    needStop = true;
    abortscriptEngineEvaluation();
}

void ScriptEngineEvaluation::cameraCaptured(QImage *videoFrame)
{
    this->videoFrame = videoFrame;
    emit hasCaptureCamera();
}

void ScriptEngineEvaluation::sleep(float sec)
{
    if (needStop) {
        abortscriptEngineEvaluation();
        return;
    }
//    QThread::msleep((int)(time * 1000));

    for (int i = 1; i <= sec * 10; i++) {
        if (needStop) {
            abortscriptEngineEvaluation();
            return;
        }
//        QThread::msleep((int)(100));
        QEventLoop eventLoop;
        QTimer::singleShot(100, Qt::PreciseTimer, &eventLoop, &QEventLoop::quit);
        eventLoop.exec(QEventLoop::ExcludeUserInputEvents);
    }

//    QTime timer = QTime::currentTime().addMSecs((int)(sec * 1000));
//    while (QTime::currentTime() < timer) {
//        if (needStop) {
//            abortscriptEngineEvaluation();
//            return;
//        }
//        QThread::msleep((int)(100));
    //    }
}

void ScriptEngineEvaluation::sleepClock(int ms)
{
    if (needStop) {
        abortscriptEngineEvaluation();
        return;
    }


    QEventLoop eventLoop;
    QTimer::singleShot(ms, Qt::PreciseTimer, &eventLoop, &QEventLoop::quit);
    eventLoop.exec(QEventLoop::ExcludeUserInputEvents);
}

void ScriptEngineEvaluation::pressButton(QString string, float sec)
{
    if (needStop) {
        abortscriptEngineEvaluation();
        return;
    }
    emit sendData(string);
    sleep(sec);
    if (needStop) {
        abortscriptEngineEvaluation();
        return;
    }
    emit sendData("RELEASE");
}

void ScriptEngineEvaluation::pressButtonClock(QString string, int ms)
{
    if (needStop) {
        abortscriptEngineEvaluation();
        return;
    }
    emit sendData(string);
    sleepClock(ms);
    if (needStop) {
        abortscriptEngineEvaluation();
        return;
    }
    emit sendData("RELEASE");
}

void ScriptEngineEvaluation::pressDownButton(QString string)
{
    if (needStop) {
        abortscriptEngineEvaluation();
        return;
    }
    emit sendData(string);
}

void ScriptEngineEvaluation::releaseButton()
{
    if (needStop) {
        abortscriptEngineEvaluation();
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

void ScriptEngineEvaluation::capture(QString path, int x, int y, int width, int height)
{
//    bool result = false;
    QEventLoop* eventLoop = new QEventLoop();
    connect(this, &ScriptEngineEvaluation::hasCaptureCamera, eventLoop, &QEventLoop::quit);
    emit needCaptureCamera();
    eventLoop->exec();
    eventLoop->deleteLater();
//    qDebug() << (videoFrame == Q_NULLPTR ? QSize() : videoFrame->size());
//    videoFrame->save("123.jpg");
    if (videoFrame != Q_NULLPTR) {
        // 110, 590, 100, 45
        if (x < 0 || y < 0 || width <= 0 || height <= 0) {
            videoFrame->save(path);
        }
        else {
            QImage newCut = videoFrame->copy(x, y, width, height);
            newCut.save(path);
        }
    }
    //    return result;
}

void ScriptEngineEvaluation::capture(QString sourcePath, QString svePath, int x, int y, int width, int height)
{
    QImage img(sourcePath);
    // 110, 590, 100, 45
    if (x < 0 || y < 0 || width <= 0 || height <= 0) {
        img.save(svePath);
    }
    else {
        QImage newCut = img.copy(x, y, width, height);
        newCut.save(svePath);
    }
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

bool ScriptEngineEvaluation::judgeCapture(QString path, int x, int y, int offsetX, int offsetY, int offsetWidth, int offsetHeight, int method)
{
    bool result = false;
    QEventLoop* eventLoop = new QEventLoop();
    connect(this, &ScriptEngineEvaluation::hasCaptureCamera, eventLoop, &QEventLoop::quit);
    emit needCaptureCamera();
    eventLoop->exec();
    eventLoop->deleteLater();
    if (videoFrame != Q_NULLPTR) {
        QImage img(path);
        cv::Mat captureFrame;
        if (offsetX < 0 || offsetY < 0 || offsetWidth <= 0 || offsetHeight <= 0) {
            captureFrame = Utils::QImage2cvMat(*videoFrame);
        }
        else {
            QImage cut = videoFrame->copy(offsetX, offsetY, offsetWidth, offsetHeight);
            captureFrame = Utils::QImage2cvMat(cut);
        }
        cv::Mat captureFrame2;
        cv::cvtColor(captureFrame, captureFrame2, cv::COLOR_BGR2RGB);
        cv::Mat template1 = Utils::QImage2cvMat(img);
        cv::Mat template2;
        cv::cvtColor(template1, template2, cv::COLOR_BGR2RGB);
        cv::Mat dstImg;
        dstImg.create(captureFrame2.dims, captureFrame2.size, captureFrame2.type());
        // enum { TM_SQDIFF=0, TM_SQDIFF_NORMED=1, TM_CCORR=2, TM_CCORR_NORMED=3, TM_CCOEFF=4, TM_CCOEFF_NORMED=5 };
        if (method < 0 || method > 5) {
            method = 0;
        }
        cv::matchTemplate(captureFrame2, template2, dstImg, method);
        cv::Point minPoint;
        cv::Point maxPoint;
        double *minVal = 0;
        double *maxVal = 0;
        cv::minMaxLoc(dstImg, minVal, maxVal, &minPoint,&maxPoint);
        maxPoint = cv::Point(minPoint.x + template2.cols, minPoint.y + template2.rows);
        // 110 590 210 635
//        qDebug() << minPoint.x << minPoint.y << maxPoint.x << maxPoint.y;
        if (offsetX >= 0 && offsetY >= 0) {
            x -= offsetX;
            y -= offsetY;
        }
        if (minPoint.x == x && minPoint.y == y) {
            result = true;
        }
        dstImg.release();
        captureFrame.release();
        captureFrame2.release();
        template1.release();
        template2.release();
    }
    return result;
}

QString ScriptEngineEvaluation::judgeCaptureTest(QString path, int offsetX, int offsetY, int offsetWidth, int offsetHeight, int method)
{
    QString result = "";
    QEventLoop* eventLoop = new QEventLoop();
    connect(this, &ScriptEngineEvaluation::hasCaptureCamera, eventLoop, &QEventLoop::quit);
    emit needCaptureCamera();
    eventLoop->exec();
    eventLoop->deleteLater();
    if (videoFrame != Q_NULLPTR) {
        QImage img(path);
        cv::Mat captureFrame;
        if (offsetX < 0 || offsetY < 0 || offsetWidth <= 0 || offsetHeight <= 0) {
            captureFrame = Utils::QImage2cvMat(*videoFrame);
        }
        else {
            QImage cut = videoFrame->copy(offsetX, offsetY, offsetWidth, offsetHeight);
            captureFrame = Utils::QImage2cvMat(cut);
        }
        cv::Mat captureFrame2;
        cv::cvtColor(captureFrame, captureFrame2, cv::COLOR_BGR2RGB);
        cv::Mat template1 = Utils::QImage2cvMat(img);
        cv::Mat template2;
        cv::cvtColor(template1, template2, cv::COLOR_BGR2RGB);
        cv::Mat dstImg;
        dstImg.create(captureFrame2.dims, captureFrame2.size, captureFrame2.type());
        // enum { TM_SQDIFF=0, TM_SQDIFF_NORMED=1, TM_CCORR=2, TM_CCORR_NORMED=3, TM_CCOEFF=4, TM_CCOEFF_NORMED=5 };
        if (method < 0 || method > 5) {
            method = 0;
        }
        cv::matchTemplate(captureFrame2, template2, dstImg, method);
        cv::Point minPoint;
        cv::Point maxPoint;
        double *minVal = 0;
        double *maxVal = 0;
        cv::minMaxLoc(dstImg, minVal, maxVal, &minPoint,&maxPoint);
        maxPoint = cv::Point(minPoint.x + template2.cols, minPoint.y + template2.rows);
        // 110 590 210 635
//        qDebug() << minPoint.x << minPoint.y << maxPoint.x << maxPoint.y;
        result = QString("%1, %2").arg(minPoint.x).arg(minPoint.y);
        if (offsetX >= 0 && offsetY >= 0) {
            result += QString(", %1, %2").arg(minPoint.x + offsetX).arg(minPoint.y + offsetY);
        }
        dstImg.release();
        captureFrame.release();
        captureFrame2.release();
        template1.release();
        template2.release();
    }
    return result;
}

QString ScriptEngineEvaluation::judgeCaptureTest(QString sourcePath, QString templatePath, int offsetX, int offsetY, int offsetWidth, int offsetHeight, int method, bool isShow)
{
    QString result = "";
    QEventLoop* eventLoop = new QEventLoop();
    connect(this, &ScriptEngineEvaluation::hasCaptureCamera, eventLoop, &QEventLoop::quit);
    emit needCaptureCamera();
    eventLoop->exec();
    eventLoop->deleteLater();
    QImage img1(sourcePath);
    QImage img2(templatePath);
    cv::Mat captureFrame;
    if (offsetX < 0 || offsetY < 0 || offsetWidth <= 0 || offsetHeight <= 0) {
        captureFrame = Utils::QImage2cvMat(img1);
    }
    else {
        QImage cut = img1.copy(offsetX, offsetY, offsetWidth, offsetHeight);
        captureFrame = Utils::QImage2cvMat(cut);
    }
//    cv::Mat captureFrame = Utils::QImage2cvMat(img1);
    cv::Mat captureFrame2;
    cv::cvtColor(captureFrame, captureFrame2, cv::COLOR_BGR2RGB);
    cv::Mat template1 = Utils::QImage2cvMat(img2);
    cv::Mat template2;
    cv::cvtColor(template1, template2, cv::COLOR_BGR2RGB);
    cv::Mat dstImg;
    dstImg.create(captureFrame2.dims, captureFrame2.size, captureFrame2.type());
    // enum { TM_SQDIFF=0, TM_SQDIFF_NORMED=1, TM_CCORR=2, TM_CCORR_NORMED=3, TM_CCOEFF=4, TM_CCOEFF_NORMED=5 };
    cv::matchTemplate(captureFrame2, template2, dstImg, method);
    cv::Point minPoint;
    cv::Point maxPoint;
    double *minVal = 0;
    double *maxVal = 0;
    cv::minMaxLoc(dstImg, minVal, maxVal, &minPoint,&maxPoint);
    maxPoint = cv::Point(minPoint.x + template2.cols, minPoint.y + template2.rows);
    // 110 590 210 635
//        qDebug() << minPoint.x << minPoint.y << maxPoint.x << maxPoint.y;
    result = QString("%1, %2").arg(minPoint.x).arg(minPoint.y);
    if (offsetX >= 0 && offsetY >= 0) {
        result += QString(", %1, %2").arg(minPoint.x + offsetX).arg(minPoint.y + offsetY);
    }
    if (isShow) {
        emit cvShow(sourcePath, templatePath, minPoint);
//        cv::Mat mask = captureFrame.clone();
//        rectangle(mask, maxPoint, cv::Point(maxPoint.x + template1.cols, maxPoint.y + template1.rows), cv::Scalar(0, 255, 0), 2, 8, 0);
//        cv::imshow("mask",mask);
//        mask.release();
    }
    dstImg.release();
    captureFrame.release();
    captureFrame2.release();
    template1.release();
    template2.release();
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

QString ScriptEngineEvaluation::getCaptureString(int offsetX, int offsetY, int offsetWidth, int offsetHeight, QString tessdata)
{
    QString result = "";

    QEventLoop* eventLoop = new QEventLoop();
    connect(this, &ScriptEngineEvaluation::hasCaptureCamera, eventLoop, &QEventLoop::quit);
    emit needCaptureCamera();
    eventLoop->exec();
    eventLoop->deleteLater();
    if (videoFrame != Q_NULLPTR) {

        tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
        std::string s_tessdata = tessdata.toStdString();
        std::string s_tessdataPath = Setting::instance()->getTessdataPath().toStdString();
        if (api->Init(s_tessdataPath.c_str(), tessdata.isEmpty() ? "chi_sim" : s_tessdata.c_str()))
        {
            api->End();
            return result;
        }

        cv::Mat captureFrame;
        if (offsetX < 0 || offsetY < 0 || offsetWidth <= 0 || offsetHeight <= 0) {
            captureFrame = Utils::QImage2cvMat(*videoFrame);
        }
        else {
            QImage cut = videoFrame->copy(offsetX, offsetY, offsetWidth, offsetHeight);
            captureFrame = Utils::QImage2cvMat(cut);
        }
        cv::Mat captureFrame2;
        cv::cvtColor(captureFrame, captureFrame2, cv::COLOR_BGR2GRAY);
        api->SetImage((uchar*)captureFrame2.data, captureFrame2.cols, captureFrame2.rows, 1, captureFrame2.cols);
        char *outText = api->GetUTF8Text();
        result = QString::fromUtf8(outText);

        captureFrame.release();
        captureFrame2.release();
        api->End();
        delete [] outText;
    }
    return result;
}

QString ScriptEngineEvaluation::getCaptureStringTest(QString tessdataPath, QString tessdata, int offsetX, int offsetY, int offsetWidth, int offsetHeight)
{
    QString result = "";

    QEventLoop* eventLoop = new QEventLoop();
    connect(this, &ScriptEngineEvaluation::hasCaptureCamera, eventLoop, &QEventLoop::quit);
    emit needCaptureCamera();
    eventLoop->exec();
    eventLoop->deleteLater();
    if (videoFrame != Q_NULLPTR) {

        tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
        std::string s_tessdata = tessdata.toStdString();
        std::string s_tessdataPath = tessdataPath.toStdString();
        if (api->Init(s_tessdataPath.c_str(), tessdata.isEmpty() ? "chi_sim" : s_tessdata.c_str()))
        {
            api->End();
            return result;
        }

        cv::Mat captureFrame;
        if (offsetX < 0 || offsetY < 0 || offsetWidth <= 0 || offsetHeight <= 0) {
            captureFrame = Utils::QImage2cvMat(*videoFrame);
        }
        else {
            QImage cut = videoFrame->copy(offsetX, offsetY, offsetWidth, offsetHeight);
            captureFrame = Utils::QImage2cvMat(cut);
        }
        cv::Mat captureFrame2;
        cv::cvtColor(captureFrame, captureFrame2, cv::COLOR_BGR2GRAY);
        api->SetImage((uchar*)captureFrame2.data, captureFrame2.cols, captureFrame2.rows, 1, captureFrame2.cols);
        char *outText = api->GetUTF8Text();
        result = QString::fromUtf8(outText);

        captureFrame.release();
        captureFrame2.release();
        api->End();
        delete [] outText;
    }
    return result;
}

QString ScriptEngineEvaluation::getCaptureStringTest(QString imgPath, QString tessdataPath, QString tessdata, int offsetX, int offsetY, int offsetWidth, int offsetHeight)
{
    QString result = "";

    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
    std::string s_tessdata = tessdata.toStdString();
    std::string s_tessdataPath = tessdataPath.toStdString();
    if (api->Init(s_tessdataPath.c_str(), tessdata.isEmpty() ? "chi_sim" : s_tessdata.c_str()))
    {
        api->End();
        return result;
    }

    QImage img1(imgPath);
    cv::Mat captureFrame;
    if (offsetX < 0 || offsetY < 0 || offsetWidth <= 0 || offsetHeight <= 0) {
        captureFrame = Utils::QImage2cvMat(img1);
    }
    else {
        QImage cut = img1.copy(offsetX, offsetY, offsetWidth, offsetHeight);
        captureFrame = Utils::QImage2cvMat(cut);
    }
    cv::Mat captureFrame2;
    cv::cvtColor(captureFrame, captureFrame2, cv::COLOR_BGR2GRAY);
    api->SetImage((uchar*)captureFrame2.data, captureFrame2.cols, captureFrame2.rows, 1, captureFrame2.cols);
    char *outText = api->GetUTF8Text();
    result = QString::fromUtf8(outText);

    captureFrame.release();
    captureFrame2.release();
    api->End();
    delete [] outText;

    return result;
}

int ScriptEngineEvaluation::compareTest(QString imgPath1, QString imgPath2, int method)
{
    cv::Mat img1 = cv::imread(imgPath1.toStdString());
    cv::Mat img2 = cv::imread(imgPath2.toStdString());

    int res = Utils::SimilarImage(img1, img2, method);

    img1.release();
    img2.release();
    return res;
}

int ScriptEngineEvaluation::compare(QString imgPath1, int offsetX, int offsetY, int offsetWidth, int offsetHeight, int method)
{
    int res = 0;

    QEventLoop* eventLoop = new QEventLoop();
    connect(this, &ScriptEngineEvaluation::hasCaptureCamera, eventLoop, &QEventLoop::quit);
    emit needCaptureCamera();
    eventLoop->exec();
    eventLoop->deleteLater();
    if (videoFrame != Q_NULLPTR) {
        cv::Mat captureFrame;
        if (offsetX < 0 || offsetY < 0 || offsetWidth <= 0 || offsetHeight <= 0) {
            captureFrame = Utils::QImage2cvMat(*videoFrame);
        }
        else {
            QImage cut = videoFrame->copy(offsetX, offsetY, offsetWidth, offsetHeight);
            captureFrame = Utils::QImage2cvMat(cut);
        }
        cv::Mat captureFrame2;
        cv::cvtColor(captureFrame, captureFrame2, cv::COLOR_BGR2RGB);

        cv::Mat img1 = cv::imread(imgPath1.toStdString());
        res = Utils::SimilarImage(img1, captureFrame2, method);

        captureFrame.release();
        captureFrame2.release();
        img1.release();
    }
    return res;
}

void ScriptEngineEvaluation::messageBoxReturn(bool result)
{
    this->messageBoxResult = result;
    emit messageBoxReturned();
}

void ScriptEngineEvaluation::abortscriptEngineEvaluation()
{
    emit sendData("RELEASE");
    scriptEngine.setInterrupted(true);
}
