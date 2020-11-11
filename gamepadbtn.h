#ifndef GAMEPADBTN_H
#define GAMEPADBTN_H

#include <QPushButton>


class GamepadBtn : public QPushButton
{
    Q_OBJECT

public:
    GamepadBtn(QWidget *parent = Q_NULLPTR);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

signals:
    void sendData(QString data);
};

#endif // GAMEPADBTN_H
