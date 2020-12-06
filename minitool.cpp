#include "minitool.h"
#include "ui_minitool.h"
#include <QWindow>
#include <QMouseEvent>
#include <QDebug>

MiniTool::MiniTool(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MiniTool)
{
    this->setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    ui->setupUi(this);

    this->installEventFilter(this);
}

MiniTool::~MiniTool()
{
    delete ui;
}

void MiniTool::clearScriptList()
{
    ui->scriptList->blockSignals(true);
    ui->scriptList->clear();
    ui->scriptList->blockSignals(false);
}

void MiniTool::AddScriptList(QStringList scripts)
{
    ui->scriptList->blockSignals(true);
    for(QString script : scripts) {
        ui->scriptList->addItem(script.split(".")[0]);
    }
    ui->scriptList->blockSignals(false);
}

void MiniTool::AddScript(QString script)
{
    ui->scriptList->blockSignals(true);
    ui->scriptList->addItem(script.split(".")[0]);
    ui->scriptList->blockSignals(false);
}

void MiniTool::InsertScript(int index, QString script)
{
    ui->scriptList->blockSignals(true);
    ui->scriptList->insertItem(index, script.split(".")[0]);
    ui->scriptList->blockSignals(true);
}

void MiniTool::RefreshScriptList(QStringList scripts)
{
    clearScriptList();
    AddScriptList(scripts);
}

QLayout *MiniTool::GetVIdeoCaptionFrameLayout()
{
    return ui->videoCaptureFrame->layout();
}

void MiniTool::setScriptRunText(QString text)
{
    ui->runScript->setText(text);
}

void MiniTool::setScriptName(int index, QString name)
{
    ui->scriptList->setItemText(index, name);
}

void MiniTool::setScriptListCurrentIndex(int index)
{
    ui->scriptList->blockSignals(true);
    ui->scriptList->setCurrentIndex(index);
    ui->scriptList->blockSignals(false);
}

void MiniTool::setScriptRunEnabled(bool b)
{
    ui->runScript->setEnabled(b);
}

bool MiniTool::eventFilter(QObject *target, QEvent *event)
{
    switch (event->type()) {
    case QEvent::MouseButtonPress: {
        if (!ui->resize->isChecked()) {
            this->windowHandle()->startSystemMove();
            return true;
        }
    }
//    case QEvent::MouseMove: {
//        const int border = 5;
//        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
//        Qt::Edges edge = 0;
//        if (mouseEvent->pos().x() < border) edge |= Qt::LeftEdge;
//        if (mouseEvent->pos().x() >= this->width() - border) edge |= Qt::RightEdge;
//        if (mouseEvent->pos().y() < border) edge |= Qt::TopEdge;
//        if (mouseEvent->pos().y() >= this->height() - border) edge |= Qt::BottomEdge;
//        if (edge == 0) {
////            this->windowHandle()->startSystemMove();
//        }
//        else {
//            qDebug() << this->windowHandle()->startSystemResize(edge);
//        }
//        return true;
//    }
    }

    return QWidget::eventFilter(target, event);
}

void MiniTool::on_resize_clicked(bool checked)
{
    if (checked) {
        this->setWindowFlags(this->windowFlags() & ~Qt::FramelessWindowHint);
    }
    else {
        this->setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint);
    }
    this->show();
}

void MiniTool::on_runScript_clicked()
{
    emit runScriptClicked();
}

void MiniTool::on_scriptList_currentIndexChanged(int index)
{
    emit scriptListCurrentIndexChanged(index);
}

void MiniTool::on_showMainWindow_clicked()
{
    ((QMainWindow*)this->parent())->show();
}
