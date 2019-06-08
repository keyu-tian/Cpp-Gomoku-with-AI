#include "aithread.h"

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
	memf1(S3_KEY_POS_TABLE);

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

	// 注意：五格没有任何活棋型！

	//AI活三
	INIT(0x150, AI_A3, 1, 5, 0);	// .ooo..（右边是低位）
	INIT(0x144, AI_A3, 2, 5, 0);	// .oo.o.
	INIT(0x114, AI_A3, 3, 0, 5);	// .o.oo.
	INIT(0x054, AI_A3, 4, 0, 5);	// ..ooo.

	//Man活三
	INIT(0x2a0, HU_A3, 1, 5, 0);	// .xxx..
	INIT(0x288, HU_A3, 2, 5, 0);	// .xx.x.
	INIT(0x228, HU_A3, 3, 0, 5);	// .x.xx.
	INIT(0x0a8, HU_A3, 4, 0, 5);	// ..xxx.

	//AI活二
	INIT(0x140, AI_A2);
	INIT(0x110, AI_A2);
	INIT(0x104, AI_A2);
	INIT(0x050, AI_A2);
	INIT(0x044, AI_A2);
	INIT(0x014, AI_A2);

	//Man活二
	INIT(0x280, HU_A2);
	INIT(0x220, HU_A2);
	INIT(0x208, HU_A2);
	INIT(0x0a0, HU_A2);
	INIT(0x088, HU_A2);
	INIT(0x028, HU_A2);

	//AI活一
	INIT(0x100, AI_A1);
	INIT(0x040, AI_A1);
	INIT(0x010, AI_A1);
	INIT(0x004, AI_A1);

	//Man活一
	INIT(0x200, HU_A1);
	INIT(0x080, HU_A1);
	INIT(0x020, HU_A1);
	INIT(0x008, HU_A1);

	int scanner[6];
	int ai_l, hu_l, ai_r, hu_r;
	__int32 bits_line;
	int empty_pos[6], empty_cnt;

	// 枚举所有棋型，把没有INIT的给INIT
	RUSH(scanner[0], H1_CHESS) RUSH(scanner[1], H1_CHESS) RUSH(scanner[2], H1_CHESS)
	RUSH(scanner[3], H1_CHESS) RUSH(scanner[4], H1_CHESS) RUSH(scanner[5], OUT_CHESS) // 注意scanner[5]的上界范围
	{
		empty_cnt = 0;
		ai_l = hu_l = ai_r = hu_r = 0;

		if (scanner[0] == AI_CHESS) ++ai_l;
		else if (scanner[0] == H1_CHESS) ++hu_l;
		else empty_pos[empty_cnt++] = 0;

		if (scanner[1] == AI_CHESS) ++ai_l,++ai_r;
		else if (scanner[1] == H1_CHESS) ++hu_l,++hu_r;
		else empty_pos[empty_cnt++] = 1;

		if (scanner[2] == AI_CHESS) ++ai_l,++ai_r;
		else if (scanner[2] == H1_CHESS) ++hu_l,++hu_r;
		else empty_pos[empty_cnt++] = 2;

		if (scanner[3] == AI_CHESS) ++ai_l,++ai_r;
		else if (scanner[3] == H1_CHESS) ++hu_l,++hu_r;
		else empty_pos[empty_cnt++] = 3;

		if (scanner[4] == AI_CHESS) ++ai_l,++ai_r;
		else if (scanner[4] == H1_CHESS) ++hu_l,++hu_r;
		else empty_pos[empty_cnt++] = 4;

		if (scanner[5] == AI_CHESS) ++ai_r;
		else if (scanner[5] == H1_CHESS) ++hu_r;
		else if (scanner[5] == OUT_CHESS) ++ai_r, ++hu_r;
		else empty_pos[empty_cnt++] = 5;

		bits_line = scanner[0] + (scanner[1]<<2) + (scanner[2]<<4) + (scanner[3]<<6) + (scanner[4]<<8) + (scanner[5]<<10);

		if (!M_TABLE[bits_line])
		{
			// AI冲四
			if ( (ai_l==4 && hu_l==0) || (ai_r==4 && hu_r==0) )
			{
				if (empty_cnt == 1)
					INIT(bits_line, AI_P4, empty_pos[0]);
				else // empty_cnt == 2
				{
					if (scanner[0] == AI_CHESS)
						INIT(bits_line, AI_P4, empty_pos[0]);
					else
						INIT(bits_line, AI_P4, empty_pos[1]);
				}
			}

			// Man冲四
			else if ( (ai_l==0 && hu_l==4) || (ai_r==0 && hu_r==4) )
			{
				if (empty_cnt == 1)
					INIT(bits_line, HU_P4, empty_pos[0]);
				else // empty_cnt == 2
				{
					if (scanner[0] == H1_CHESS)
						INIT(bits_line, HU_P4, empty_pos[0]);
					else
						INIT(bits_line, HU_P4, empty_pos[1]);
				}
			}

			// AI眠三
			else if ( (ai_l==3 && hu_l==0) || (ai_r==3 && hu_r==0) )
			{
				if (empty_cnt == 2)
					INIT(bits_line, AI_S3, empty_pos[0], empty_pos[1]);
				else // empty_cnt == 3
				{
					if (scanner[0] == AI_CHESS)
						INIT(bits_line, AI_S3, empty_pos[0], empty_pos[1]);
					else
						INIT(bits_line, AI_S3, empty_pos[2], empty_pos[1]);
				}
			}

			// Man眠三
			else if ( (ai_l==0 && hu_l==3) || (ai_r==0 && hu_r==3) )
			{
				if (empty_cnt == 2)
					INIT(bits_line, HU_S3, empty_pos[0], empty_pos[1]);
				else // empty_cnt == 3
				{
					if (scanner[0] == H1_CHESS)
						INIT(bits_line, HU_S3, empty_pos[0], empty_pos[1]);
					else
						INIT(bits_line, HU_S3, empty_pos[2], empty_pos[1]);
				}
			}

			// AI眠二
			else if ( (ai_l==2 && hu_l==0) || (ai_r==2 && hu_r==0) )
				INIT(bits_line, AI_S2);

			// Man眠二
			else if ( (ai_l==0 && hu_l==2) || (ai_r==0 && hu_r==2) )
				INIT(bits_line, HU_S2);
		}
	}
}


