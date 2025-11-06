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
#include <QSslSocket>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QUrl>

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
    QSslSocket socket;
    socket.connectToHostEncrypted("smtp.qq.com", 465);
    if (!socket.waitForEncrypted(5000)) {
        qDebug() << "SSL handshake failed:" << socket.errorString();
        return;
    }

    QByteArray recvData;

    auto readResponse = [&]() -> QByteArray {
        if (socket.waitForReadyRead(5000)) {
            QByteArray data = socket.readAll();
            qDebug() << data;
            return data;
        }
        return QByteArray();
    };

    // 1. EHLO
    socket.write("EHLO smtp.qq.com\r\n");
    readResponse();

    // 2. AUTH LOGIN
    socket.write("AUTH LOGIN\r\n");
    readResponse();

    // 3. 用户名 (Base64)
    socket.write(username.toUtf8().toBase64() + "\r\n");
    readResponse();

    // 4. 授权码 (Base64)
    socket.write(password.toUtf8().toBase64() + "\r\n");
    readResponse();

    // 5. MAIL FROM
    socket.write("MAIL FROM:<" + username.toUtf8() + ">\r\n");
    readResponse();

    // 6. RCPT TO
    socket.write("RCPT TO:<" + receiver.toUtf8() + ">\r\n");
    readResponse();

    // 7. DATA
    socket.write("DATA\r\n");
    readResponse();

    // 8. 邮件头 + 内容
    socket.write("From: " + username.toUtf8() + "\r\n");
    socket.write("To: " + receiver.toUtf8() + "\r\n");
    socket.write("Subject: " + subject.toUtf8() + "\r\n");
    socket.write("\r\n"); // 空行分隔头和正文
    socket.write(content.toUtf8() + "\r\n");
    socket.write(".\r\n"); // 结束 DATA
    readResponse();

    // 9. QUIT
    socket.write("QUIT\r\n");
    readResponse();

    socket.disconnectFromHost();
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

void ScriptEngineEvaluation::bark(QString url, QStringList devices, QString level, QString title, QString content)
{
    QJsonObject json;
    json["title"] = title;
    json["body"] = content;
    json["level"] = level;

    // 构建 device_keys 数组
    QJsonArray deviceArray;
    for (const QString &dev : devices) {
        deviceArray.append(dev);
    }
    json["device_keys"] = deviceArray;

    // 转为 QByteArray
    QJsonDocument doc(json);
    QByteArray data = doc.toJson(QJsonDocument::Compact);

    // 网络请求
    QNetworkRequest request{ QUrl(url) };
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    // 发送 POST 请求
    QNetworkReply *reply = manager->post(request, data);

    // 异步处理响应
    connect(reply, &QNetworkReply::finished, this, [reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << "Bark push failed:" << reply->errorString();
        } else {
            QByteArray response = reply->readAll();
            qDebug() << "Bark push success:" << response;
        }
        reply->deleteLater();
    });
}
