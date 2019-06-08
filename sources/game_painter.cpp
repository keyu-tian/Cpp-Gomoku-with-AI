#include "game_painter.h"
#include <QTime>

enum BW
{
	BLACK1 = 58,
	BLACK2 = 64,
	WHITE1 = 238,//239
};

GamePainter::GamePainter(QPaintDevice *parent)
	: QPainter(parent)
{

}

void GamePainter::init()
{
	this->setPen(Qt::black);
	this->setRenderHint(QPainter::Antialiasing, true);	// 抗锯齿
}

void GamePainter::drawGrid()
{
	for(int i=BOARD_L; i<=BOARD_R; i+=GRID_W)
	{
		this->drawLine(BOARD_L, i, BOARD_R, i);	// 水平线
		this->drawLine(i, BOARD_L, i, BOARD_R);	// 竖直线
	}
	this->drawRect(BOARD_L-7, BOARD_L-7, BOARD_W+14, BOARD_W+14);
}

void GamePainter::drawKeyPos(const Grid &c, bool is_P4)
{
	QPen prev_pen = this->pen();
	QRadialGradient radi;

	radi.setColorAt(0, qRgb(WHITE1, WHITE1, WHITE1));
	this->setBrush(radi);

	if (is_P4)
		this->setPen(qRgb(255, 100, 110));
	else
		this->setPen(qRgb(13, 144, 233));

	this->drawRect(BOARD_L + GRID_W * c.x + GRID_W - MARK_R,
			 BOARD_L + GRID_W * c.y + GRID_W - MARK_R,
			 2 * MARK_R, 2 * MARK_R);
	// 以上 +GRID_W 是因为 棋格坐标（0, 0） <=> 屏幕坐标（GRID_W, GRID_W）

	this->setPen(prev_pen);
}

void GamePainter::drawMark(const Grid &c, bool is_black)
{
	QPen prev_pen = this->pen();
	QRadialGradient radi;

	if (is_black)
		radi.setColorAt(0, qRgb(BLACK1, BLACK1, BLACK1));
	else
		radi.setColorAt(0, qRgb(WHITE1, WHITE1, WHITE1));

	this->setBrush(radi);
	this->setPen(qRgb(BLACK1, BLACK1, BLACK1));
	this->drawRect(BOARD_L + GRID_W * c.x + GRID_W - MARK_R,
			 BOARD_L + GRID_W * c.y + GRID_W - MARK_R,
			 2 * MARK_R, 2 * MARK_R);
	// 以上 +GRID_W 是因为 棋格坐标（0, 0） <=> 屏幕坐标（GRID_W, GRID_W）

	this->setPen(prev_pen);
}

void GamePainter::drawAIPreMove(const Grid &c)
{
	QPen prev_pen = this->pen();
	QRadialGradient radi;
	radi.setColorAt(0, qRgb(WHITE1, WHITE1, WHITE1));

	QPen pen;
//	pen.setColor(qRgb(100, 160, 50));
	pen.setColor(qRgb(13, 144, 233));
	pen.setWidth(2);
	pen.setStyle(Qt::DotLine);

	this->setPen(pen);
	this->setBrush(radi);
	this->drawEllipse(BOARD_L + GRID_W * (c.x+1) - CHESS_R, BOARD_L + GRID_W * (c.y+1) - CHESS_R, CHESS_R * 2, CHESS_R * 2);
	// 以上 +GRID_W 是因为 棋格坐标（0, 0） <=> 屏幕坐标（GRID_W, GRID_W）

	this->setPen(prev_pen);
}

void GamePainter::drawChess(const ChessBoard &c)
{
	this->setPen(Qt::NoPen);
	QRadialGradient radi;

	for (int i=0; i<GRID_N; i++)
	{
		for (int j=0; j<GRID_N; j++)
		{
			Chessid now_id = c.getChessID(i, j);
			Chessid black_player_id = c.getBlackPlayerID();
			if (now_id != NO_CHESS)
			{
				if (now_id == black_player_id)
				{
					radi.setColorAt(0,qRgb(BLACK1, BLACK1, BLACK1));
					this->setBrush(radi);
					this->drawEllipse(BOARD_L + GRID_W * (i+1) - CHESS_R, BOARD_L + GRID_W * (j+1) - CHESS_R, CHESS_R * 2, CHESS_R * 2);
				}
				else
				{
					radi.setColorAt(0,qRgb(BLACK2, BLACK2, BLACK2));
					this->setBrush(radi);
					this->drawEllipse(BOARD_L + GRID_W * (i+1) - CHESS_R, BOARD_L + GRID_W * (j+1) - CHESS_R, CHESS_R * 2, CHESS_R * 2);

					radi.setColorAt(0,qRgb(WHITE1, WHITE1, WHITE1));
					this->setBrush(radi);
					this->drawEllipse(BOARD_L + GRID_W * (i+1) - CHESS_R+2, BOARD_L + GRID_W * (j+1) - CHESS_R+2, (CHESS_R-2) * 2, (CHESS_R-2) * 2);
				}
			}
		}
	}
}

void GamePainter::drawLastMove(const Grid &c, bool is_black)
{
	QPen prev_pen = this->pen();
	QRadialGradient radi;

	if (is_black)
	{
		radi.setColorAt(0, qRgb(BLACK1, BLACK1, BLACK1));
		this->setPen(qRgb(BLACK1, BLACK1, BLACK1));
	}
	else
	{
		radi.setColorAt(0, qRgb(WHITE1, WHITE1, WHITE1));
	}

	this->setBrush(radi);
	this->drawEllipse(BOARD_L + GRID_W * c.x + GRID_W - MARK_R,
			 BOARD_L + GRID_W * c.y + GRID_W - MARK_R,
			 2 * MARK_R, 2 * MARK_R);
	// 以上 +GRID_W 是因为 棋格坐标（0, 0） <=> 屏幕坐标（GRID_W, GRID_W）

	this->setPen(prev_pen);
}
