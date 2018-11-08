#include "chess_board.h"
#include <cstring>

ChessBoard::ChessBoard()
{

}

void ChessBoard::initBoard()
{
	memset(board, 0, sizeof(board));
}

void ChessBoard::putChess(int x, int y, Chessid id)
{
	board[y][x] = id;
}

void ChessBoard::takeChess(int x, int y)
{
	board[y][x] = NO_CHESS;
}

void ChessBoard::setBlackPlayerID(Chessid id)
{
	this->black_player_id = id;
}

Chessid ChessBoard::getBlackPlayerID() const
{
	return this->black_player_id;
}

bool ChessBoard::isInside(int r, int c) const
{
	return r>=0 && r<GRID_N &&
	       c>=0 && c<GRID_N;
}

bool ChessBoard::isInside(const Grid &c) const
{
	return c.x>=0 && c.x<GRID_N &&
		   c.y>=0 && c.y<GRID_N;
}

bool ChessBoard::isAvaliable(const Grid &c) const
{
	return c.x>=0 && c.x<GRID_N &&
		   c.y>=0 && c.y<GRID_N &&
			board[c.y][c.x] == NO_CHESS;
}

bool ChessBoard::win(const Grid &just_now, const Chessid id)
{
	static const int dr[] = {-1, 1, 0, 0, 1, 1, -1, -1};
	static const int dc[] = {0, 0, -1, 1, 1, -1, 1, -1};
	int r, c, r0, c0, cnt;
	r0 = just_now.y;
	c0 = just_now.x;

	bool isWin = false;

	for (int i=0; i<8; i++)
	{
		cnt = 1;

		r = r0 + dr[i];
		c = c0 + dc[i];
		while (isInside(r, c) && board[r][c] == id)
		{
			cnt++;
			r += dr[i];
			c += dc[i];
		}

		r = r0 - dr[i];
		c = c0 - dc[i];
		while (isInside(r, c) && board[r][c] == id)
		{
			cnt++;
			r -= dr[i];
			c -= dc[i];
		}

		if (cnt>=5)
		{
			isWin = true;
			break;
		}
	}

	return isWin;
}

Chessid ChessBoard::getChessID(int x, int y) const
{
	return board[y][x];
}
