#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include "base.h"

class ChessBoard
{
	private:

		Chessid black_player_id;
		Chessid white_player_id;
		Chessid board[GRID_N][GRID_N];

	public:
		ChessBoard();

		void init();
		void putChess(int x, int y, Chessid id);
		void takeChess(int x, int y);

		bool isInside(const Grid &c) const;
		bool isAvaliable(const Grid &c) const;
		bool win(const Grid &just_now, const Chessid id);
		Chessid getChessID(int x, int y) const;
		Chessid getBlackID() const;
		Chessid getWhiteID() const;

};

#endif // CHESSBOARD_H
