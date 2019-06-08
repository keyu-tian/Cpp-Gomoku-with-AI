#include "aithread.h"

#include <ctime>

#define BOARD_CTR 7

void AiThread::firstRound()
{
	// 第一轮，若AI先手，则选择棋盘正中心；若后手但对手没有落子在正中心，也选择正中心
	if (mirror_board[BOARD_CTR][BOARD_CTR] == NO_CHESS)
		ai_next_move.x = ai_next_move.y = BOARD_CTR;
	else // AI后手且对手落在了正中心，AI随机落在其四个对角线位置
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
	emit findingNextMove(ai_next_move);

	this->msleep(300);
}

void AiThread::secondRound()
{
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
		if ( BOARD_CTR==x )		// 直止，花月
		{
			ai_next_move.x = x + 2*(rand()&1)-1;
			ai_next_move.y = y;
		}
		else if ( BOARD_CTR==y )	// 直止，花月
		{
			ai_next_move.x = x;
			ai_next_move.y = y + 2*(rand()&1)-1;
		}
		else	// 斜止，浦月
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
	emit findingNextMove(ai_next_move);

	this->msleep(800);
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
	emit findingNextMove(ai_next_move);
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
	emit findingNextMove(ai_next_move);
}
