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

    ui->centralwidget->installEventFilter(this);
    ui->videoCaptureFrame->installEventFilter(this);
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

QLayout *MiniTool::GetVideoCaptionFrameLayout()
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

void MiniTool::adjustVideoCaptionFrameLayout()
{
    ui->videoCaptureFrame->setGeometry(0, 0, ui->videoCaptureFrameParent->width(), ui->videoCaptureFrameParent->height());
    ui->zoom->move(ui->videoCaptureFrameParent->width() - ui->zoom->width(), ui->videoCaptureFrameParent->height() - ui->zoom->height());
}

void MiniTool::setScriptListEnabled(bool b)
{
    ui->scriptList->setEnabled(b);
}

void MiniTool::setScriptRunEnabled(bool b)
{
    ui->runScript->setEnabled(b);
}

bool MiniTool::eventFilter(QObject *target, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        if (target != ui->videoCaptureFrame) {
            if (!ui->resize->isChecked()) {
                this->windowHandle()->startSystemMove();
            }
        }
        else {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            lastPoint = mouseEvent->pos();
        }
        return true;
    }
    else if (event->type() == QEvent::MouseMove) {
        if (target == ui->videoCaptureFrame) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (lastPoint.x() != 0 && lastPoint.y() != 0) {
                QPoint p = mouseEvent->pos() - lastPoint;
                ui->videoCaptureFrame->move(ui->videoCaptureFrame->pos() + p);
            }
            else {
                lastPoint = mouseEvent->pos();
            }
            return true;
        }
    }
//    switch (event->type()) {
//    case QEvent::MouseMove: {
//        const int border = 5;
//        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
//        Qt::Edges edge = 0;
//        if (mouseEvent->pos().x() < border) edge |= Qt::LeftEdge;
//        if (mouseEvent->pos().x() >= this->width() - border) edge |= Qt::RightEdge;
//        if (mouseEvent->pos().y() < border) edge |= Qt::TopEdge;
//        if (mouseEvent->pos().y() >= this->height() - border) edge |= Qt::BottomEdge;
//        if (edge == 0) {
//            this->windowHandle()->startSystemMove();
//        }
//        else {
//            qDebug() << this->windowHandle()->startSystemResize(edge);
//        }
//        return true;
//    }
//    }

    return QWidget::eventFilter(target, event);
}

void MiniTool::resizeEvent(QResizeEvent *event)
{
    adjustVideoCaptionFrameLayout();
    QWidget::resizeEvent(event);
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

void MiniTool::on_zoomIn_clicked()
{
    QRect rc = ui->videoCaptureFrame->geometry();
    ui->videoCaptureFrame->setGeometry(rc.x() - 20, rc.y() - 20, rc.width() + 40, rc.height() + 40);
}

void MiniTool::on_zoomOut_clicked()
{
    QRect rc = ui->videoCaptureFrame->geometry();
    rc.setWidth(rc.width() - 40);
    rc.setHeight(rc.height() - 40);
    rc.moveTo(rc.x() + 20, rc.y() + 20);
    if (rc.width() < ui->videoCaptureFrameParent->width() || rc.height() < ui->videoCaptureFrameParent->height()) {
        ui->videoCaptureFrame->setGeometry(0, 0, ui->videoCaptureFrameParent->width(), ui->videoCaptureFrameParent->height());
    }
    else {
        ui->videoCaptureFrame->setGeometry(rc);
    }
}

void MiniTool::on_restore_clicked()
{
    adjustVideoCaptionFrameLayout();
}
