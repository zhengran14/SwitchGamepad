#include "gamepad.h"
#include "ui_gamepad.h"
#include <QMessageBox>
#include <QDebug>
#include <QKeyEvent>
#include "setting.h"
#include <QDesktopServices>
#include <QFileDialog>
#include <QTimer>
#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"
#include <QPermission>

Gamepad::Gamepad(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Gamepad)
    , scriptEngine(&serialPort)
    , videoCapture(this)
    , miniTool(this)
    , player(this)
    , server(this)
    , client(this)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);
    ui->serialPortSwitch->setProperty("isOpen", false);
    ui->videoCaptureSwitch->setProperty("isOpen", false);
    ui->serverSwitch->setProperty("isOpen", false);
    ui->clientSwitch->setProperty("isOpen", false);
    ui->scriptRun->setProperty("isStop", false);
    on_serialPortRefresh_clicked();
    ui->splitter->setStretchFactor(0, 1);
    ui->splitter->setStretchFactor(1, 3);
    on_scriptListRefresh_clicked();
    ui->pathEdit->setText(Setting::instance()->getScriptPath());
    ui->tessdataPathEdit->setText(Setting::instance()->getTessdataPath());
    on_videoCaptureRefresh_clicked();
//    videoCapture.init(ui->videoCaptureFrame->layout());
//    ui->splitter_2->setStretchFactor(1, 2);
//    ui->splitter_2->setStretchFactor(0, 1);
    QList<int> list;
    list << 100 << 600;
    ui->splitter_2->setSizes(list);
    ui->remotePort->setValue(Setting::instance()->getRemotePort());
    ui->serverUrl->setText(Setting::instance()->getServerUrl());
    ui->serverPort->setValue(Setting::instance()->getServerPort());
    ui->liveUrl->setText(Setting::instance()->getLiveUrl());
    ui->scriptEdit->installEventFilter(this);
