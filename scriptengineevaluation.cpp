#include "scriptengineevaluation.h"
#include <QThread>
#include <QTime>
#include <QEventLoop>
#include <QDebug>
#include <QImage>
#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"
#include <utils.h>

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
//    videoFrame->save("C:/Project/123.jpg");
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

void ScriptEngineEvaluation::messageBoxReturn(bool result)
{
    this->messageBoxResult = result;
    emit messageBoxReturned();
}
