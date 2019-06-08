#include "aithread.h"

#include <QString>
#include <QTime>

enum TIME_PARAM
{
	ALL_MIN_TIME = 150,
	WIN_MIN_TIME = 150,
	DEF_MIN_TIME = 800,
	LOSE_MIN_TIME = 2000
};

AiThread::AiThread(QObject *parent)
	: QThread(parent)
{

}

void AiThread::run()
{
	srand(time(NULL));
	int start_time = QTime::currentTime().msecsSinceStartOfDay();
	qDebug() << "\nsearching...";

	int board_score = 0;
	if (round == 0)
	{
		this->firstRound();
		board_score = A2score;
	}
	else if (round == 1 && is_black)
	{
		this->secondRound();
		board_score = -A2score;
	}
	else if (all_type[I_AI_P4] || all_type[I_AI_A4])
	{
		this->highestPriorityAttack();
		board_score = C5score+20;
	}
	else if (all_type[I_HU_P4])
	{
		this->highestPriorityDefence();
		board_score = prev_score-1;
	}
	else
	{
		board_score = minMaxSearch(AI_CHESS, 0, -INF, INF);
	}
	if((board_score >= C5score || board_score <= -C5score) && !warned)
	{
		warned = true;
		emit sweetWarning();
	}

	// 实际搜索花费时间
	int cost_time = QTime::currentTime().msecsSinceStartOfDay() - start_time;

	// VCF/VCT 缓冲时间
	if ( round >= 4 && board_score >= C5score && cost_time < WIN_MIN_TIME )
		this->msleep(WIN_MIN_TIME - cost_time);

	// 劣势缓冲时间
	else if ( round >= 4 && board_score <= 0 && cost_time < DEF_MIN_TIME )
		this->msleep(DEF_MIN_TIME - cost_time);

	// 大劣缓冲时间
	else if ( round >= 4 && board_score <= -C5score)
		this->msleep(LOSE_MIN_TIME);

	// 最小缓冲时间
	else if ( round >= 4 && cost_time < ALL_MIN_TIME)
		this->msleep(ALL_MIN_TIME - cost_time);
	// 最终花费时间
	cost_time = QTime::currentTime().msecsSinceStartOfDay() - start_time;
	qDebug() << "score : " << board_score << "time : " << cost_time;
//	qDebug() << "AI move at" << ai_next_move.x << "," << ai_next_move.y;

	emit foundNextMove(ai_next_move);
	round++;
	prev_score = board_score;
}

void AiThread::setIsBlack()
{
	is_black = true;
}

void AiThread::setNotBlack()
{
	is_black = false;
}

void AiThread::setNotWarned()
{
	warned = false;
}

void AiThread::resetRound()
{
	round = 1;
}

bool AiThread::win() const
{
	return all_type[I_AI_C5];
}

bool AiThread::lose() const
{
	return all_type[I_HU_C5];
}

void AiThread::printChessType()
{
	qDebug() << "C5A4" << all_type[1] << all_type[2] << all_type[3] << all_type[4];
	qDebug() << "P4A3" << all_type[5] << all_type[6] << all_type[7] << all_type[8];
	qDebug() << "S3A2" << all_type[9] << all_type[10] << all_type[11] << all_type[12];
	qDebug() << "S2A1" << all_type[13] << all_type[14] << all_type[15] << all_type[16];
}

void AiThread::putChess(const int x, const int y, const Chessid id)
{
//	xor_sum ^= zobrist[id][y][x];
	mirror_board[y][x] = id;
	__int32 idl2y = id << 2*y;
	__int32 idl2x = id << 2*x;

	col_chess[x] |= idl2y;
	row_chess[y] |= idl2x;

	int l = x - y + 10;
	if (l>=0 && l<GRID_DN)
	{
		if (l<=10)
			left_chess[l] |= idl2x;
		else
			left_chess[l] |= idl2y;
	}

	int r = x + y - 4;
	if (r>=0 && r<GRID_DN)
	{
		if (r<=10)
			right_chess[r] |= idl2y;
		else
			right_chess[r] |= id << (28-2*x);
	}

	updateChessType(x, y);
}



void AiThread::takeChess(const int x, const int y)
{
//	xor_sum ^= zobrist[id][y][x];
	mirror_board[y][x] = 0;
	__int32 a3l2y = ~(3 << 2*y);
	__int32 a3l2x = ~(3 << 2*x);

	col_chess[x] &= a3l2y;
	row_chess[y] &= a3l2x;

	int l = x - y + 10;
	if (l>=0 && l<GRID_DN)
	{
		if (l<=10)
			left_chess[l] &= a3l2x;
		else
			left_chess[l] &= a3l2y;
	}

	int r = x + y - 4;
	if (r>=0 && r<GRID_DN)
	{
		if (r<=10)
			right_chess[r] &= a3l2y;
		else
			right_chess[r] &= ~(0x3 << (28-2*x));
	}

	updateChessType(x, y);
}

void AiThread::updateChessType(const int x, const int y)
{
	int count, i;
	__int32 temp1 = col_type[x];
	__int32 temp2 = row_type[y];

	{
		// 删除之前的行列棋型
		RESET(col_type[x], temp1, all_type);
		RESET(row_type[y], temp2, all_type);

		// 重新判断行列棋型
		for(i=0, temp1=col_chess[x], temp2=row_chess[y]; i<10; ++i, temp1>>=2, temp2>>=2)
		{
			col_type[x] |= M_TABLE[temp1 & MASK];
			row_type[y] |= M_TABLE[temp2 & MASK];
		}

		// 处理 4 子冲突
		DEAL_34(col_type[x]);
		DEAL_34(row_type[y]);

		// 记录当前行列棋型
		temp1 = col_type[x];
		SET(temp1, all_type);
		temp2 = row_type[y];
		SET(temp2, all_type);
	}

	int l = x - y + 10;
	if (l>=0 && l<GRID_DN)
	{
		temp1 = left_type[l];
		RESET(left_type[l], temp1, all_type);

		for(i=0, temp1=left_chess[l], count=getLeftCount(x, y); i<count; ++i, temp1>>=2)
			left_type[l] |= M_TABLE[temp1 & MASK];

		DEAL_34(left_type[l]);

		temp1 = left_type[l];
		SET(temp1, all_type);
	}

	int r = x + y - 4;
	if (r>=0 && r<GRID_DN)
	{
		temp2 = right_type[r];
		RESET(right_type[r], temp2, all_type);

		for(i=0, temp2=right_chess[r], count=getRightCount(x, y); i<count; ++i, temp2>>=2)
			right_type[r] |= M_TABLE[temp2 & MASK];

		DEAL_34(right_type[r]);

		temp2 = right_type[r];
		SET(temp2, all_type);
	}
}
