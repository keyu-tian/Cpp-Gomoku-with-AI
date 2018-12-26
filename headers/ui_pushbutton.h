#ifndef UI_PUSHBUTTON_H
#define UI_PUSHBUTTON_H

#include <QPushButton>
#include <QCloseEvent>

class UIPushbutton : public QPushButton
{
	public:

		UIPushbutton(QWidget *parent = Q_NULLPTR);
		virtual void closeEvent(QEvent *event);
		void test();
};

#endif // UI_PUSHBUTTON_H
