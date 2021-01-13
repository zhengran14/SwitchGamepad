#include "setting.h"
#include <QStandardPaths>
#include <QDir>

Q_GLOBAL_STATIC(Setting, _setting)

Setting::Setting()
{
    settings = new QSettings(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/config.ini", QSettings::IniFormat);

    if (!settings->contains("scriptPath")) {
        setScriptPath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/scripts");
    }
    else {
        QDir dir;
        dir.mkpath(getScriptPath());
    }
    if (!settings->contains("remotePort")) {
        setRemotePort(8080);
    }
    if (!settings->contains("serverUrl")) {
        setServerUrl("xxx.com");
    }
    if (!settings->contains("serverPort")) {
        setServerPort(8080);
    }
    if (!settings->contains("liveUrl")) {
        setLiveUrl("rtmp://xxx.com/live/xxx");
    }
}

Setting::~Setting()
{
    if (settings == Q_NULLPTR) {
        settings->deleteLater();
    }
}

Setting* Setting::instance()
{
    return _setting;
}

void Setting::setScriptPath(QString path)
{
    QDir dir;
    dir.mkpath(path);
    settings->setValue("scriptPath", path);
}
