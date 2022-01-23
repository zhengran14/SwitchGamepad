#ifndef SCRIPTENGINEEVALUATION_H
#define SCRIPTENGINEEVALUATION_H

#include <QObject>
#include <QScriptEngine>
#include "opencv2/core/types.hpp"

class ScriptEngineEvaluation : public QObject
{
    Q_OBJECT
public:
    explicit ScriptEngineEvaluation(QObject *parent = nullptr);
    ~ScriptEngineEvaluation();
    Q_INVOKABLE void sleep(float sec);
    Q_INVOKABLE void sleepClock(int ms);
    Q_INVOKABLE void pressButton(QString string, float sec);
    Q_INVOKABLE void pressButtonClock(QString string, int ms);
    Q_INVOKABLE void pressDownButton(QString string);
    Q_INVOKABLE void releaseButton();
    Q_INVOKABLE bool messageBox(QString title, QString content);
    Q_INVOKABLE bool judgeShinePokemon();
    Q_INVOKABLE bool judgeShinePokemon2();
    Q_INVOKABLE bool judgeShinePokemonCustome(QString path);
    Q_INVOKABLE QString judgeShinePokemonTest(QString path);
    Q_INVOKABLE bool judgeCapture(QString path, int x, int y, int offsetX = -1, int offsetY = -1, int offsetWidth = -1, int offsetHeight = -1, int method = 0);
    Q_INVOKABLE QString judgeCaptureTest(QString path, int offsetX = -1, int offsetY = -1, int offsetWidth = -1, int offsetHeight = -1, int method = 0);
    Q_INVOKABLE QString judgeCaptureTest(QString sourcePath, QString templatePath, int offsetX = -1, int offsetY = -1, int offsetWidth = -1, int offsetHeight = -1, int method = 0, bool isShow = false);
    Q_INVOKABLE void capture(QString path, int x = -1, int y = -1, int width = -1, int height = -1);
    Q_INVOKABLE void capture(QString sourcePath, QString svePath, int x = -1, int y = -1, int width = -1, int height = -1);
    Q_INVOKABLE void statusText(QString text);
    Q_INVOKABLE void mail(QString username, QString password, QString receiver, QString subject, QString content);
    Q_INVOKABLE QString getCaptureString(int offsetX = -1, int offsetY = -1, int offsetWidth = -1, int offsetHeight = -1, QString tessdata = "chi_sim");
    Q_INVOKABLE QString getCaptureStringTest(QString tessdataPath, QString tessdata, int offsetX = -1, int offsetY = -1, int offsetWidth = -1, int offsetHeight = -1);
    Q_INVOKABLE QString getCaptureStringTest(QString imgPath, QString tessdataPath, QString tessdata, int offsetX = -1, int offsetY = -1, int offsetWidth = -1, int offsetHeight = -1);
    Q_INVOKABLE int compareTest(QString imgPath1, QString imgPath2, int method);
    Q_INVOKABLE int compare(QString imgPath1, int offsetX, int offsetY, int offsetWidth, int offsetHeight, int method = 1);
    void messageBoxReturn(bool result);

public slots:
    void evaluate(QString script);
    void stop();
    void cameraCaptured(QImage *videoFrame);

private:
    QScriptEngine scriptEngine;
    bool needStop = false;
    QImage *videoFrame = Q_NULLPTR;
    bool messageBoxResult = false;
    void abortscriptEngineEvaluation();

signals:
    void sendData(QString data);
    void finished();
    void hasException(QString ex);
    void messageBoxShow(QString title, QString content);
    void needCaptureCamera();
    void hasCaptureCamera();
    void messageBoxReturned();
    void setStatusText(QString text);
    void cvShow(QString sourcePath, QString templatePath, cv::Point point);
};

#endif // SCRIPTENGINEEVALUATION_H
