#include "main_window.h"

#include <cstring>
#include <ctime>

#include <QApplication>
#include <QPropertyAnimation>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent),
	  game_mode(PVE_MODE),
	  black_player_id(AI_CHESS)
{
	qRegisterMetaType<Grid>("Grid");	// 注册后 Grid 类型才可以作为信号的参数


	this->initWindow();
	this->initButton();
	this->setConnections();

	ai_thread.initHashTable();  // 这个 init，在整个程序运行过程中只需进行一次

	if (game_mode == PVE_MODE)
	{
		this->resetPVEGame();
		this->startPVEGame();
	}
	else if (game_mode == PVP_MODE)
	{
		this->resetPVPGame();
		this->startPVPGame();
	}

//	qDebug() << "Inited. The size of MainWindow is : " << sizeof(*this);
}

MainWindow::~MainWindow()
{

}

void MainWindow::initWindow()
{
	QFont font("Century Gothic", 10);
	QApplication::setFont(font);

	this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
	this->setFixedSize(SCREEN_W, SCREEN_H);
	this->setWindowTitle(QString("小猪佩棋的对弈房间"));
	this->move(SCREEN_W/2, SCREEN_H/8);
	this->setMouseTracking(true);

	QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
	animation->setDuration(250);
	animation->setStartValue(0);
	animation->setEndValue(1);
	animation->start();
}

void MainWindow::initButton()
{
	for(int i=0; i<CTRL_BUTTON_N; i++)
	{
		ctrl_button[i].setParent(this);
		ctrl_button[i].setGeometry(CTRL_BUTTON_L + i*CTRL_BUTTON_ITV, CTRL_BUTTON_U,
							  CTRL_BUTTON_W, CTRL_BUTTON_H);
		ctrl_button[i].setStyleSheet(CTRL_BUTTON_STYLE);
	}

	for(int i=0; i<TITLE_BUTTON_N; i++)
	{
		title_button[i].setParent(this);
		title_button[i].setGeometry(TITLE_BUTTON_L + i*TITLE_BUTTON_ITV, TITLE_BUTTON_U + i*TITLE_BUTTON_ITV/16,
							  TITLE_BUTTON_W, TITLE_BUTTON_H);
		title_button[i].setStyleSheet(TITLE_BUTTON_STYLE[i]);
	}

	title_button[MINI_BUTTON].setText("▁");
	title_button[CLOSE_BUTTON].setText("×");

	ctrl_button[NEW_BUTTON].setText("New");
	ctrl_button[RESTART_BUTTON].setText("Restart");
	ctrl_button[RETRACT_BUTTON].setText("Retract");
	ctrl_button[CHANGE_BUTTON].setText("Change");

}

void MainWindow::setConnections()
{
	// 设置AI搜索线程与UI线程间的信号通信
	connect(&ai_thread, &AiThread::foundNextMove, this, &MainWindow::aiMove);
	connect(&ai_thread, &AiThread::sweetWarning, this, &MainWindow::sweetWarning);

	// 设置标题栏按钮的信号通信
	connect(title_button+MINI_BUTTON, &UIPushbutton::clicked, this, &MainWindow::showMinimized);
	connect(title_button+CLOSE_BUTTON, &UIPushbutton::clicked, this, &MainWindow::close);

	// 设置操作按钮的信号通信
	connect(ctrl_button+NEW_BUTTON, &UIPushbutton::clicked,
				[&]()
				{
					if (game_mode == PVE_MODE)
					{
						if (now_player_id == H1_CHESS)
						{
							if (black_player_id == AI_CHESS)
							{
								black_player_id = H1_CHESS;
							}
							else
							{
								black_player_id = AI_CHESS;
							}
							this->resetPVEGame();
							this->startPVEGame();
						}
					}
					else if (game_mode == PVP_MODE)
					{
						this->resetPVPGame();
						this->startPVPGame();
					}
				}
			);

	connect(ctrl_button+RESTART_BUTTON, &UIPushbutton::clicked,
				[&]()
				{
					if (game_mode == PVE_MODE)
					{
						if (now_player_id != AI_CHESS)
						{
							this->resetPVEGame();
							this->startPVEGame();
						}
					}
					else if (game_mode == PVP_MODE)
					{
						this->resetPVPGame();
						this->startPVPGame();
					}
				}
			);

	connect(ctrl_button+RETRACT_BUTTON, &UIPushbutton::clicked,
				[&]()
				{
					if (game_mode == PVE_MODE)
					{
						if (now_player_id != AI_CHESS)
						{
							this->retractPVEGame();
						}
					}
					else if (game_mode == PVP_MODE)
					{
						this->retractPVPGame();
					}
				}
			);

	connect(ctrl_button+CHANGE_BUTTON, &UIPushbutton::clicked,
				[&]()
				{
					if (now_player_id != AI_CHESS)
					{
						if (game_mode == PVE_MODE)
						{
							game_mode = PVP_MODE;
							black_player_id = H1_CHESS;
							this->resetPVPGame();
							this->startPVPGame();
						}
						else if (game_mode == PVP_MODE)
						{
							game_mode = PVE_MODE;
							black_player_id = AI_CHESS;
							this->resetPVEGame();
							this->startPVEGame();
						}
					}
				}
			);
}
