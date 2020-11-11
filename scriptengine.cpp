#include "scriptengine.h"
#include <QDir>
#include <QTextStream>

ScriptEngine::ScriptEngine(QObject *parent) : QObject(parent)
{
    scriptEngineEvaluation.moveToThread(&scriptEngineEvaluationThread);
//    connect(&scriptEngineEvaluationThread, &QThread::finished, &scriptEngineEvaluation, &QObject::deleteLater);
    connect(this, &ScriptEngine::evaluate, &scriptEngineEvaluation, &ScriptEngineEvaluation::evaluate);
//    connect(this, &ScriptEngine::stop, &scriptEngineEvaluation, &ScriptEngineEvaluation::stop);
    connect(&scriptEngineEvaluation, &ScriptEngineEvaluation::finished, this, [this]() {
        emit runScriptFinish();
    });
    connect(&scriptEngineEvaluation, &ScriptEngineEvaluation::sendData, this, [this](QString data) {
        emit sendData(data);
    });
    connect(&scriptEngineEvaluation, &ScriptEngineEvaluation::hasException, this, [this](QString ex) {
        emit hasException(ex);
    });
    scriptEngineEvaluationThread.start();
}

ScriptEngine::~ScriptEngine()
{
    scriptEngineEvaluation.stop();
    scriptEngineEvaluationThread.quit();
    scriptEngineEvaluationThread.wait();
    scriptEngineEvaluationThread.deleteLater();
}

QStringList ScriptEngine::getAllScripts()
{
    QDir dir("/Users/rabbit/Documents/develop/SwitchGamepad/js");
    QStringList nameFilters;
    nameFilters << "*.js";
    QStringList files = dir.entryList(nameFilters, QDir::Files | QDir::Readable, QDir::Time);
    return files;
}

QString ScriptEngine::getScript(QString scriptName)
{
    QString contents = "";
    QFile scriptFile("/Users/rabbit/Documents/develop/SwitchGamepad/js/" + scriptName + ".js");
    if (scriptFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&scriptFile);
        contents = stream.readAll();
        scriptFile.close();
    }
    return contents;
}

QString ScriptEngine::addNewScript()
{
    QString name = "script";
    int num = 1;
    while(QFile::exists("/Users/rabbit/Documents/develop/SwitchGamepad/js/" + name + ".js")) {
        name = "script" + QString::number(num++);
    }
    QFile newFile("/Users/rabbit/Documents/develop/SwitchGamepad/js/" + name + ".js");
    newFile.open(QIODevice::ReadWrite | QIODevice::Text);
    newFile.close();
    return name;
}

void ScriptEngine::setScript(QString scriptName, QString content)
{
    QFile scriptFile("/Users/rabbit/Documents/develop/SwitchGamepad/js/" + scriptName + ".js");
    if (scriptFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&scriptFile);
        stream << content;
        scriptFile.close();
    }
}

void ScriptEngine::removeScript(QString scriptName)
{
    QFile::remove("/Users/rabbit/Documents/develop/SwitchGamepad/js/" + scriptName + ".js");
}

void ScriptEngine::runScript(QString content)
{
    emit evaluate(content);
}

void ScriptEngine::stopScript()
{
//    emit stop();
    scriptEngineEvaluation.stop();
}
