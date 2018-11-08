#include "ui_pushbutton.h"
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

UIPushbutton::UIPushbutton(QWidget *parent)
	: QPushButton(parent)
{

}

void UIPushbutton::closeEvent(QEvent *event)
{
	QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
	animation->setDuration(3000);
	animation->setStartValue(1);
	animation->setEndValue(0);
	animation->start();

	event->accept();
}

void UIPushbutton::test()
{
	QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
	animation->setDuration(1000);
	animation->setStartValue(1);
	animation->setEndValue(0);
	animation->start();
	connect(animation, &QPropertyAnimation::finished, this, &UIPushbutton::hide);
}
