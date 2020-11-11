#include "gamepadbtn.h"
#include <QVariant>

GamepadBtn::GamepadBtn(QWidget *parent)
    : QPushButton(parent)
{
}

void GamepadBtn::mousePressEvent(QMouseEvent *e)
{
    emit sendData(this->property("data").toString());
    QPushButton::mousePressEvent(e);
}

void GamepadBtn::mouseReleaseEvent(QMouseEvent *e)
{
    emit sendData("RELEASE");
    QPushButton::mouseReleaseEvent(e);
}
