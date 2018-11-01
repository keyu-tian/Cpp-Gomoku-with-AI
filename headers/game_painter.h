#ifndef GAME_PAINTER_H
#define GAME_PAINTER_H

#include <QBrush>
#include <QColor>
#include <QPainter>
#include <QPen>

#include "base.h"
#include "chess_board.h"
#include "ui_size_param.h"

class GamePainter : public QPainter
{
	public:
		GamePainter(QPaintDevice *parent = Q_NULLPTR);
		void init(void);
		void drawGrid(void);
		void drawMark(const Grid &c, bool is_black);
		void drawCross(const Grid &c, bool is_black);
		void drawChess(const ChessBoard &c);
};

#endif // GAME_PAINTER_H
