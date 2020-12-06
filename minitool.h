#ifndef MINITOOL_H
#define MINITOOL_H

#include <QMainWindow>

namespace Ui {
class MiniTool;
}

class MiniTool : public QMainWindow
{
    Q_OBJECT

public:
    explicit MiniTool(QWidget *parent = nullptr);
    ~MiniTool();
    void clearScriptList();
    void AddScriptList(QStringList scripts);
    void AddScript(QString script);
    void InsertScript(int index, QString script);
    void RefreshScriptList(QStringList scripts);
    QLayout *GetVIdeoCaptionFrameLayout();
    void setScriptRunEnabled(bool b);
    void setScriptRunText(QString text);
    void setScriptName(int index, QString name);
    void setScriptListCurrentIndex(int index);

protected:
    bool eventFilter(QObject *target, QEvent *event);

private slots:
    void on_resize_clicked(bool checked);

    void on_runScript_clicked();

    void on_scriptList_currentIndexChanged(int index);

    void on_showMainWindow_clicked();

private:
    Ui::MiniTool *ui;

signals:
    void runScriptClicked();
    void scriptListCurrentIndexChanged(int index);
};

#endif // MINITOOL_H
