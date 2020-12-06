#include "scriptengine.h"
#include <QDir>
#include <QTextStream>
#include "setting.h"

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
    connect(&scriptEngineEvaluation, &ScriptEngineEvaluation::messageBoxShow, this, [this](QString title, QString content) {
        emit messageBoxShow(title, content);
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
    QDir dir(Setting::instance()->getScriptPath());
    QStringList nameFilters;
    nameFilters << "*.js";
    QStringList files = dir.entryList(nameFilters, QDir::Files | QDir::Readable, QDir::Time);
    return files;
}

QString ScriptEngine::getScript(QString scriptName)
{
    QString contents = "";
    QFile scriptFile(Setting::instance()->getScriptPath() + "/" + scriptName + ".js");
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
    while(QFile::exists(Setting::instance()->getScriptPath() + "/" + name + ".js")) {
        name = "script" + QString::number(num++);
    }
    QFile newFile(Setting::instance()->getScriptPath() + "/" + name + ".js");
    newFile.open(QIODevice::ReadWrite | QIODevice::Text);
    newFile.close();
    return name;
}

void ScriptEngine::setScript(QString scriptName, QString content)
{
    QFile scriptFile(Setting::instance()->getScriptPath() + "/" + scriptName + ".js");
    if (scriptFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&scriptFile);
        stream << content;
        scriptFile.close();
    }
}

void ScriptEngine::removeScript(QString scriptName)
{
    QFile::remove(Setting::instance()->getScriptPath() + "/" + scriptName + ".js");
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

bool ScriptEngine::renameScript(QString oldScriptName, QString newScriptName)
{
    if (QFile::exists(Setting::instance()->getScriptPath() + "/" + newScriptName + ".js")) {
        return false;
    }
    return QFile::rename(Setting::instance()->getScriptPath() + "/" + oldScriptName + ".js",
                  Setting::instance()->getScriptPath() + "/" + newScriptName + ".js");
}
