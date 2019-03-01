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

		// 注意绘图的顺序：网格点->关键位点->鼠标标记点->AI预落子点->棋子->上次落子点
		void init(void);
		void drawGrid(void);
		void drawKeyPos(const Grid &c, bool is_P4);
		void drawMark(const Grid &c, bool is_black);
		void drawAIPreMove(const Grid &c);
		void drawChess(const ChessBoard &c);
		void drawLastMove(const Grid &c, bool is_black);
};

#endif // GAME_PAINTER_H
