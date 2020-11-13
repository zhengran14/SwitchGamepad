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
