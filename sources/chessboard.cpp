#include "chessboard.h"
#include <cstring>

ChessBoard::ChessBoard()
{

}

void ChessBoard::init()
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
	int r, c, cnt, x, y;
	x = just_now.x;
	y = just_now.y;

	r=x, c=y, cnt=0;
	while (c<GRID_N && board[r][c] == id)
		cnt++, c++;
	c = y-1;
	while ( c>=0 && board[r][c] == id)
		cnt++, c--;
	if (cnt>=5)
		return true;

	r=x, c=y, cnt=0;
	while ( r<GRID_N && board[r][c] == id)
		cnt++, r++;
	r = x-1;
	while (  r>=0 && board[r][c] == id)
		cnt++, r--;
	if (cnt>=5)
		return true;

	r=x, c=y, cnt=0;
	while ( c<GRID_N && r<GRID_N && board[r][c] == id)
		cnt++,c++,r++;
	c=y-1, r=x-1;
	while ( r>=0  && c>=0  && board[r][c] == id)
		cnt++, c--, r--;
	if (cnt>=5)
		return true;

	r=x, c=y, cnt=0;
	while ( c<GRID_N && r>=0  && board[r][c] == id)
		cnt++, c++, r--;
	c=y-1, r=x+1;
	while ( c>=0  && r<GRID_N && board[r][c] == id)
		cnt++, c--, r++;
	if (cnt>=5)
		return true;

	return false;
}

Chessid ChessBoard::getChessID(int x, int y) const
{
	return board[y][x];
}

Chessid ChessBoard::getWhiteID() const
{
	return white_player_id;
}

Chessid ChessBoard::getBlackID() const
{
	return black_player_id;
}
