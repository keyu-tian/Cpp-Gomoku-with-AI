#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include "base.h"

class ChessBoard
{
	private:

		// 记录黑方和棋盘信息
		Chessid black_player_id;
		Chessid board[GRID_N][GRID_N];

	public:

		ChessBoard();

		void initBoard();
		void putChess(int x, int y, Chessid id);
		void takeChess(int x, int y);

		Chessid getChessID(int x, int y) const;
		Chessid getBlackPlayerID() const;
		void setBlackPlayerID(Chessid id);

		//  判断是否越界/是合法落子点/刚刚落子是否造成胜利
		bool isInside(int r, int c) const;
		bool isInside(const Grid &c) const;
		bool isAvaliable(const Grid &c) const;
		bool win(const Grid &just_now, const Chessid id);

};

#endif // CHESSBOARD_H
