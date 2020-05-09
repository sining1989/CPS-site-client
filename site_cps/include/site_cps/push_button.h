#ifndef SITE_CPS_PUSHBUTTON_H
#define SITE_CPS_PUSHBUTTON_H

#include <QPushButton>
#include <QPainter>
#include <QMouseEvent>
#include "cps.h"

namespace site_cps
{

class PushButton : public QPushButton
{
	Q_OBJECT

public:

	explicit PushButton(QWidget *parent = 0);
	~PushButton();
	void setPicName(QString pic_name);

protected:

	void enterEvent(QEvent *);
	void leaveEvent(QEvent *);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void paintEvent(QPaintEvent *);

private:
	ButtonStatus status;
	QString pic_name;

	int btn_width; //按钮宽度
	int btn_height; //按钮高度
	bool mouse_press; //按钮左键是否按下
};
}
#endif //SITE_CPS_PUSHBUTTON_H

