#include "main_window.h"

#include <QEvent>
#include <qt_windows.h>

#define Distance(x0, y0, x1, y1) sqrt(((x0)-(x1))*((x0)-(x1))+((y0)-(y1))*((y0)-(y1)))

void MainWindow::paintEvent(QPaintEvent *event)
{
	GamePainter painter(this);
	painter.init();
	painter.drawGrid();
	painter.drawChess(chess_board);

	if (chess_cnt && chess_board.isInside(history[chess_cnt-1]))
		painter.drawCross(history[chess_cnt-1], now_player_id == black_player_id);

	if (chess_board.isAvaliable(mouse_cursor) && now_player_id != AI_CHESS)
		painter.drawMark(mouse_cursor, now_player_id == black_player_id);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
	// 得到鼠标光标的屏幕坐标
	int x = event->x(),
		y = event->y();

	// 在范围内
	if (x >= BOARD_L && x <= BOARD_R &&
		y >= BOARD_L && y <= BOARD_R )
	{
		// 选取附近结点
		Grid near_grid((x-BOARD_L) / GRID_W, (y-BOARD_L) / GRID_W);

		// 找此节点周围范围内的节点
		// 若找到，设置其中最近的一点为标记点
		this->findNearistGrid(near_grid, x, y);
		this->update();
	}
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
	if (game_mode == PVE_MODE)
	{
		if (chess_board.isAvaliable(mouse_cursor) && this->now_player_id == H1_CHESS)
		{
			PVERound();				// 现在是玩家的回合，并且该落子点可用，玩家落子
		}
	}
	else if (game_mode == PVP_MODE)
	{
		if (chess_board.isAvaliable(mouse_cursor))
		{
			PVPRound();			// 当前玩家落子
		}
	}
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
	if ( ReleaseCapture() && event->y()<=TITLE_BUTTON_D )
	{
		QWidget *pWindow = this->window();
		if (pWindow->isTopLevel())
		{
		   SendMessage(HWND(pWindow->winId()), WM_SYSCOMMAND, SC_MOVE + HTCAPTION, 0);
		}
	}
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	if (now_player_id == AI_CHESS)
		ai_thread.terminate();
	else
		ai_thread.quit();

	ai_thread.wait();
	qDebug() << "Exit safely.";
	event->accept();
}

void MainWindow::findNearistGrid(const Grid &near_grid, int x, int y)
{
	static const int dx[] = {0, 1, 0, 1};
	static const int dy[] = {0, 0, 1, 1};
	// 计算屏幕坐标
	int x0 = BOARD_L + GRID_W * near_grid.x,
		y0 = BOARD_L + GRID_W * near_grid.y;
	// 初始化，默认找不到标记点
	mouse_cursor.x = mouse_cursor.y = INVALID_FLAG;

	// 寻找并更新
	int min_dis = SCREEN_H*SCREEN_H;
	for(int i=0; i<4; i++)
	{
		int xi = x0 + dx[i]*GRID_W,
			yi = y0 + dy[i]*GRID_W;
		int now_dis = Distance(x, y, xi, yi);
		if (DETECT_R >= now_dis && min_dis > now_dis)
		{
			mouse_cursor.x = near_grid.x + dx[i] - 1;	// -1 是因为 棋格坐标（0,0）-> 屏幕格点（1, 1）
			mouse_cursor.y = near_grid.y + dy[i] - 1;	// -1 是因为 棋格坐标（0,0）-> 屏幕格点（1, 1）
			min_dis = now_dis;
		}
	}
}
