#ifndef SCRIPTENGINEEVALUATION_H
#define SCRIPTENGINEEVALUATION_H

#include <QObject>
#include <QScriptEngine>

class ScriptEngineEvaluation : public QObject
{
    Q_OBJECT
public:
    explicit ScriptEngineEvaluation(QObject *parent = nullptr);
    ~ScriptEngineEvaluation();
    Q_INVOKABLE void sleep(float sec);
    Q_INVOKABLE void pressButton(QString string, float sec);
    Q_INVOKABLE bool messageBox(QString title, QString content);
    Q_INVOKABLE bool judgeShinePokemon();
    Q_INVOKABLE bool judgeShinePokemon2();
    Q_INVOKABLE bool judgeShinePokemonCustome(QString path);
    Q_INVOKABLE QString judgeShinePokemonTest(QString path);
    Q_INVOKABLE bool judgeCapture(QString path, int x, int y, int offsetX = -1, int offsetY = -1, int offsetWidth = -1, int offsetHeight = -1);
    Q_INVOKABLE QString judgeCaptureTest(QString path, int offsetX = -1, int offsetY = -1, int offsetWidth = -1, int offsetHeight = -1);
    Q_INVOKABLE void capture(QString path, int x = -1, int y = -1, int width = -1, int height = -1);
    Q_INVOKABLE void statusText(QString text);
    Q_INVOKABLE void mail(QString username, QString password, QString receiver, QString subject, QString content);
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

signals:
    void sendData(QString data);
    void finished();
    void hasException(QString ex);
    void messageBoxShow(QString title, QString content);
    void needCaptureCamera();
    void hasCaptureCamera();
    void messageBoxReturned();
    void setStatusText(QString text);
};

#endif // SCRIPTENGINEEVALUATION_H
