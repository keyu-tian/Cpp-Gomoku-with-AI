#include "aithread.h"

#include <ctime>

#define BOARD_CTR 7

void AiThread::firstRound()
{
	this->msleep(300);

	if (mirror_board[BOARD_CTR][BOARD_CTR] == NO_CHESS)
		ai_next_move.x = ai_next_move.y = BOARD_CTR;
	else
	{
		int dx = 0, dy = 0;
		while (dx==0 || dy==0)
		{
			dx = rand()%3 - 1;
			dy = rand()%3 - 1;
		}
		ai_next_move.x = BOARD_CTR + dx;
		ai_next_move.y = BOARD_CTR + dy;
	}
}

void AiThread::secondRound()
{
	this->msleep(1000);

	int x, y;
	bool found;

	for(y=BOARD_CTR-1, found = false; not found && y<=BOARD_CTR+1; y++)
	{
		for(x=BOARD_CTR-1; not found && x<=BOARD_CTR+1; x++)
		{
			if (mirror_board[y][x]==H1_CHESS)
				found = true;
			if (found)
				break;
		}
		if (found)
			break;
	}
	if (found)	// 非妖刀
	{
		if ( BOARD_CTR==x )			// 直止，行花月
		{
			ai_next_move.x = x + 2*(rand()&1)-1;
			ai_next_move.y = y;
		}
		else if ( BOARD_CTR==y )	// 直止，行花月
		{
			ai_next_move.x = x;
			ai_next_move.y = y + 2*(rand()&1)-1;
		}
		else	// 斜止，行浦月
		{
			if (rand()&1)
			{
				ai_next_move.x = x;
				ai_next_move.y = 2*BOARD_CTR - y;
			}
			else
			{
				ai_next_move.x = 2*BOARD_CTR - x;
				ai_next_move.y = y;
			}
		}
	}
	else	// 妖刀，不予理睬
	{
		int dx = 0, dy = 0;
		while (dx==0 || dy==0)
		{
			dx = rand()%3 - 1;
			dy = rand()%3 - 1;
		}
		ai_next_move.x = BOARD_CTR + dx;
		ai_next_move.y = BOARD_CTR + dy;
	}
}

void AiThread::highestPriorityAttack()
{
	for (int i=0; i<GRID_N; i++)
	{
		for (int j=0; j<GRID_N; j++)
		{
			if (mirror_board[i][j] == NO_CHESS)
			{
				this->putChess(j, i, AI_CHESS);
				if (all_type[I_AI_C5])
				{
					ai_next_move.x = j;
					ai_next_move.y = i;
				}
				this->takeChess(j, i);
			}
		}
	}
}

void AiThread::highestPriorityDefence()
{
	for (int i=0; i<GRID_N; i++)
	{
		for (int j=0; j<GRID_N; j++)
		{
			if (mirror_board[i][j] == NO_CHESS)
			{
				this->putChess(j, i, H1_CHESS);
				if (all_type[I_HU_C5])
				{
					ai_next_move.x = j;
					ai_next_move.y = i;
				}
				this->takeChess(j, i);
			}
		}
	}
}
