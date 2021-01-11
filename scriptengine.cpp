#include "scriptengine.h"
#include <QDir>
#include <QTextStream>
#include "setting.h"
#include <QJsonArray>

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
    connect(&scriptEngineEvaluation, &ScriptEngineEvaluation::needCaptureCamera, this, [this]() {
        emit needCaptureCamera();
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
    QStringList files;
//    switch (this->scriptEngineMode) {
//    case ScriptEngineLocalMode: {
//        QDir dir(Setting::instance()->getScriptPath());
//        QStringList nameFilters;
//        nameFilters << "*.js";
//        files = dir.entryList(nameFilters, QDir::Files | QDir::Readable, QDir::Time);
//        break;
//    }
//    case ScriptEngineRemoteMode: {
//        for(int i = 0; i < this->filesJson.size(); i++) {
//            QJsonObject fileJson = this->filesJson[i].toObject();
//            QString name = fileJson["name"].toString("");
//            if (fileJson.contains("name") && !name.isEmpty()) {
//                files << name;
//            }
//        }
//        break;
//    }
//    }
    QDir dir(Setting::instance()->getScriptPath());
    QStringList nameFilters;
    nameFilters << "*.js";
    files = dir.entryList(nameFilters, QDir::Files | QDir::Readable, QDir::Time);
    return files;
}

QString ScriptEngine::getScript(QString scriptName)
{
    QString contents = "";
//    switch (this->scriptEngineMode) {
//    case ScriptEngineLocalMode: {
//        QFile scriptFile(Setting::instance()->getScriptPath() + "/" + scriptName + ".js");
//        if (scriptFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
//            QTextStream stream(&scriptFile);
//            contents = stream.readAll();
//            scriptFile.close();
//        }
//        break;
//    }
//    case ScriptEngineRemoteMode: {
//        for(int i = 0; i < this->filesJson.size(); i++) {
//            QJsonObject fileJson = this->filesJson[i].toObject();
//            QString name = fileJson["name"].toString("");
//            if (fileJson.contains("name") && !name.isEmpty()) {
//                if (name == (scriptName + ".js")) {
//                    contents = fileJson["contents"].toString("");
//                    break;
//                }
//            }
//        }
//        break;
//    }
//    }
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
    QString name = "";
//    switch (this->scriptEngineMode) {
//    case ScriptEngineLocalMode: {
//        name = "script";
//        int num = 1;
//        while(QFile::exists(Setting::instance()->getScriptPath() + "/" + name + ".js")) {
//            name = "script" + QString::number(num++);
//        }
//        QFile newFile(Setting::instance()->getScriptPath() + "/" + name + ".js");
//        newFile.open(QIODevice::ReadWrite | QIODevice::Text);
//        newFile.close();
//        break;
//    }
//    case ScriptEngineRemoteMode: {
//        break;
//    }
//    }
    name = "script";
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
//    switch (this->scriptEngineMode) {
//    case ScriptEngineLocalMode: {
//        break;
//    }
//    case ScriptEngineRemoteMode: {
//        break;
//    }
//    }
    QFile scriptFile(Setting::instance()->getScriptPath() + "/" + scriptName + ".js");
    if (scriptFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&scriptFile);
        stream << content;
        scriptFile.close();
    }
}

void ScriptEngine::removeScript(QString scriptName)
{
//    switch (this->scriptEngineMode) {
//    case ScriptEngineLocalMode: {
//        break;
//    }
//    case ScriptEngineRemoteMode: {
//        break;
//    }
//    }
    QFile::remove(Setting::instance()->getScriptPath() + "/" + scriptName + ".js");
}

void ScriptEngine::runScript(QString content)
{
    emit evaluate(content);
    emit runScriptStart();
}

void ScriptEngine::stopScript()
{
//    emit stop();
    scriptEngineEvaluation.stop();
}

bool ScriptEngine::renameScript(QString oldScriptName, QString newScriptName)
{
//    switch (this->scriptEngineMode) {
//    case ScriptEngineLocalMode: {
//        break;
//    }
//    case ScriptEngineRemoteMode: {
//        break;
//    }
//    }
//    return false;
    if (QFile::exists(Setting::instance()->getScriptPath() + "/" + newScriptName + ".js")) {
        return false;
    }
    return QFile::rename(Setting::instance()->getScriptPath() + "/" + oldScriptName + ".js",
                         Setting::instance()->getScriptPath() + "/" + newScriptName + ".js");
}

void ScriptEngine::cameraCaptured(QImage *videoFrame)
{
    scriptEngineEvaluation.cameraCaptured(videoFrame);
}

//void ScriptEngine::setScriptEngineMode(ScriptEngine::ScriptEngineMode scriptEngineMode, QJsonArray filesJson)
//{
//    this->scriptEngineMode = scriptEngineMode;
//    if (this->scriptEngineMode == ScriptEngineRemoteMode) {
//        this->filesJson = filesJson;
//    }
//}

//ScriptEngine::ScriptEngineMode ScriptEngine::getScriptEngineMode()
//{
//    return this->scriptEngineMode;
//}

//QJsonObject ScriptEngine::getAllScriptsIntoJson()
//{
//    QJsonObject json;
//    QJsonArray filesJson;
//    QDir dir(Setting::instance()->getScriptPath());
//    QStringList nameFilters;
//    nameFilters << "*.js";
//    for (QFileInfo fileInfo : dir.entryInfoList(nameFilters, QDir::Files | QDir::Readable, QDir::Time)) {
//        QFile scriptFile(fileInfo.absoluteFilePath());
//        QString contents = "";
//        if (scriptFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
//            QTextStream stream(&scriptFile);
//            contents = stream.readAll();
//            scriptFile.close();
//        }
//        QJsonObject fileJson;
//        fileJson.insert("name", fileInfo.fileName());
//        fileJson.insert("contents", contents);
//        filesJson.append(fileJson);
//    }
//    json.insert("files", filesJson);
//    return json;
//}
