#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	qDebug() << "the sizeof MainWindow is : " << sizeof(w);

	return a.exec();
}