void AiThread::initCountTable()
{
	memset(XY_TO_LEFT_COUNT, 0, sizeof(XY_TO_LEFT_COUNT));
	memset(XY_TO_RIGHT_COUNT, 0, sizeof(XY_TO_RIGHT_COUNT));
	memset(L_TO_LEFT_COUNT, 0, sizeof(L_TO_LEFT_COUNT));
	memset(R_TO_RIGHT_COUNT, 0, sizeof(R_TO_RIGHT_COUNT));

	for (int x=0; x<GRID_N; ++x)
	{
		for (int y=0; y<GRID_N; ++y)
		{
			int l = x - y + 10;
			if (l>=0 && l<GRID_DN)
			{
				if (!l)
					XY_TO_LEFT_COUNT[x][y] = L_TO_LEFT_COUNT[l] = 1;
				else if (l<10)
					XY_TO_LEFT_COUNT[x][y] = L_TO_LEFT_COUNT[l] = l;
				else if (l<GRID_DN-1)
					XY_TO_LEFT_COUNT[x][y] = L_TO_LEFT_COUNT[l] = GRID_DN - l - 1;
				else
					XY_TO_LEFT_COUNT[x][y] = L_TO_LEFT_COUNT[l] = 1;
			}

			int r = x + y - 4;
			if (r>=0 && r<GRID_DN)
			{
				if (!r)
					XY_TO_RIGHT_COUNT[x][y] = R_TO_RIGHT_COUNT[r] = 1;
				else if (r<10)
					XY_TO_RIGHT_COUNT[x][y] = R_TO_RIGHT_COUNT[r] = r;
				else if (r<GRID_DN-1)
					XY_TO_RIGHT_COUNT[x][y] = R_TO_RIGHT_COUNT[r] = GRID_DN - r - 1;
				else
					XY_TO_RIGHT_COUNT[x][y] = R_TO_RIGHT_COUNT[r] = 1;
			}
		}
	}
}
