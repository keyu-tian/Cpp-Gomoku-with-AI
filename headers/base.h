#ifndef BASE_H
#define BASE_H

#include <QDebug>

enum GRID_PARAMETER
{
	GRID_N   = 15,
	GRID_DN  = 19,
	GRID_W   = 40,
	GRID_OUT = 2,
};

enum CHESS_ID
{
	NO_CHESS,
	AI_CHESS,
	H1_CHESS,
	H2_CHESS,
	CHESS_ID_N
};

enum : bool
{
	BLACK = true,
	WHITE = false
};

struct Grid
{
	int x, y;
	Grid(void);
	Grid(int x, int y);
};

struct Choice
{
	int x, y;
	int prior;
	bool operator < (const Choice &c) const;
};

typedef char Chessid;


#endif // BASE_H
