#ifndef GAMEPAD_H
#define GAMEPAD_H

#include <QMainWindow>
#include "gamepadbtn.h"
#include "serialport.h"
#include "scriptengine.h"
#include <QListWidgetItem>
#include "videocapture.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Gamepad; }
QT_END_NAMESPACE

class Gamepad : public QMainWindow
{
    Q_OBJECT

public:
    Gamepad(QWidget *parent = nullptr);
    ~Gamepad();
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private slots:
    void on_serialPortSwitch_clicked();
    void on_serialPortRefresh_clicked();
    void on_scriptList_itemClicked(QListWidgetItem *item);
    void on_scriptEdit_textChanged();
    void on_scriptList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_scriptSave_clicked();
    void on_scriptRefresh_clicked();
    void on_scriptListRefresh_clicked();
    void on_scriptRemove_clicked();
    void on_scriptAdd_clicked();
    void on_scriptRun_clicked();
    void on_choosePath_clicked();
    void on_openPath_clicked();
    void on_addKey_clicked();
    void on_addSleep_clicked();

    void on_videoCaptureSwitch_clicked();

    void on_videoCaptureRefresh_clicked();

private:
    Ui::Gamepad *ui;
    SerialPort serialPort;
    QHash<int, GamepadBtn*> gamepadBtns;
    QHash<QString, QString> buttomDatum;
    ScriptEngine scriptEngine;
    QListWidgetItem *previousScriptListItem = Q_NULLPTR;
    VideoCapture videoCapture;

    void saveScript(QListWidgetItem *item);
};
#endif // GAMEPAD_H
