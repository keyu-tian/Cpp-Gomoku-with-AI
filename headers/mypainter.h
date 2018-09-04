#ifndef MYPAINTER_H
#define MYPAINTER_H

#include "base.h"
#include "chessboard.h"

#include <QBrush>
#include <QColor>
#include <QPainter>
#include <QPen>

enum SCREEN_PARAMETER
{
	SCREEN_W = 790,
	SCREEN_H = (int)(SCREEN_W * 1.08)
};

enum BOARD_PARAMETER
{
	BOARD_W = (GRID_N+1) * GRID_W,
	BOARD_L = (SCREEN_W - BOARD_W)/2,
	BOARD_U = BOARD_L,
	BOARD_R = BOARD_L + BOARD_W,
	BOARD_D = BOARD_R
};

enum RADIUS
{
	MARK_R = 2,
	CHESS_R = 14,
	DETECT_R = GRID_W/2-2
};

class MyPainter : public QPainter
{
	public:
		MyPainter(QPaintDevice *parent = Q_NULLPTR);
		void init(void);
		void drawBoard(void);
		void drawMark(const Grid &c);
		void drawChess(const ChessBoard &c);
};

#endif // MYPAINTER_H
