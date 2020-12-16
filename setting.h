#ifndef SETTING_H
#define SETTING_H

#include <QObject>
#include <QSettings>

class Setting : public QObject
{
    Q_OBJECT
public:
    Setting();
    ~Setting();
    static Setting* instance();
    QString getScriptPath() {
        return settings->value("scriptPath").toString();
    };
    quint16 getRemotePort() {
        return settings->value("remotePort").toUInt();
    };
    QString getServerUrl() {
        return settings->value("serverUrl").toString();
    };
    quint16 getServerPort() {
        return settings->value("serverPort").toUInt();
    };
    QString getLiveUrl() {
        return settings->value("liveUrl").toString();
    };
    void setScriptPath(QString path);
    void setRemotePort(quint16 remotePort) {
        settings->setValue("remotePort", remotePort);
    };
    void setServerUrl(QString serverUrl) {
        settings->setValue("serverUrl", serverUrl);
    };
    void setServerPort(quint16 serverPort) {
        settings->setValue("serverPort", serverPort);
    };
    void setLiveUrl(QString liveUrl) {
        settings->setValue("liveUrl", liveUrl);
    };

private:
    QSettings *settings = Q_NULLPTR;

signals:

};

#endif // SETTING_H
