#include "scriptengineevaluation.h"
#include <QThread>
#include <QTime>

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
        emit hasException(tr("Uncaught exception at line") + QString::number(line) + tr(": ") + result.toString());
    }
    emit finished();
}

void ScriptEngineEvaluation::stop()
{
    needStop = true;
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
