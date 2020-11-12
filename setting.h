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
    void setScriptPath(QString path);

private:
    QSettings *settings = Q_NULLPTR;

signals:

};

#endif // SETTING_H
