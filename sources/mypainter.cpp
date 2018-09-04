#include "mypainter.h"

#include <QTime>

enum BW
{
	BLACK1 = 58,
	BLACK2 = 64,
	WHITE1 = 238,
	WHITE2 = 244
};

MyPainter::MyPainter(QPaintDevice *parent)
	: QPainter(parent)
{

}

void MyPainter::init()
{
	this->setPen(Qt::black);
	this->setRenderHint(QPainter::Antialiasing, true);	// 抗锯齿
}

void MyPainter::drawBoard()
{
	for(int i=BOARD_L; i<=BOARD_R; i+=GRID_W)
	{
		this->drawLine(BOARD_L, i, BOARD_R, i);	// 水平线
		this->drawLine(i, BOARD_L, i, BOARD_R);	// 竖直线
	}
	this->drawRect(BOARD_L-10, BOARD_L-10, BOARD_W + 20, BOARD_W + 20);
}

void MyPainter::drawMark(const Grid &c)
{
	QRadialGradient radi;

//	if (game->playerFlag)
//		radi.setColorAt(0,qRgb(WHITE1, WHITE1, WHITE1));
//	else
		radi.setColorAt(0,qRgb(BLACK2, BLACK2, BLACK2));
	this->setBrush(radi);
	this->drawRect(BOARD_L + GRID_W * c.x + GRID_W - MARK_R,
			 BOARD_L + GRID_W * c.y + GRID_W - MARK_R,
			 2 * MARK_R, 2 * MARK_R);
	// 以上 +GRID_W 是因为 棋格坐标（0,0）-> 屏幕坐标（GRID_W, GRID_W）
}

void MyPainter::drawChess(const ChessBoard &c)
{
	this->setPen(Qt::NoPen);
	QRadialGradient radi;

	for (int i=0; i<GRID_N; i++)
	{
		for (int j=0; j<GRID_N; j++)
		{
			Chessid now_id = c.getChessID(i, j);
			if (now_id != NO_CHESS)
			{
				if (now_id == AI_CHESS)
				{
//					radi.setColorAt(0,qRgb(WHITE2, WHITE2, WHITE2));
//					this->setBrush(radi);
//					this->drawEllipse(BOARD_UB + GRID_W * (i+1) - CHESS_R-2, BOARD_UB + GRID_W * (j+1) - CHESS_R-2, (CHESS_R+2) * 2, (CHESS_R+2) * 2);

					radi.setColorAt(0,qRgb(BLACK1, BLACK1, BLACK1));
					this->setBrush(radi);
					this->drawEllipse(BOARD_L + GRID_W * (i+1) - CHESS_R, BOARD_L + GRID_W * (j+1) - CHESS_R, CHESS_R * 2, CHESS_R * 2);
				}
				else
				{
//					radi.setColorAt(0,qRgb(WHITE2, WHITE2, WHITE2));
//					this->setBrush(radi);
//					this->drawEllipse(BOARD_UB + GRID_W * (i+1) - CHESS_R-1, BOARD_UB + GRID_W * (j+1) - CHESS_R-1, (CHESS_R+1) * 2, (CHESS_R+1) * 2);

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


