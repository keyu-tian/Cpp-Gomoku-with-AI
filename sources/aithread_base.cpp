#include "aithread.h"

#include <QString>
#include <QTime>

enum TIME_PARAM
{
	ALL_MIN_TIME = 150,
	WIN_MIN_TIME = 150,
	DEF_MIN_TIME = 1000,
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
	qDebug() << "searching";

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

bool AiThread::win()
{
	return all_type[I_AI_C5];
}

bool AiThread::lose()
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

void AiThread::initHashTable()
{
	this->initMTable();
	this->initCountTable();
}

void AiThread::initBoard()
{
	mem0(mirror_board);

	mem0(col_chess);
	mem0(col_type);
	mem0(row_chess);
	mem0(row_type);
	mem0(left_chess);
	mem0(left_type);
	mem0(right_chess);
	mem0(right_type);
	mem0(all_type);

	left_chess[0] = left_chess[GRID_DN-1] = OUT_CHESS << 10;
	right_chess[0] = right_chess[GRID_DN-1] = OUT_CHESS << 10;

	this->setNotWarned();
	round = 0;
}


void AiThread::initMTable()
{
	mem0(M_TABLE);
	memf1(P4_KEY_POS_TABLE);
	memf1(A3_KEY_POS_TABLE);

	// AI连五
	INIT(0x555, AI_C5);
	INIT(0x554, AI_C5);
	INIT(0x155, AI_C5);
	INIT(0x556, AI_C5);
	INIT(0x955, AI_C5);
	INIT(0xd55, AI_C5);  // 五格

	// Man连五
	INIT(0xaaa, HU_C5);
	INIT(0xaa8, HU_C5);
	INIT(0x2aa, HU_C5);
	INIT(0xaa9, HU_C5);
	INIT(0x6aa, HU_C5);
	INIT(0xeaa, HU_C5);  // 五格

	//AI活四
	INIT(0x154, AI_A4);

	//Man活四
	INIT(0x2a8, HU_A4);

	//AI活三
	INIT(0x150, AI_A3, 1, 5, 0);	// .ooo..（右边是低位）
	INIT(0x144, AI_A3, 2, 5, 0);	// .oo.o.
	INIT(0x114, AI_A3, 3, 0, 5);	// .o.oo.
	INIT(0x054, AI_A3, 4, 0, 5);	// ..ooo.
//	INIT(0xc54, AI_A3);				// 五格没有活三！

	//Man活三
	INIT(0x2a0, HU_A3, 1, 5, 0);	// .xxx..
	INIT(0x288, HU_A3, 2, 5, 0);	// .xx.x.
	INIT(0x228, HU_A3, 3, 0, 5);	// .x.xx.
	INIT(0x0a8, HU_A3, 4, 0, 5);	// ..xxx.
//	INIT(0xca8, HU_A3);				// 五格没有活三！

	//AI活二
	INIT(0x140, AI_A2);
	INIT(0x110, AI_A2);
	INIT(0x104, AI_A2);
	INIT(0x050, AI_A2);
	INIT(0xc50, AI_A2);  // 五格
	INIT(0x044, AI_A2);
	INIT(0xc44, AI_A2);  // 五格
	INIT(0x014, AI_A2);
	INIT(0xc14, AI_A2);  // 五格

	//Man活二
	INIT(0x280, HU_A2);
	INIT(0x220, HU_A2);
	INIT(0x208, HU_A2);
	INIT(0x0a0, HU_A2);
	INIT(0xca0, HU_A2);  // 五格
	INIT(0x088, HU_A2);
	INIT(0xc88, HU_A2);  // 五格
	INIT(0x028, HU_A2);
	INIT(0xc28, HU_A2);  // 五格

	//AI活一
	INIT(0x100, AI_A1);
	INIT(0x040, AI_A1);
	INIT(0xc40, AI_A1);  // 五格
	INIT(0x010, AI_A1);
	INIT(0xc10, AI_A1);  // 五格
	INIT(0x004, AI_A1);
	INIT(0xc04, AI_A1);  // 五格

	//Man活一
	INIT(0x200, HU_A1);
	INIT(0x080, HU_A1);
	INIT(0xc80, HU_A1);  // 五格
	INIT(0x020, HU_A1);
	INIT(0xc20, HU_A1);  // 五格
	INIT(0x008, HU_A1);
	INIT(0xc08, HU_A1);  // 五格

	int i, x, y, ix, iy;
	int six_scanner[6];

	// 枚举six_scanner中出现的所有棋型，打表
	RUSH3(six_scanner[0]) RUSH3(six_scanner[1]) RUSH3(six_scanner[2])
	RUSH3(six_scanner[3]) RUSH3(six_scanner[4]) RUSH3(six_scanner[5])
	{
		x = y = ix = iy = 0;
		if( six_scanner[0] == AI_CHESS ) x++;
		else if( six_scanner[0] == H1_CHESS ) y++;
		if( six_scanner[1] == AI_CHESS ) x++,ix++;
		else if( six_scanner[1] == H1_CHESS ) y++,iy++;
		if( six_scanner[2] == AI_CHESS ) x++,ix++;
		else if( six_scanner[2] == H1_CHESS ) y++,iy++;
		if( six_scanner[3] == AI_CHESS ) x++,ix++;
		else if( six_scanner[3] == H1_CHESS ) y++,iy++;
		if( six_scanner[4] == AI_CHESS ) x++,ix++;
		else if( six_scanner[4] == H1_CHESS ) y++,iy++;
		if( six_scanner[5] == AI_CHESS ) ix++;
		else if( six_scanner[5] == H1_CHESS ) iy++;

		i = six_scanner[0] + (six_scanner[1]<<2) + (six_scanner[2]<<4) + (six_scanner[3]<<6) + (six_scanner[4]<<8) + (six_scanner[5]<<10);

		if( !M_TABLE[i] )
		{
			//AI冲四
			if( (x==4 && y==0) || (ix==4 && iy==0) )
			{
				for (int pos=0; pos<6; pos++)
				{
					if (six_scanner[pos] == NO_CHESS)
					{
						int cnt = 1, l = pos-1, r = pos+1;
						while (l>=0 && six_scanner[l] == AI_CHESS)
							--l, ++cnt;
						while ( r<6 && six_scanner[r] == AI_CHESS)
							++r, ++cnt;
						if (cnt == 5)
						{
							INIT(i, AI_P4, pos);
							break;
						}
					}
				}
			}

			//Man冲四
			else if( (x==0 && y==4) || (ix==0 && iy==4) )
			{
				for (int pos=0; pos<6; pos++)
				{
					if (six_scanner[pos] == NO_CHESS)
					{
						int cnt = 1, l = pos-1, r = pos+1;
						while (l>=0 && six_scanner[l] == H1_CHESS)
							--l, ++cnt;
						while ( r<6 && six_scanner[r] == H1_CHESS)
							++r, ++cnt;
						if (cnt == 5)
						{
							INIT(i, HU_P4, pos);
							break;
						}
					}
				}
			}

			//AI眠三
			else if( (x==3 && y==0) || (ix==3 && iy==0) )
				INIT(i, AI_S3);

			//Man眠三
			else if( (x==0 && y==3) || (ix==0 && iy==3) )
				INIT(i, HU_S3);

			//AI眠二
			else if( (x==2 && y==0) || (ix==2 && iy==0) )
				INIT(i, AI_S2);

			//Man眠二
			else if( (x==0 && y==2) || (ix==0 && iy==2) )
				INIT(i, HU_S2);
		}
	}

	int five_scanner[5];

	// 枚举five_scanner中出现的所有棋型，打表
	RUSH3(five_scanner[0]) RUSH3(five_scanner[1]) RUSH3(five_scanner[2])
	RUSH3(five_scanner[3]) RUSH3(five_scanner[4])
	{
		x = y = ix = iy = 0;
		if( five_scanner[0] == AI_CHESS ) x++;
		else if( five_scanner[0] == H1_CHESS ) y++;
		if( five_scanner[1] == AI_CHESS ) x++,ix++;
		else if( five_scanner[1] == H1_CHESS ) y++,iy++;
		if( five_scanner[2] == AI_CHESS ) x++,ix++;
		else if( five_scanner[2] == H1_CHESS ) y++,iy++;
		if( five_scanner[3] == AI_CHESS ) x++,ix++;
		else if( five_scanner[3] == H1_CHESS ) y++,iy++;
		if( five_scanner[4] == AI_CHESS ) ix++;
		else if( five_scanner[4] == H1_CHESS ) iy++;

		i = five_scanner[0] + (five_scanner[1]<<2) + (five_scanner[2]<<4) + (five_scanner[3]<<6) + (five_scanner[4]<<8) + (OUT_CHESS<<10);

		if( !M_TABLE[i] )
		{
			//AI冲四
			if( (x==3 && y==0) || (ix==3 && iy==0) )
			{
				for (int pos=0; pos<5; pos++)
				{
					if (five_scanner[pos] == NO_CHESS)
					{
						int cnt = 1, l = pos-1, r = pos+1;
						while (l>=0 && five_scanner[l] == AI_CHESS)
							--l, ++cnt;
						while ( r<6 && five_scanner[r] == AI_CHESS)
							++r, ++cnt;
						if (cnt == 5)
						{
							INIT(i, AI_P4, pos);
							break;
						}
					}
				}
			}

			//Man冲四
			else if( (x==0 && y==3) || (ix==0 && iy==3) )
			{
				for (int pos=0; pos<5; pos++)
				{
					if (five_scanner[pos] == NO_CHESS)
					{
						int cnt = 1, l = pos-1, r = pos+1;
						while (l>=0 && five_scanner[l] == H1_CHESS)
							--l, ++cnt;
						while ( r<6 && five_scanner[r] == H1_CHESS)
							++r, ++cnt;
						if (cnt == 5)
						{
							INIT(i, HU_P4, pos);
							break;
						}
					}
				}
			}

			//AI眠三
			else if( (x==2 && y==0) || (ix==2 && iy==0) )
				INIT(i, AI_S3);

			//Man眠三
			else if( (x==0 && y==2) || (ix==0 && iy==2) )
				INIT(i, HU_S3);

			//AI眠二
			else if( (x==1 && y==0) || (ix==1 && iy==0) )
				INIT(i, AI_S2);

			//Man眠二
			else if( (x==0 && y==1) || (ix==0 && iy==1) )
				INIT(i, HU_S2);
		}
	}
}

void AiThread::initCountTable()
{
	for (int x=0; x<GRID_N; ++x)
	{
		for (int y=0; y<GRID_N; ++y)
		{
			int l = x - y + 10;
			if (l>=0 && l<GRID_DN)
			{
				if (!l)
					XY_TO_LEFT_COUNT[x][y] = 1;
				else if (l<10)
					XY_TO_LEFT_COUNT[x][y] = l;
				else if (l<GRID_DN-1)
					XY_TO_LEFT_COUNT[x][y] = GRID_DN - l - 1;
				else
					XY_TO_LEFT_COUNT[x][y] = 1;
			}

			int r = x + y - 4;
			if (r>=0 && r<GRID_DN)
			{
				if (!r)
					XY_TO_RIGHT_COUNT[x][y] = 1;
				else if (r<10)
					XY_TO_RIGHT_COUNT[x][y] = r;
				else if (r<GRID_DN-1)
					XY_TO_RIGHT_COUNT[x][y] = GRID_DN - r - 1;
				else
					XY_TO_RIGHT_COUNT[x][y] = 1;
			}
		}
	}
}
