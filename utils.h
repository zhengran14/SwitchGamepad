#ifndef UTILS_H
#define UTILS_H

#include <QObject>

class Utils : public QObject
{
    Q_OBJECT
public:
    enum Operation {
        UnknownOperation = -1,
        GetRunScript,
        StopRunScript,
        ScriptStopped,
        ScriptFinished,
    };
    enum RunMode {
        LocalRunMode,
        RemoteRunMode,
    };

    explicit Utils(QObject *parent = nullptr);

signals:

};

#endif // UTILS_H