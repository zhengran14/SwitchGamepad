#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H

#include <QObject>
#include <QThread>
#include <QJsonObject>
#include <QJsonArray>
#include "scriptengineevaluation.h"

class ScriptEngine : public QObject
{
    Q_OBJECT

public:
//    enum ScriptEngineMode {
//        ScriptEngineLocalMode,
//        ScriptEngineRemoteMode
//    };
    explicit ScriptEngine(QObject *parent = nullptr);
    ~ScriptEngine();
    QStringList getAllScripts();
    QString getScript(QString scriptName);
    QString addNewScript();
    void setScript(QString scriptName, QString content);
    void removeScript(QString scriptName);
    void runScript(QString content);
    void stopScript();
    bool renameScript(QString oldScriptName, QString newScriptName);
//    void setScriptEngineMode(ScriptEngineMode scriptEngineMode, QJsonArray filesJson = QJsonArray());
//    ScriptEngineMode getScriptEngineMode();
//    QJsonObject getAllScriptsIntoJson();
    void messageBoxReturn(bool result);

public slots:
    void cameraCaptured(QImage *videoFrame);

private:
    QThread scriptEngineEvaluationThread;
    ScriptEngineEvaluation scriptEngineEvaluation;
//    ScriptEngineMode scriptEngineMode = ScriptEngineLocalMode;
//    QJsonArray filesJson;

signals:
    void evaluate(QString script);
    void sendData(QString data);
//    void stop();
    void runScriptFinish();
    void hasException(QString ex);
    void messageBoxShow(QString title, QString content);
    void runScriptStart();
    void needCaptureCamera();
    void setStatusText(QString text);
};

#endif // SCRIPTENGINE_H
