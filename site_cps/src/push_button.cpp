/*!
 * \file    push_button.cpp
 * \date    2018.10.24
 * \author  zhusizhi
 * \version 1.0.0
 *
 * \brief
 *
 */
#include "push_button.h"


namespace site_cps
{

PushButton::PushButton(QWidget *parent)
    :QPushButton(parent)
{
    status = NORMALSTU;
    mouse_press = false;
}

PushButton::~PushButton()
{

}

void PushButton::setPicName(QString pic_name)
{
    this->pic_name = pic_name;
    setFixedSize(QPixmap(pic_name).size());
}

void PushButton::enterEvent(QEvent *)
{
    status = ENTER;
    update();
}

void PushButton::mousePressEvent(QMouseEvent *event)
{
    //若点击鼠标左键
    if(event->button() == Qt::LeftButton)
    {
        mouse_press = true;
        status = PRESS;
        update();
    }
}

void PushButton::mouseReleaseEvent(QMouseEvent *event)
{
    //若点击鼠标左键
    if(mouse_press  && this->rect().contains(event->pos()))
    {
        mouse_press = false;
        status = ENTER;
        update();
        emit clicked();
    }
}

void PushButton::leaveEvent(QEvent *)
{
    status = NORMALSTU;
    update();
}

void PushButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPixmap pixmap;
    switch(status)
    {
    case NORMALSTU:
        {
            pixmap.load(pic_name);
            break;
        }
    case ENTER:
        {
            pixmap.load(pic_name + QString("_hover"));
            break;
        }
    case PRESS:
        {
            pixmap.load(pic_name + QString("_pressed"));
            break;
        }
    case NOSTATUS:
        {
            pixmap.load(pic_name);
            break;
        }
    default:
        pixmap.load(pic_name);
    }

    painter.drawPixmap(rect(), pixmap);
}
}