//    connect(&serialPort, &SerialPort::openFailed, this, [this]() {
//        QMessageBox::critical(this, tr("Error"), tr("Failed to open!"), QMessageBox::Ok, QMessageBox::Ok);
//    });
    connect(&scriptEngine, &ScriptEngine::sendData, &serialPort, &SerialPort::sendData);
    connect(&scriptEngine, &ScriptEngine::runScriptStart, this, [this]() {
        ui->status->setText(tr("Local script running"));
    });
    connect(&scriptEngine, &ScriptEngine::runScriptFinish, this, [this]() {
        ui->status->setText(tr(""));
        if (this->runMode == Utils::LocalRunMode) {
            if (ui->scriptRun->property("isStop").toBool()) {
                switchRunUIStatus();
            }
        }
        else if (this->runMode == Utils::RemoteRunMode) {
            ui->remoteInfo->append(tr("Script Finished"));
            QJsonObject json;
            server.write(json, tr("Script Finished"), Utils::ScriptFinished, false);
        }
    });
    connect(&scriptEngine, &ScriptEngine::hasException, this, [this](QString ex) {
        QMessageBox::critical(this, tr("Error"), ex, QMessageBox::Ok, QMessageBox::Ok);
    });
    connect(&scriptEngine, &ScriptEngine::messageBoxShow, this, [this](QString title, QString content) {
        bool result = (QMessageBox::information(0x0, title, content, QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok);
        scriptEngine.messageBoxReturn(result);
    });
    connect(&scriptEngine, &ScriptEngine::setStatusText, this, [this](QString text) {
        ui->status->setText(text);
    });
    connect(&scriptEngine, &ScriptEngine::needCaptureCamera, this, &Gamepad::captureCamera);
    connect(&scriptEngine, &ScriptEngine::cvShow, this, [/*this*/](QString sourcePath, QString templatePath, cv::Point point) {
        QImage img1(sourcePath);
        QImage img2(templatePath);
        cv::Mat captureFrame = Utils::QImage2cvMat(img1);
        cv::Mat template1 = Utils::QImage2cvMat(img2);
        rectangle(captureFrame, point, cv::Point(point.x + template1.cols, point.y + template1.rows), cv::Scalar(0, 255, 0), 2, 8, 0);
        cv::imshow("", captureFrame);
        cv::waitKey(0);
        cv::destroyAllWindows();
//        cv::waitKey(1);
        template1.release();
        captureFrame.release();
    });
    connect(this, &Gamepad::cameraCaptured, &scriptEngine, &ScriptEngine::cameraCaptured);
    connect(&miniTool, &MiniTool::runScriptClicked, this, &Gamepad::on_scriptRun_clicked);
    connect(&miniTool, &MiniTool::scriptListCurrentIndexChanged, this, [this](int index) {
        ui->scriptList->setCurrentRow(index);
        this->on_scriptList_itemClicked(ui->scriptList->currentItem());
    });
    connect(&server, &Server::clientNewConnectiton, this, &Gamepad::on_server_clientNewConnectiton);
    connect(&server, &Server::clientConnectionClosed, this, [this](QString str) {
        ui->remoteInfo->append(tr("Client connection close") + (str.isEmpty() ? tr("") : (tr(": ") + str)));
        operation(QJsonObject(), Utils::StopRunScript);
    });
    connect(&client, &Client::connectSuccess, this, &Gamepad::on_client_connectSuccess);
    connect(&client, &Client::closeConnect, this, [this](QString str) {
        ui->remoteInfo->append(tr("Connect close") + (str.isEmpty() ? tr("") : (tr(": ") + str)));
    });
    connect(&client, &Client::connectError, this, [this](QString str) {
        ui->remoteInfo->append(tr("Connect error") + (str.isEmpty() ? tr("") : (tr(": ") + str)));
        if (ui->clientSwitch->property("isOpen").toBool()) {
//            QMouseEvent mouseEvent(QEvent::MouseButtonPress, QPoint(0, 0), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
//            QApplication::sendEvent(ui->clientSwitch, &mouseEvent);
            on_clientSwitch_clicked();
        }
    });
    connect(&server, &Server::receiveData, this, &Gamepad::on_server_receiveData);
    connect(&client, &Client::receiveData, this, &Gamepad::on_client_receiveData);

    // test get status text
    QTcpServer *tcpServer = new QTcpServer(this);
    tcpServer->listen(QHostAddress::Any, 7001);
    connect(tcpServer, &QTcpServer::newConnection, this, [this, tcpServer]() {
        QTcpSocket *tcpSocket = tcpServer->nextPendingConnection();
        connect(tcpSocket, &QTcpSocket::disconnected, this, []() {
        });
        connect(tcpSocket, &QTcpSocket::readyRead, this, [this, tcpSocket]() {
            QString contentStr = QString("{\"status\":\"%1\"}").arg(ui->status->text());
            //send msg
            QString str = "HTTP/1.1 200 OK\r\n";
            str.append("Server:nginx\r\n");
            str.append("Content-Type:application/json;charset=UTF-8\r\n");
            str.append("Connection:keep-alive\r\n");
            str.append(QString("Content-Length:%1\r\n\r\n").arg(contentStr.size()));
            str.append(contentStr);
            tcpSocket->write(str.toStdString().c_str());
            tcpSocket->close();
//            tcpSocket->deleteLater();
        });
    });
    connect(this, &Gamepad::destroyed, this, [tcpServer]() {
        tcpServer->close();
        tcpServer->deleteLater();
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
        ui->upBtn->setProperty("data", "HAT TOP");
        ui->downBtn->setProperty("data", "HAT DOWN");
        ui->leftBtn->setProperty("data", "HAT LEFT");
        ui->rightBtn->setProperty("data", "HAT RIGHT");
        ui->screenshotBtn->setProperty("data", "Button CAPTURE");
        ui->homeBtn->setProperty("data", "Button HOME");
        ui->minBtn->setProperty("data", "Button SELECT");
        ui->plusBtn->setProperty("data", "Button START");
        ui->lcBtn->setProperty("data", "Button LCLICK");
        ui->rcBtn->setProperty("data", "Button RCLICK");

        gamepadBtns.insert(Qt::Key_W, ui->lyMinBtn);
        gamepadBtns.insert(Qt::Key_S, ui->lyMaxBtn);
        gamepadBtns.insert(Qt::Key_A, ui->lxMinBtn);
        gamepadBtns.insert(Qt::Key_D, ui->lxMaxBtn);
        gamepadBtns.insert(Qt::Key_Z, ui->upBtn);
        gamepadBtns.insert(Qt::Key_X, ui->downBtn);
        gamepadBtns.insert(Qt::Key_C, ui->leftBtn);
        gamepadBtns.insert(Qt::Key_V, ui->rightBtn);
        gamepadBtns.insert(Qt::Key_E, ui->lBtn);
        gamepadBtns.insert(Qt::Key_Q, ui->zlBtn);
        gamepadBtns.insert(Qt::Key_J, ui->yBtn);
        gamepadBtns.insert(Qt::Key_K, ui->bBtn);
        gamepadBtns.insert(Qt::Key_I, ui->xBtn);
        gamepadBtns.insert(Qt::Key_L, ui->aBtn);
        gamepadBtns.insert(Qt::Key_U, ui->rBtn);
        gamepadBtns.insert(Qt::Key_O, ui->zrBtn);
        gamepadBtns.insert(Qt::Key_Up, ui->ryMinBtn);
        gamepadBtns.insert(Qt::Key_Down, ui->ryMaxBtn);
        gamepadBtns.insert(Qt::Key_Left, ui->rxMinBtn);
        gamepadBtns.insert(Qt::Key_Right, ui->rxMaxBtn);
        gamepadBtns.insert(Qt::Key_B, ui->screenshotBtn);
        gamepadBtns.insert(Qt::Key_N, ui->homeBtn);
        gamepadBtns.insert(Qt::Key_R, ui->minBtn);
        gamepadBtns.insert(Qt::Key_Y, ui->plusBtn);
        gamepadBtns.insert(Qt::Key_F, ui->lcBtn);
        gamepadBtns.insert(Qt::Key_H, ui->rcBtn);

        buttomDatum.insert("ZL", "Button ZL");
        buttomDatum.insert("L", "Button L");
        buttomDatum.insert("ZR", "Button ZR");
        buttomDatum.insert("R", "Button R");
        buttomDatum.insert("-", "Button SELECT");
        buttomDatum.insert("+", "Button START");
        buttomDatum.insert("LY MIN", "LY MIN");
        buttomDatum.insert("LY MAX", "LY MAX");
        buttomDatum.insert("LX MIN", "LX MIN");
        buttomDatum.insert("LX MAX", "LX MAX");
        buttomDatum.insert("LC", "Button LCLICK");
        buttomDatum.insert("A", "Button A");
        buttomDatum.insert("B", "Button B");
        buttomDatum.insert("X", "Button X");
        buttomDatum.insert("Y", "Button Y");
        buttomDatum.insert("Up", "HAT TOP");
        buttomDatum.insert("Down", "HAT DOWN");
        buttomDatum.insert("Left", "HAT LEFT");
        buttomDatum.insert("Right", "HAT RIGHT");
        buttomDatum.insert("RY MIN", "RY MIN");
        buttomDatum.insert("RY MAX", "RY MAX");
        buttomDatum.insert("RX MIN", "RX MIN");
        buttomDatum.insert("RX MAX", "RX MAX");
        buttomDatum.insert("RC", "Button RCLICK");
        buttomDatum.insert("Screenshot", "Button CAPTURE");
        buttomDatum.insert("Home", "Button HOME");

        connect(ui->lyMinBtn, &GamepadBtn::sendData, this, &Gamepad::on_gamepadBtn_sendData);
        connect(ui->lyMaxBtn, &GamepadBtn::sendData, this, &Gamepad::on_gamepadBtn_sendData);
        connect(ui->lxMinBtn, &GamepadBtn::sendData, this, &Gamepad::on_gamepadBtn_sendData);
        connect(ui->lxMaxBtn, &GamepadBtn::sendData, this, &Gamepad::on_gamepadBtn_sendData);
        connect(ui->lBtn, &GamepadBtn::sendData, this, &Gamepad::on_gamepadBtn_sendData);
        connect(ui->zlBtn, &GamepadBtn::sendData, this, &Gamepad::on_gamepadBtn_sendData);
        connect(ui->yBtn, &GamepadBtn::sendData, this, &Gamepad::on_gamepadBtn_sendData);
        connect(ui->bBtn, &GamepadBtn::sendData, this, &Gamepad::on_gamepadBtn_sendData);
        connect(ui->xBtn, &GamepadBtn::sendData, this, &Gamepad::on_gamepadBtn_sendData);
        connect(ui->aBtn, &GamepadBtn::sendData, this, &Gamepad::on_gamepadBtn_sendData);
        connect(ui->rBtn, &GamepadBtn::sendData, this, &Gamepad::on_gamepadBtn_sendData);
        connect(ui->zrBtn, &GamepadBtn::sendData, this, &Gamepad::on_gamepadBtn_sendData);
        connect(ui->ryMinBtn, &GamepadBtn::sendData, this, &Gamepad::on_gamepadBtn_sendData);
        connect(ui->ryMaxBtn, &GamepadBtn::sendData, this, &Gamepad::on_gamepadBtn_sendData);
        connect(ui->rxMinBtn, &GamepadBtn::sendData, this, &Gamepad::on_gamepadBtn_sendData);
        connect(ui->rxMaxBtn, &GamepadBtn::sendData, this, &Gamepad::on_gamepadBtn_sendData);
        connect(ui->upBtn, &GamepadBtn::sendData, this, &Gamepad::on_gamepadBtn_sendData);
        connect(ui->downBtn, &GamepadBtn::sendData, this, &Gamepad::on_gamepadBtn_sendData);
        connect(ui->leftBtn, &GamepadBtn::sendData, this, &Gamepad::on_gamepadBtn_sendData);
        connect(ui->rightBtn, &GamepadBtn::sendData, this, &Gamepad::on_gamepadBtn_sendData);
        connect(ui->screenshotBtn, &GamepadBtn::sendData, this, &Gamepad::on_gamepadBtn_sendData);
        connect(ui->homeBtn, &GamepadBtn::sendData, this, &Gamepad::on_gamepadBtn_sendData);
        connect(ui->minBtn, &GamepadBtn::sendData, this, &Gamepad::on_gamepadBtn_sendData);
        connect(ui->plusBtn, &GamepadBtn::sendData, this, &Gamepad::on_gamepadBtn_sendData);
        connect(ui->lcBtn, &GamepadBtn::sendData, this, &Gamepad::on_gamepadBtn_sendData);
        connect(ui->rcBtn, &GamepadBtn::sendData, this, &Gamepad::on_gamepadBtn_sendData);
    }

    // camera
    QCameraPermission cameraPermission;
    switch (qApp->checkPermission(cameraPermission)) {
    case Qt::PermissionStatus::Undetermined:
        qApp->requestPermission(cameraPermission, [](const QPermission &){});
        break;
    case Qt::PermissionStatus::Denied:
        qWarning("Camera permission is not granted!");
        break;
    case Qt::PermissionStatus::Granted:
        break;
    }
    // microphone
    QMicrophonePermission microphonePermission;
    switch (qApp->checkPermission(microphonePermission)) {
    case Qt::PermissionStatus::Undetermined:
        qApp->requestPermission(microphonePermission, [](const QPermission &){});
        break;
    case Qt::PermissionStatus::Denied:
        qWarning("Microphone permission is not granted!");
        break;
    case Qt::PermissionStatus::Granted:
        break;
    }
}

Gamepad::~Gamepad()
{
    on_miniToolShow_clicked(false);
    miniTool.close();
    miniTool.deleteLater();
    scriptEngine.deleteLater();
    serialPort.deleteLater();
    videoCapture.deleteLater();
    player.deleteLater();
    client.deleteLater();
    server.deleteLater();
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

void Gamepad::closeEvent(QCloseEvent *event)
{
    this->hide();
    event->ignore();
    //    QMainWindow::closeEvent(event);
}

bool Gamepad::eventFilter(QObject *object, QEvent *event)
{
    if (object == ui->scriptEdit && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Tab) {
            ui->scriptEdit->insertPlainText("    ");
            return true;
        }
        else if (keyEvent->key() == Qt::Key_Backspace) {
            QTextCursor cursor = ui->scriptEdit->textCursor();
            int col = cursor.columnNumber();
//            int row = cursor.blockNumber();
            for (int i = 0; i < 4 && col > 0; i++) {
                cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
                col = cursor.columnNumber();
//                row = cursor.blockNumber();
            }
            if (cursor.selectedText() == "    ") {
                ui->scriptEdit->setTextCursor(cursor);
//                return true;
            }
        }
        else if (keyEvent->key() == Qt::Key_Slash && (keyEvent->modifiers() & Qt::ControlModifier)) {
            QTextCursor cursor = ui->scriptEdit->textCursor();
//            int oriCol = cursor.columnNumber();
            int oriRow = cursor.blockNumber();
            cursor.movePosition(QTextCursor::StartOfLine);
            int row = cursor.blockNumber();
            for (int i = 0; i < 2 && row == oriRow; i++) {
                cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
                row = cursor.blockNumber();
            }
            if (row != oriRow) {
                cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
            }
            if (cursor.selectedText() == "//") {
                cursor.insertText("");
            }
            else {
                cursor.movePosition(QTextCursor::StartOfLine);
                cursor.insertText("//");
            }
            return true;
        }
    }
    return false;
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
    ui->serialPortBaudRate->setEnabled(!ui->serialPortSwitch->property("isOpen").toBool());
    ui->serialPort->setEnabled(!ui->serialPortSwitch->property("isOpen").toBool());
    ui->serialPortRefresh->setEnabled(!ui->serialPortSwitch->property("isOpen").toBool());
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

void Gamepad::on_gamepadBtn_sendData(QString data)
{
    if (this->runMode == Utils::LocalRunMode || ui->serverSwitch->property("isOpen").toBool()) {
        serialPort.sendData(data);
    }
    else if (this->runMode == Utils::RemoteRunMode && ui->clientSwitch->property("isOpen").toBool()) {
        ui->remoteInfo->append(tr("Send key: ") + data);
        QJsonObject json;
        json.insert("data", data);
        client.write(json, tr("Receive key: ") + data, Utils::ReceiveKey);
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
        saveMessageBox.addButton(tr("Don't Save"), QMessageBox::NoRole);
        int ret = saveMessageBox.exec();
        if (ret == QMessageBox::Cancel) {
            ui->scriptList->blockSignals(true);
            ui->scriptList->setCurrentItem(previousScriptListItem);
            ui->scriptList->blockSignals(false);
            miniTool.setScriptListCurrentIndex(ui->scriptList->currentRow());
            return;
        }
        else if (ret == QMessageBox::Save) {
            saveScript(previousScriptListItem);
        }
    }

    ui->scriptEdit->setPlainText(scriptEngine.getScript(item->text()));
    miniTool.setScriptListCurrentIndex(ui->scriptList->currentRow());
    ui->scriptEdit->setReadOnly(false);
    ui->scriptSave->setEnabled(false);
    ui->scriptRun->setEnabled(true);
    miniTool.setScriptRunEnabled(ui->scriptRun->isEnabled());
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
        saveMessageBox.addButton(tr("Don't Save"), QMessageBox::NoRole);
        int ret = saveMessageBox.exec();
        if (ret == QMessageBox::Cancel) {
            return;
        }
        else if (ret == QMessageBox::Save) {
            saveScript(ui->scriptList->currentItem());
        }
    }
    ui->scriptEdit->setPlainText(scriptEngine.getScript(ui->scriptList->currentItem()->text()));
    miniTool.setScriptListCurrentIndex(ui->scriptList->currentRow());
    ui->scriptEdit->setReadOnly(false);
    ui->scriptSave->setEnabled(false);
    ui->scriptRun->setEnabled(true);
    miniTool.setScriptRunEnabled(ui->scriptRun->isEnabled());
    this->repaint();
}

void Gamepad::on_scriptListRefresh_clicked()
{
    if (ui->scriptSave->isEnabled()) {
        QMessageBox saveMessageBox(QMessageBox::Warning, tr("Warning"),
                                   tr("Do you want to save the changes you made to \"") + ui->scriptList->currentItem()->text() + tr("\"?"),
                                   QMessageBox::No | QMessageBox::Save | QMessageBox::Cancel, this);
        saveMessageBox.addButton(tr("Don't Save"), QMessageBox::NoRole);
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
    miniTool.setScriptRunEnabled(ui->scriptRun->isEnabled());
    previousScriptListItem = Q_NULLPTR;
    ui->scriptList->clear();
    miniTool.clearScriptList();
    QStringList scriptList = scriptEngine.getAllScripts();
    for(QString script : scriptList) {
        ui->scriptList->blockSignals(true);
        QListWidgetItem *listWidgetItem = new QListWidgetItem(script.split(".")[0], ui->scriptList);
        listWidgetItem->setFlags(listWidgetItem->flags() | Qt::ItemIsEditable);
        ui->scriptList->addItem(listWidgetItem);
        ui->scriptList->blockSignals(false);
        miniTool.AddScript(script);
    }
    miniTool.setScriptListCurrentIndex(-1);
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
        saveMessageBox.addButton(tr("Don't Save"), QMessageBox::NoRole);
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
        saveMessageBox.addButton(tr("Don't Save"), QMessageBox::NoRole);
        int ret = saveMessageBox.exec();
        if (ret == QMessageBox::Cancel) {
            return;
        }
        else if (ret == QMessageBox::Save) {
            saveScript(ui->scriptList->currentItem());
        }
    }

    ui->scriptList->blockSignals(true);
    QListWidgetItem *listWidgetItem = new QListWidgetItem(scriptEngine.addNewScript());
    listWidgetItem->setFlags(listWidgetItem->flags() | Qt::ItemIsEditable);
    ui->scriptList->insertItem(0, listWidgetItem);
    ui->scriptList->blockSignals(false);
    miniTool.InsertScript(0, listWidgetItem->text());
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

void Gamepad::operation(QJsonObject json, Utils::Operation operation)
{
    switch (operation) {
//    case Utils::GetAllScripts: {
//        if (json.contains("files")) {
//            QJsonArray filesJson = json["files"].toArray();
//            scriptEngine.setScriptEngineMode(ScriptEngine::ScriptEngineRemoteMode, filesJson);
//            on_scriptListRefresh_clicked();
//            return;
//        }
//        break;
//    }
    case Utils::GetRunScript: {
        if (json.contains("script")) {
            scriptEngine.runScript(json["script"].toString());
            ui->remoteInfo->append(tr("Start run script..."));
            QJsonObject json;
            server.write(json, tr("Start run script..."), Utils::ShowMessage);
            return;
        }
        break;
    }
    case Utils::RunScriptName: {
        if (json.contains("scriptName")) {
            QString scriptName = json["scriptName"].toString();
            QStringList scriptNames = scriptName.split(".");
            if (scriptNames.count() == 2 && scriptNames[1] == "js") {
                scriptName = scriptNames[0];
            }
            scriptEngine.runScript(scriptEngine.getScript(scriptName));
            ui->remoteInfo->append(tr("Start run script..."));
            QJsonObject json;
            server.write(json, tr("Start run script..."), Utils::ShowMessage);
            return;
        }
        break;
    }
    case Utils::StopRunScript: {
        scriptEngine.stopScript();
        ui->remoteInfo->append(tr("Script stopped"));
        QJsonObject json;
        server.write(json, tr("Script stopped"), Utils::ScriptStopped);
        return;
    }
    case Utils::ScriptStopped: {
        if (ui->scriptRun->property("isStop").toBool()) {
            switchRunUIStatus();
        }
        ui->status->setText(tr(""));
        return;
    }
    case Utils::ScriptFinished: {
        if (ui->scriptRun->property("isStop").toBool()) {
            switchRunUIStatus();
        }
        ui->status->setText(tr(""));
        return;
    }
    case Utils::ReceiveKey: {
        if (json.contains("data")) {
            QString data = json["data"].toString();
            serialPort.sendData(data);
            ui->remoteInfo->append(tr("Run key: ") + data);
            QJsonObject json;
            server.write(json, tr("Run key: ") + data, Utils::ShowMessage);
            return;
        }
        break;
    }
    case Utils::SendAllScriptsName: {
        ui->remoteInfo->append(tr("Send all scripts name"));
        QJsonArray array = QJsonArray::fromStringList(scriptEngine.getAllScripts());
        QJsonObject json;
        json.insert("scripts", array);
        server.write(json, tr("Send all scripts name"), Utils::GetAllScriptsName, false);

        return;
    }
    case Utils::ShowMessage: {
        return;
    }
    case Utils::UnknownOperation:
    default: {
        break;
    }
    }
    ui->remoteInfo->append(tr("Unknown operation"));
}

void Gamepad::switchRunUIStatus()
{
    ui->scriptRun->setText(ui->scriptRun->property("isStop").toBool() ? tr("Run") : tr("Stop"));
    miniTool.setScriptRunText(ui->scriptRun->text());
    ui->scriptRun->setProperty("isStop", !ui->scriptRun->property("isStop").toBool());
    ui->scriptEdit->setReadOnly(ui->scriptRun->property("isStop").toBool());
    ui->scriptList->setEnabled(!ui->scriptRun->property("isStop").toBool());
    miniTool.setScriptListEnabled(ui->scriptList->isEnabled());
}

//void Gamepad::sendAllScripts()
//{
//    ui->remoteInfo->append(tr("Send all scripts"));
//    server.write(scriptEngine.getAllScriptsIntoJson(), tr("Get all scripts"), Utils::GetAllScripts);
//}

void Gamepad::on_scriptRun_clicked()
{
    if (ui->scriptRun->property("isStop").toBool()) {
        if (this->runMode == Utils::LocalRunMode) {
            scriptEngine.stopScript();
        }
        else if (this->runMode == Utils::RemoteRunMode) {
            ui->remoteInfo->append(tr("Stop run script"));
            QJsonObject json;
            client.write(json, tr("Stop run script"), Utils::StopRunScript);
            return;
        }
    }
    else {
        if (this->runMode == Utils::LocalRunMode) {
            scriptEngine.runScript(ui->scriptEdit->toPlainText());
        }
        else if (this->runMode == Utils::RemoteRunMode) {
            ui->remoteInfo->append(tr("Send run script"));
            QJsonObject json;
            json.insert("script", ui->scriptEdit->toPlainText());
            client.write(json, tr("Get run script"), Utils::GetRunScript);
            ui->status->setText(tr("Remote script running"));
        }
    }
    switchRunUIStatus();
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

void Gamepad::on_chooseTessdataPath_clicked()
{
    QString newPath = QFileDialog::getExistingDirectory(this, tr("Choose Directory"), Setting::instance()->getTessdataPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!newPath.isEmpty()) {
        QString oldPath = Setting::instance()->getTessdataPath();
        if (newPath.contains(oldPath)) {
            QMessageBox::warning(this, tr("Warning"), tr("Can't choose this directory."), QMessageBox::Ok);
            return;
        }
        newPath += "/tessdata";
        QDir dir;
        dir.rename(oldPath, newPath);
        Setting::instance()->setTessdataPath(newPath);
        ui->tessdataPathEdit->setText(newPath);
    }
}

void Gamepad::on_openTessdataPath_clicked()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(ui->tessdataPathEdit->text()));
}

void Gamepad::on_addKey_clicked()
{
    if (ui->scriptRun->isEnabled()) {
        ui->scriptEdit->moveCursor(QTextCursor::EndOfLine);
        if (ui->scriptEdit->textCursor().columnNumber() > 0) {
            ui->scriptEdit->insertPlainText("\n");
        }
        ui->scriptEdit->insertPlainText("gp.pressButton(\'" + buttomDatum.value(ui->keyName->currentText()) + "\'," + QString::number(ui->keySleepSec->value()) + ");");
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
    on_hideVideoCapture_clicked(false);
    ui->hideVideoCapture->setChecked(false);
    player.stop();
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
        on_miniToolShow_clicked(!ui->miniToolShow->isChecked());
        videoCapture.open(ui->videoCaptureList->currentIndex(),
                          ui->videoCaptureCameraFormat->currentIndex(),
                          ui->audioCaptureList->currentIndex());
        videoCapture.setAudioInputVolume(ui->audioCaptureVolume->value());
        videoCapture.setAudioInputMute(ui->audioCaptureMute->isChecked());
        on_miniToolShow_clicked(ui->miniToolShow->isChecked());
    }
    ui->videoCaptureSwitch->setText(ui->videoCaptureSwitch->property("isOpen").toBool() ? tr("Open") : tr("Close"));
    ui->videoCaptureSwitch->setProperty("isOpen", !ui->videoCaptureSwitch->property("isOpen").toBool());
    ui->videoCaptureCameraFormat->setEnabled(!ui->videoCaptureSwitch->property("isOpen").toBool());
    ui->videoCaptureList->setEnabled(!ui->videoCaptureSwitch->property("isOpen").toBool());
    ui->videoCaptureRefresh->setEnabled(!ui->videoCaptureSwitch->property("isOpen").toBool());
    ui->audioCaptureList->setEnabled(!ui->videoCaptureSwitch->property("isOpen").toBool());
    ui->audioCaptureRefresh->setEnabled(!ui->videoCaptureSwitch->property("isOpen").toBool());
}

void Gamepad::on_videoCaptureRefresh_clicked()
{
    ui->videoCaptureList->setCurrentText("");
    ui->videoCaptureList->clear();
    QString defaultName = "";
    ui->videoCaptureList->addItems(videoCapture.refreshCamera("USB Video", defaultName));
    if (!defaultName.isEmpty()) {
        ui->videoCaptureList->setCurrentText(defaultName);
    }
    if (ui->videoCaptureList->count() > 0 && ui->videoCaptureList->currentIndex() >= 0) {
        on_videoCaptureList_activated(ui->videoCaptureList->currentIndex());
    }

    ui->audioCaptureList->setCurrentText("");
    ui->audioCaptureList->clear();
    defaultName = "";
    ui->audioCaptureList->addItems(videoCapture.refreshAudioInput("USB Digital Audio", defaultName));
    if (!defaultName.isEmpty()) {
        ui->audioCaptureList->setCurrentText(defaultName);
    }
}

void Gamepad::on_videoCaptureList_activated(int index)
{
    ui->videoCaptureCameraFormat->setCurrentText("");
    ui->videoCaptureCameraFormat->clear();
    int defaultIndex = -1;
    QStringList defaultSearch;
    defaultSearch << "1280x720" << "30";
    ui->videoCaptureCameraFormat->addItems(videoCapture.GetCameraFormats(index, defaultIndex, defaultSearch));
    if (defaultIndex >= 0) {
        ui->videoCaptureCameraFormat->setCurrentIndex(defaultIndex);
    }
    else if (ui->videoCaptureCameraFormat->count() > 0) {
        ui->videoCaptureCameraFormat->setCurrentIndex(ui->videoCaptureCameraFormat->count() - 1);
    }
}

void Gamepad::on_miniToolShow_clicked(bool checked)
{
    if (checked) {
        miniTool.show();
        if (ui->clientSwitch->property("isOpen").toBool()) {
            videoCapture.removeViewfinder();
            player.play(ui->liveUrl->text(), miniTool.GetVideoCaptionFrameLayout());
        }
        else {
            player.stop();
            videoCapture.moveViewfinder(miniTool.GetVideoCaptionFrameLayout());
        }
        miniTool.adjustVideoCaptionFrameLayout();
    }
    else {
        miniTool.hide();
        if (ui->clientSwitch->property("isOpen").toBool()) {
            videoCapture.removeViewfinder();
            player.play(ui->liveUrl->text(), ui->videoCaptureFrame->layout());
        }
        else {
            player.stop();
            videoCapture.moveViewfinder(ui->videoCaptureFrame->layout());
        }
//        ui->videoCaptureFrame->repaint();
    }
}

void Gamepad::on_scriptList_itemChanged(QListWidgetItem *item)
{
    if (!this->doubliClickScriptListItemText.isEmpty()) {
        if (!scriptEngine.renameScript(this->doubliClickScriptListItemText, item->text())) {
            ui->scriptList->blockSignals(true);
            item->setText(this->doubliClickScriptListItemText);
            ui->scriptList->blockSignals(false);
        }
        else {
            miniTool.setScriptName(ui->scriptList->currentRow(), item->text());
        }

        this->doubliClickScriptListItemText = "";
    }
}

void Gamepad::on_scriptList_itemDoubleClicked(QListWidgetItem *item)
{
    doubliClickScriptListItemText = item->text();
}

void Gamepad::on_quit_clicked()
{
    QApplication::quit();
}

void Gamepad::on_serverSwitch_clicked()
{
    this->runMode = Utils::LocalRunMode;
    if (ui->serverSwitch->property("isOpen").toBool()) {
        ui->remoteInfo->append(tr("Close server"));
        operation(QJsonObject(), Utils::StopRunScript);
        server.close();
    }
    else {
        ui->remoteInfo->clear();
        ui->remoteInfo->append(tr("Open port: ") + QString::number(ui->remotePort->value()));
        if (!server.open(ui->remotePort->value())) {
            server.close();
            return;
        }
        if (ui->scriptRun->property("isStop").toBool()) {
            on_scriptRun_clicked();
        }
        ui->remoteInfo->append(tr("Listening..."));
        this->runMode = Utils::RemoteRunMode;
    }
    ui->serverSwitch->setText(ui->serverSwitch->property("isOpen").toBool() ? tr("Open") : tr("Close"));
    ui->serverSwitch->setProperty("isOpen", !ui->serverSwitch->property("isOpen").toBool());
    ui->clientGroup->setEnabled(!ui->serverSwitch->property("isOpen").toBool());
    ui->serverInfo->setEnabled(!ui->serverSwitch->property("isOpen").toBool());
    ui->scriptTab->setEnabled(!ui->serverSwitch->property("isOpen").toBool());
}

void Gamepad::on_clientSwitch_clicked()
{
    if (ui->videoCaptureSwitch->property("isOpen").toBool()) {
        on_videoCaptureSwitch_clicked();
    }
    videoCapture.close();
    videoCapture.removeViewfinder();
    this->runMode = Utils::LocalRunMode;
    if (ui->scriptRun->property("isStop").toBool()) {
        on_scriptRun_clicked();
    }
    ui->clientSwitch->setText(ui->clientSwitch->property("isOpen").toBool() ? tr("Open") : tr("Close"));
    ui->clientSwitch->setProperty("isOpen", !ui->clientSwitch->property("isOpen").toBool());
    ui->serverGroup->setEnabled(!ui->clientSwitch->property("isOpen").toBool());
    ui->clientInfo->setEnabled(!ui->clientSwitch->property("isOpen").toBool());
    ui->videoCapturePanel->setEnabled(!ui->clientSwitch->property("isOpen").toBool());
    if (!ui->clientSwitch->property("isOpen").toBool()) {
        ui->remoteInfo->append(tr("Close connect"));
        client.disConnectServer();
        player.stop();
    }
    else {
        ui->remoteInfo->clear();
        ui->remoteInfo->append(tr("Connecting ") + ui->serverUrl->text() + tr(":") + QString::number(ui->serverPort->value()));
        client.connectServer(ui->serverUrl->text(), ui->serverPort->value());
        on_miniToolShow_clicked(ui->miniToolShow->isChecked());
    }
}

void Gamepad::on_remotePort_editingFinished()
{
    Setting::instance()->setRemotePort(ui->remotePort->value());
}

void Gamepad::on_serverUrl_editingFinished()
{
    Setting::instance()->setServerUrl(ui->serverUrl->text());
}

void Gamepad::on_serverPort_editingFinished()
{
    Setting::instance()->setServerPort(ui->serverPort->value());
}

void Gamepad::on_liveUrl_editingFinished()
{
    Setting::instance()->setLiveUrl(ui->liveUrl->text());
}

void Gamepad::on_remoteInfo_cursorPositionChanged()
{
//    QTextCursor cursor =  ui->remoteInfo->textCursor();
//    cursor.movePosition(QTextCursor::End);
    //    ui->remoteInfo->setTextCursor(cursor);
}

void Gamepad::on_server_receiveData()
{
    QJsonObject json;
    QString message = "";
    Utils::Operation operation = Utils::UnknownOperation;
    bool b = server.read(json, message, operation);
    ui->remoteInfo->append(b ? (tr("Receive: ") + message) : tr("Receive failed"));
    if (b) {
        this->operation(json, operation);
    }
}

void Gamepad::on_client_receiveData()
{
    QJsonObject json;
    QString message = "";
    Utils::Operation operation = Utils::UnknownOperation;
    bool b = client.read(json, message, operation);
    ui->remoteInfo->append(b ? (tr("Receive: ") + message) : tr("Receive failed"));
    if (b) {
        this->operation(json, operation);
    }
}

void Gamepad::on_server_clientNewConnectiton(QString str)
{
    ui->remoteInfo->append(tr("New client connected") + (str.isEmpty() ? tr("") : (tr(": ") + str)));
//    sendAllScripts();
}

void Gamepad::on_client_connectSuccess(QString str)
{
    ui->remoteInfo->append(tr("Connect success") + (str.isEmpty() ? tr("") : (tr(": ") + str)));
    this->runMode = Utils::RemoteRunMode;
}

void Gamepad::on_remoteInfoClear_clicked()
{
    ui->remoteInfo->clear();
}

void Gamepad::captureCamera()
{
    QImage *videoFrame = this->videoCapture.capture();
    emit cameraCaptured(videoFrame);
}

void Gamepad::on_hideVideoCapture_clicked(bool checked)
{
//    ui->videoCaptureFrame->setVisible(!checked);
//    miniTool.GetVideoCaptionFrameLayout()->parentWidget()->setVisible(!checked);
    ui->hideVideoCapture->setEnabled(false);
    videoCapture.stopViewfinder(checked);
    QTimer::singleShot(1 * 1000, this, [this]() {
        ui->hideVideoCapture->setEnabled(true);
    });
}


void Gamepad::on_audioCaptureVolume_valueChanged(int value)
{
    videoCapture.setAudioInputVolume(value);
}


void Gamepad::on_audioCaptureMute_clicked(bool checked)
{
    videoCapture.setAudioInputMute(checked);
}

