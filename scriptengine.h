#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H

#include <QObject>
#include <QThread>
#include "scriptengineevaluation.h"

class ScriptEngine : public QObject
{
    Q_OBJECT

public:
    explicit ScriptEngine(QObject *parent = nullptr);
    ~ScriptEngine();
    QStringList getAllScripts();
    QString getScript(QString scriptName);
    QString addNewScript();
    void setScript(QString scriptName, QString content);
    void removeScript(QString scriptName);
    void runScript(QString content);
    void stopScript();

private:
    QThread scriptEngineEvaluationThread;
    ScriptEngineEvaluation scriptEngineEvaluation;

signals:
    void evaluate(QString script);
    void sendData(QString data);
//    void stop();
    void runScriptFinish();
    void hasException(QString ex);
};

#endif // SCRIPTENGINE_H
