#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>

#include "chess_board.h"
#include "aithread.h"
#include "game_painter.h"
#include "ui_style.h"
#include "ui_pushbutton.h"
#include "ui_size_param.h"

enum GAME_MODE
{
	PVE_MODE,
	PVP_MODE
};

enum FLAGS
{
	INVALID_FLAG = -1
};

class MainWindow : public QMainWindow
{
	Q_OBJECT

	private:

		char game_mode;
		ChessBoard chess_board;
		Chessid black_player_id;
		Chessid now_player_id;

		Grid mouse_cursor;

		Grid history[GRID_N * GRID_N];
		Grid ai_pre_move;
		int chess_cnt;

		AiThread ai_thread;

		UIPushbutton ctrl_button[CTRL_BUTTON_N];
		UIPushbutton title_button[TITLE_BUTTON_N];

		int P4_key_pos_cnt;
		int SA3_key_pos_cnt;
		Choice P4_key_pos[GRID_N * GRID_N];
		Choice A3_key_pos[GRID_N * GRID_N];

	public:

		MainWindow(QWidget *parent = 0);
		~MainWindow();

		// 初始化
		void initWindow();
		void initButton();
		void setConnections();

		// 游戏逻辑
		void startPVEGame();
		void startPVPGame();
		void resetPVEGame();
		void resetPVPGame();
		void retractPVEGame();
		void retractPVPGame();
		void PVERound();
		void PVPRound();
		void aiMove(const Grid &ai_next_move);
		void aiPreMove(const Grid &ai_pre_move);
		void PVEPutChess(const Grid &next_move, const Chessid id);
		void PVPPutChess(const Grid &next_move, const Chessid id);
		void sweetWarning();

		// 辅助UI的函数
		void findNearistGrid(const Grid &near_grid, int x, int y);
		void resetMarks(void);
		void clearMarks(void);

	protected:

		// UI相关
		virtual void paintEvent(QPaintEvent *event);
		virtual void mouseMoveEvent(QMouseEvent *event);	// 监听鼠标，显示标记节点
		virtual void mouseReleaseEvent(QMouseEvent *event);	// 玩家落子
		virtual void mousePressEvent(QMouseEvent *event);	// 窗口移动
		virtual void closeEvent(QCloseEvent* event);
};

#endif // MAINWINDOW_H
