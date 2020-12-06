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

public slots:
    void evaluate(QString script);
    void stop();

private:
    QScriptEngine scriptEngine;
    bool needStop = false;

signals:
    void sendData(QString data);
    void finished();
    void hasException(QString ex);
    void messageBoxShow(QString title, QString content);
};

#endif // SCRIPTENGINEEVALUATION_H
