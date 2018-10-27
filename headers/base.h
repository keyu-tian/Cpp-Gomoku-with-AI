#ifndef BASE_H
#define BASE_H

#include <QDebug>

enum CHESSID
{
	NO_CHESS	=	0x0,
	AI_CHESS	=	0x1,
	H1_CHESS	=	0x2,
	H2_CHESS	=	0x3,
	OUT_CHESS	=	0x3
};

enum GRID_CNT
{
	GRID_N   = 15,
	GRID_DN  = 21
};

enum CTRL_BUTTON_INDEX
{
	NEW_BUTTON,
	RESTART_BUTTON,
	RETRACT_BUTTON,
	CHANGE_BUTTON,
	CTRL_BUTTON_N
};

enum TITLE_BUTTON_INDEX
{
	MINI_BUTTON,
	CLOSE_BUTTON,
	TITLE_BUTTON_N
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
};

typedef char Chessid;


#endif // BASE_H
