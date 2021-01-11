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
    Q_INVOKABLE void messageBox(QString title, QString content);
    Q_INVOKABLE bool judgeShinePokemon();

public slots:
    void evaluate(QString script);
    void stop();
    void cameraCaptured(QImage *videoFrame);

private:
    QScriptEngine scriptEngine;
    bool needStop = false;
    QImage *videoFrame = Q_NULLPTR;

signals:
    void sendData(QString data);
    void finished();
    void hasException(QString ex);
    void messageBoxShow(QString title, QString content);
    void needCaptureCamera();
    void hasCaptureCamera();
};

#endif // SCRIPTENGINEEVALUATION_H
