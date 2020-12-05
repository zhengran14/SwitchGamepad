#include "gamepad.h"
#include "ui_gamepad.h"
#include <QMessageBox>
#include <QDebug>
#include <QKeyEvent>
#include "setting.h"
#include <QDesktopServices>
#include <QFileDialog>

Gamepad::Gamepad(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Gamepad)
    , scriptEngine(&serialPort)
    , videoCapture(this)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);
    ui->serialPortSwitch->setProperty("isOpen", false);
    ui->videoCaptureSwitch->setProperty("isOpen", false);
    ui->scriptRun->setProperty("isStop", false);
    on_serialPortRefresh_clicked();
    ui->splitter->setStretchFactor(1, 2);
    on_scriptListRefresh_clicked();
    ui->pathEdit->setText(Setting::instance()->getScriptPath());
    on_videoCaptureRefresh_clicked();
    videoCapture.init(ui->videoCaptureFrame->layout());
    ui->splitter_2->setStretchFactor(1, 2);
//    connect(&serialPort, &SerialPort::openFailed, this, [this]() {
//        QMessageBox::critical(this, tr("Error"), tr("Failed to open!"), QMessageBox::Ok, QMessageBox::Ok);
//    });
    connect(&scriptEngine, &ScriptEngine::sendData, &serialPort, &SerialPort::sendData);
    connect(&scriptEngine, &ScriptEngine::runScriptFinish, this, [this]() {
        if (ui->scriptRun->property("isStop").toBool()) {
            ui->scriptRun->setText(ui->scriptRun->property("isStop").toBool() ? tr("Run") : tr("Stop"));
            ui->scriptRun->setProperty("isStop", !ui->scriptRun->property("isStop").toBool());
            ui->scriptEdit->setReadOnly(ui->scriptRun->property("isStop").toBool());
        }
    });
    connect(&scriptEngine, &ScriptEngine::hasException, this, [this](QString ex) {
        QMessageBox::critical(this, tr("Error"), ex, QMessageBox::Ok, QMessageBox::Ok);
    });


    {
        ui->lyMinBtn->setProperty("data", "LY MIN");
        ui->lyMaxBtn->setProperty("data", "LY MAX");
        ui->lxMinBtn->setProperty("data", "LX MIN");
        ui->lxMaxBtn->setProperty("data", "LX MAX");
        ui->lBtn->setProperty("data", "Button L");
        ui->zlBtn->setProperty("data", "Button ZL");
        ui->yBtn->setProperty("data", "Button Y");
        ui->bBtn->setProperty("data", "Button B");
        ui->xBtn->setProperty("data", "Button X");
        ui->aBtn->setProperty("data", "Button A");
        ui->rBtn->setProperty("data", "Button R");
        ui->zrBtn->setProperty("data", "Button ZR");
        ui->ryMinBtn->setProperty("data", "RY MIN");
        ui->ryMaxBtn->setProperty("data", "RY MAX");
        ui->rxMinBtn->setProperty("data", "RX MIN");
        ui->rxMaxBtn->setProperty("data", "RX MAX");
        ui->upBtn->setProperty("data", "Button TOP");
        ui->downBtn->setProperty("data", "Button DOWN");
        ui->leftBtn->setProperty("data", "Button LEFT");
        ui->rightBtn->setProperty("data", "Button RIGHT");
        ui->screenshotBtn->setProperty("data", "Button CAPTURE");
        ui->homeBtn->setProperty("data", "Button HOME");
        ui->minBtn->setProperty("data", "Button SELECT");
        ui->plusBtn->setProperty("data", "Button START");
        ui->lcBtn->setProperty("data", "Button LCLICK");
        ui->rcBtn->setProperty("data", "Button RCLICK");

        gamepadBtns.insert(Qt::Key_Z, ui->lyMinBtn);
        gamepadBtns.insert(Qt::Key_X, ui->lyMaxBtn);
        gamepadBtns.insert(Qt::Key_C, ui->lxMinBtn);
        gamepadBtns.insert(Qt::Key_V, ui->lxMaxBtn);
        gamepadBtns.insert(Qt::Key_E, ui->lBtn);
        gamepadBtns.insert(Qt::Key_Q, ui->zlBtn);
        gamepadBtns.insert(Qt::Key_I, ui->yBtn);
        gamepadBtns.insert(Qt::Key_K, ui->bBtn);
        gamepadBtns.insert(Qt::Key_J, ui->xBtn);
        gamepadBtns.insert(Qt::Key_L, ui->aBtn);
        gamepadBtns.insert(Qt::Key_U, ui->rBtn);
        gamepadBtns.insert(Qt::Key_O, ui->zrBtn);
        gamepadBtns.insert(Qt::Key_Up, ui->ryMinBtn);
        gamepadBtns.insert(Qt::Key_Down, ui->ryMaxBtn);
        gamepadBtns.insert(Qt::Key_Left, ui->rxMinBtn);
        gamepadBtns.insert(Qt::Key_Right, ui->rxMaxBtn);
        gamepadBtns.insert(Qt::Key_W, ui->upBtn);
        gamepadBtns.insert(Qt::Key_S, ui->downBtn);
        gamepadBtns.insert(Qt::Key_A, ui->leftBtn);
        gamepadBtns.insert(Qt::Key_D, ui->rightBtn);
        gamepadBtns.insert(Qt::Key_B, ui->screenshotBtn);
        gamepadBtns.insert(Qt::Key_N, ui->homeBtn);
        gamepadBtns.insert(Qt::Key_R, ui->minBtn);
        gamepadBtns.insert(Qt::Key_Y, ui->plusBtn);
        gamepadBtns.insert(Qt::Key_F, ui->lcBtn);
        gamepadBtns.insert(Qt::Key_H, ui->rcBtn);

        connect(ui->lyMinBtn, &GamepadBtn::sendData, &serialPort, &SerialPort::sendData);
        connect(ui->lyMaxBtn, &GamepadBtn::sendData, &serialPort, &SerialPort::sendData);
        connect(ui->lxMinBtn, &GamepadBtn::sendData, &serialPort, &SerialPort::sendData);
        connect(ui->lxMaxBtn, &GamepadBtn::sendData, &serialPort, &SerialPort::sendData);
        connect(ui->lBtn, &GamepadBtn::sendData, &serialPort, &SerialPort::sendData);
        connect(ui->zlBtn, &GamepadBtn::sendData, &serialPort, &SerialPort::sendData);
        connect(ui->yBtn, &GamepadBtn::sendData, &serialPort, &SerialPort::sendData);
        connect(ui->bBtn, &GamepadBtn::sendData, &serialPort, &SerialPort::sendData);
        connect(ui->xBtn, &GamepadBtn::sendData, &serialPort, &SerialPort::sendData);
        connect(ui->aBtn, &GamepadBtn::sendData, &serialPort, &SerialPort::sendData);
        connect(ui->rBtn, &GamepadBtn::sendData, &serialPort, &SerialPort::sendData);
        connect(ui->zrBtn, &GamepadBtn::sendData, &serialPort, &SerialPort::sendData);
        connect(ui->ryMinBtn, &GamepadBtn::sendData, &serialPort, &SerialPort::sendData);
        connect(ui->ryMaxBtn, &GamepadBtn::sendData, &serialPort, &SerialPort::sendData);
        connect(ui->rxMinBtn, &GamepadBtn::sendData, &serialPort, &SerialPort::sendData);
        connect(ui->rxMaxBtn, &GamepadBtn::sendData, &serialPort, &SerialPort::sendData);
        connect(ui->upBtn, &GamepadBtn::sendData, &serialPort, &SerialPort::sendData);
        connect(ui->downBtn, &GamepadBtn::sendData, &serialPort, &SerialPort::sendData);
        connect(ui->leftBtn, &GamepadBtn::sendData, &serialPort, &SerialPort::sendData);
        connect(ui->rightBtn, &GamepadBtn::sendData, &serialPort, &SerialPort::sendData);
        connect(ui->screenshotBtn, &GamepadBtn::sendData, &serialPort, &SerialPort::sendData);
        connect(ui->homeBtn, &GamepadBtn::sendData, &serialPort, &SerialPort::sendData);
        connect(ui->minBtn, &GamepadBtn::sendData, &serialPort, &SerialPort::sendData);
        connect(ui->plusBtn, &GamepadBtn::sendData, &serialPort, &SerialPort::sendData);
        connect(ui->lcBtn, &GamepadBtn::sendData, &serialPort, &SerialPort::sendData);
        connect(ui->rcBtn, &GamepadBtn::sendData, &serialPort, &SerialPort::sendData);
    }
}

Gamepad::~Gamepad()
{
    scriptEngine.deleteLater();
    serialPort.deleteLater();
    videoCapture.deleteLater();
    delete ui;
}

void Gamepad::keyPressEvent(QKeyEvent *event)
{
    if (!ui->keyboardControl->isChecked()) {
        return;
    }
    if (gamepadBtns.contains(event->key())) {
        QMouseEvent mouseEvent(QEvent::MouseButtonPress, QPoint(0, 0), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
        QApplication::sendEvent(gamepadBtns.value(event->key()), &mouseEvent);
    }
}

void Gamepad::keyReleaseEvent(QKeyEvent *event)
{
    if (!ui->keyboardControl->isChecked()) {
        return;
    }
    if (gamepadBtns.contains(event->key())) {
        QMouseEvent mouseEvent(QEvent::MouseButtonRelease, QPoint(0, 0), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
        QApplication::sendEvent(gamepadBtns.value(event->key()), &mouseEvent);
    }
}

void Gamepad::on_serialPortSwitch_clicked()
{
    if (ui->serialPortSwitch->property("isOpen").toBool()) {
        serialPort.close();
    }
    else {
        if (ui->serialPort->count() <= 0 || ui->serialPort->currentText().isEmpty()) {
            QMessageBox::warning(this, tr("Error"), tr("Please choose a serial port."), QMessageBox::Ok, QMessageBox::Ok);
            return;
        }
        if (!serialPort.open(ui->serialPort->currentText(), ui->serialPortBaudRate->currentText().toUInt())) {
            QMessageBox::critical(this, tr("Error"), tr("Failed to open!"), QMessageBox::Ok, QMessageBox::Ok);
            return;
        }
    }
    ui->serialPortSwitch->setText(ui->serialPortSwitch->property("isOpen").toBool() ? tr("Open") : tr("Close"));
    ui->serialPortSwitch->setProperty("isOpen", !ui->serialPortSwitch->property("isOpen").toBool());
    ui->serialPortParamPanel->setEnabled(!ui->serialPortSwitch->property("isOpen").toBool());
}

void Gamepad::on_serialPortRefresh_clicked()
{
    ui->serialPort->setCurrentText("");
    ui->serialPort->clear();
    QString defaultName = "";
    ui->serialPort->addItems(serialPort.getAllPortNames("tty.usbserial", defaultName));
    if (!defaultName.isEmpty()) {
        ui->serialPort->setCurrentText(defaultName);
    }
}

void Gamepad::on_scriptList_itemClicked(QListWidgetItem *item)
{
    if (item == previousScriptListItem) {
        return;
    }
    if (ui->scriptSave->isEnabled() && previousScriptListItem != Q_NULLPTR) {
        QMessageBox saveMessageBox(QMessageBox::Warning, tr("Warning"),
                                   tr("Do you want to save the changes you made to \"") + previousScriptListItem->text() + tr("\"?"),
                                   QMessageBox::No | QMessageBox::Save | QMessageBox::Cancel, this);
        saveMessageBox.setButtonText(QMessageBox::No, tr("Don't Save"));
        int ret = saveMessageBox.exec();
        if (ret == QMessageBox::Cancel) {
            ui->scriptList->blockSignals(true);
            ui->scriptList->setCurrentItem(previousScriptListItem);
            ui->scriptList->blockSignals(false);
            return;
        }
        else if (ret == QMessageBox::Save) {
            saveScript(previousScriptListItem);
        }
    }

    ui->scriptEdit->setPlainText(scriptEngine.getScript(item->text()));
    ui->scriptEdit->setReadOnly(false);
    ui->scriptSave->setEnabled(false);
    ui->scriptRun->setEnabled(true);
    this->repaint();
}

void Gamepad::on_scriptEdit_textChanged()
{
    ui->scriptSave->setEnabled(true);
}

void Gamepad::on_scriptList_currentItemChanged(QListWidgetItem */*current*/, QListWidgetItem *previous)
{
    previousScriptListItem = previous;
}

void Gamepad::on_scriptSave_clicked()
{
    saveScript(ui->scriptList->currentItem());
}

void Gamepad::on_scriptRefresh_clicked()
{
    if (ui->scriptList->currentItem() == Q_NULLPTR) {
        return;
    }
    if (ui->scriptSave->isEnabled()) {
        QMessageBox saveMessageBox(QMessageBox::Warning, tr("Warning"),
                                   tr("Do you want to save the changes you made to \"") + ui->scriptList->currentItem()->text() + tr("\"?"),
                                   QMessageBox::No | QMessageBox::Save | QMessageBox::Cancel, this);
        saveMessageBox.setButtonText(QMessageBox::No, tr("Don't Save"));
        int ret = saveMessageBox.exec();
        if (ret == QMessageBox::Cancel) {
            return;
        }
        else if (ret == QMessageBox::Save) {
            saveScript(ui->scriptList->currentItem());
        }
    }
    ui->scriptEdit->setPlainText(scriptEngine.getScript(ui->scriptList->currentItem()->text()));
    ui->scriptEdit->setReadOnly(false);
    ui->scriptSave->setEnabled(false);
    ui->scriptRun->setEnabled(true);
    this->repaint();
}

void Gamepad::on_scriptListRefresh_clicked()
{
    if (ui->scriptSave->isEnabled()) {
        QMessageBox saveMessageBox(QMessageBox::Warning, tr("Warning"),
                                   tr("Do you want to save the changes you made to \"") + ui->scriptList->currentItem()->text() + tr("\"?"),
                                   QMessageBox::No | QMessageBox::Save | QMessageBox::Cancel, this);
        saveMessageBox.setButtonText(QMessageBox::No, tr("Don't Save"));
        int ret = saveMessageBox.exec();
        if (ret == QMessageBox::Cancel) {
            return;
        }
        else if (ret == QMessageBox::Save) {
            saveScript(ui->scriptList->currentItem());
        }
    }

    ui->scriptEdit->setPlainText("");
    ui->scriptEdit->setReadOnly(true);
    ui->scriptSave->setEnabled(false);
    ui->scriptRun->setEnabled(false);
    previousScriptListItem = Q_NULLPTR;
    ui->scriptList->clear();
    for(QString script : scriptEngine.getAllScripts()) {
        QListWidgetItem *listWidgetItem = new QListWidgetItem(script.split(".")[0], ui->scriptList);
        listWidgetItem->setFlags(listWidgetItem->flags() | Qt::ItemIsEditable);
        ui->scriptList->addItem(listWidgetItem);
    }
    ui->scriptList->repaint();
    this->repaint();
}

void Gamepad::on_scriptRemove_clicked()
{
    if (ui->scriptList->currentItem() == Q_NULLPTR) {
        return;
    }

    if (ui->scriptSave->isEnabled()) {
        QMessageBox saveMessageBox(QMessageBox::Warning, tr("Warning"),
                                   tr("Do you want to save the changes you made to \"") + ui->scriptList->currentItem()->text() + tr("\"?"),
                                   QMessageBox::No | QMessageBox::Save | QMessageBox::Cancel, this);
        saveMessageBox.setButtonText(QMessageBox::No, tr("Don't Save"));
        int ret = saveMessageBox.exec();
        if (ret == QMessageBox::Cancel) {
            return;
        }
        else if (ret == QMessageBox::Save) {
            saveScript(ui->scriptList->currentItem());
        }
    }

    QMessageBox saveMessageBox(QMessageBox::Warning, tr("Warning"),
                               tr("Do you want to delete \"") + ui->scriptList->currentItem()->text() + tr("\"?"),
                               QMessageBox::No | QMessageBox::Yes, this);
    if (saveMessageBox.exec() == QMessageBox::Yes) {
        scriptEngine.removeScript(ui->scriptList->currentItem()->text());
        on_scriptListRefresh_clicked();
    }
}

void Gamepad::on_scriptAdd_clicked()
{
    if (ui->scriptSave->isEnabled()) {
        QMessageBox saveMessageBox(QMessageBox::Warning, tr("Warning"),
                                   tr("Do you want to save the changes you made to \"") + ui->scriptList->currentItem()->text() + tr("\"?"),
                                   QMessageBox::No | QMessageBox::Save | QMessageBox::Cancel, this);
        saveMessageBox.setButtonText(QMessageBox::No, tr("Don't Save"));
        int ret = saveMessageBox.exec();
        if (ret == QMessageBox::Cancel) {
            return;
        }
        else if (ret == QMessageBox::Save) {
            saveScript(ui->scriptList->currentItem());
        }
    }

    QListWidgetItem *listWidgetItem = new QListWidgetItem(scriptEngine.addNewScript());
    listWidgetItem->setFlags(listWidgetItem->flags() | Qt::ItemIsEditable);
    ui->scriptList->insertItem(0, listWidgetItem);
    ui->scriptList->setCurrentItem(listWidgetItem);
    ui->scriptEdit->setReadOnly(false);
    ui->scriptList->setFocus();
    ui->scriptList->repaint();
    this->repaint();
}

void Gamepad::saveScript(QListWidgetItem *item)
{
    if (item == Q_NULLPTR) {
        return;
    }
    scriptEngine.setScript(item->text(), ui->scriptEdit->toPlainText());
    ui->scriptSave->setEnabled(false);
    ui->scriptSave->repaint();
}

void Gamepad::on_scriptRun_clicked()
{
    if (ui->scriptRun->property("isStop").toBool()) {
        scriptEngine.stopScript();
    }
    else {
        scriptEngine.runScript(ui->scriptEdit->toPlainText());
    }
    ui->scriptRun->setText(ui->scriptRun->property("isStop").toBool() ? tr("Run") : tr("Stop"));
    ui->scriptRun->setProperty("isStop", !ui->scriptRun->property("isStop").toBool());
    ui->scriptEdit->setReadOnly(ui->scriptRun->property("isStop").toBool());
}

void Gamepad::on_choosePath_clicked()
{
    QString newPath = QFileDialog::getExistingDirectory(this, tr("Choose Directory"), Setting::instance()->getScriptPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!newPath.isEmpty()) {
        QString oldPath = Setting::instance()->getScriptPath();
        if (newPath.contains(oldPath)) {
            QMessageBox::warning(this, tr("Warning"), tr("Can't choose this directory."), QMessageBox::Ok);
            return;
        }
        newPath += "/scripts";
        QDir dir;
        dir.rename(oldPath, newPath);
        Setting::instance()->setScriptPath(newPath);
        ui->pathEdit->setText(newPath);
    }
}

void Gamepad::on_openPath_clicked()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(ui->pathEdit->text()));
}

void Gamepad::on_addKey_clicked()
{
    if (ui->scriptRun->isEnabled()) {
        ui->scriptEdit->moveCursor(QTextCursor::EndOfLine);
        if (ui->scriptEdit->textCursor().columnNumber() > 0) {
            ui->scriptEdit->insertPlainText("\n");
        }
        ui->scriptEdit->insertPlainText("gp.pressButton(\'" + ui->keyName->currentText() + "\'," + QString::number(ui->keySleepSec->value()) + ");");
        ui->scriptEdit->setFocus();
        ui->scriptEdit->repaint();
    }
}

void Gamepad::on_addSleep_clicked()
{
    if (ui->scriptRun->isEnabled()) {
        ui->scriptEdit->moveCursor(QTextCursor::EndOfLine);
        if (ui->scriptEdit->textCursor().columnNumber() > 0) {
            ui->scriptEdit->insertPlainText("\n");
        }
        ui->scriptEdit->insertPlainText("gp.sleep(" + QString::number(ui->sleepSec->value()) + ");");
        ui->scriptEdit->setFocus();
        ui->scriptEdit->repaint();
    }
}

void Gamepad::on_videoCaptureSwitch_clicked()
{
    if (ui->videoCaptureSwitch->property("isOpen").toBool()) {
        videoCapture.close();
    }
    else {
        if (ui->videoCaptureList->count() <= 0 || ui->videoCaptureList->currentText().isEmpty()) {
            QMessageBox::warning(this, tr("Error"), tr("Please choose a video capture."), QMessageBox::Ok, QMessageBox::Ok);
            return;
        }
//        if (!serialPort.open(ui->serialPort->currentText(), ui->serialPortBaudRate->currentText().toUInt())) {
//            QMessageBox::critical(this, tr("Error"), tr("Failed to open!"), QMessageBox::Ok, QMessageBox::Ok);
//            return;
//        }
        videoCapture.open(ui->videoCaptureList->currentIndex());
    }
    ui->videoCaptureSwitch->setText(ui->videoCaptureSwitch->property("isOpen").toBool() ? tr("Open") : tr("Close"));
    ui->videoCaptureSwitch->setProperty("isOpen", !ui->videoCaptureSwitch->property("isOpen").toBool());
    ui->videoCaptureParamPanel->setEnabled(!ui->videoCaptureSwitch->property("isOpen").toBool());
}

void Gamepad::on_videoCaptureRefresh_clicked()
{
    ui->videoCaptureList->setCurrentText("");
    ui->videoCaptureList->clear();
    QString defaultName = "";
    ui->videoCaptureList->addItems(videoCapture.refresh("USB Video", defaultName));
    if (!defaultName.isEmpty()) {
        ui->videoCaptureList->setCurrentText(defaultName);
    }
}
