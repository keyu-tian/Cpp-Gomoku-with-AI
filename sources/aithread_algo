#include "aithread.h"

#define BASE_DEPTH 4
#define KILL_DEPTH 11
#define KILL_NUM   20	// 小于16会出问题！

#define swapChoice(x, y) do{Choice tp=x; x=y; y=tp;}while(0)

int AiThread::minMaxSearch(Chessid cur_player, int depth, int alpha, int beta)
{
	if ( depth )				// depth必须>0，否则无法获取必杀点的坐标就返回了
	{
		int killed_score = evalKilledScore(cur_player, depth);
		if (killed_score)
		{
			return killed_score;	// 找到必杀棋，中断极大极小搜索的迭代加深，进行剪枝
		}
	}

	if ( depth >= BASE_DEPTH )		// 超过基本搜索最大深度，进行算杀
	{
		return killSearch(cur_player, depth, alpha, beta);
	}

	int score;

	Choice choices_buffer[GRID_N*GRID_N];
	Choice *p_now = choices_buffer, *p_end = choices_buffer + getMinMaxSearchChoices(choices_buffer, cur_player);
	int x, y;

	// 继续进行极大极小搜索
	if ( cur_player == AI_CHESS )
	{
		while (p_now != p_end)
		{
			x = p_now->x;
			y = p_now->y;

			putChess(x, y, AI_CHESS);	// 扩展子节点
			score = minMaxSearch(H1_CHESS, depth+1, alpha, beta); // 递归进行极大极小搜索
			takeChess(x, y);		// 回溯

			if (score > alpha)		// 对于AI，更新极大值
			{
				if (!depth)		// 在第0层进行搜索时，还需同时更新最优选择的坐标
				{
					ai_next_move.x = x;
					ai_next_move.y = y;
				}
				alpha = score;
			}
			if (alpha >= beta)   		// 进行 Alpha 剪枝
			{
				return alpha;
			}

			if (depth && alpha >= C5score)	// 必胜剪枝
				return alpha;

			++p_now;
		}
		return alpha;				// AI从所有落子情况中选择分数最高值
	}
	else // if ( NowPlayer == H1_CHESS )
	{
		while (p_now != p_end)
		{
			x = p_now->x;
			y = p_now->y;

			putChess(x, y, H1_CHESS);	// 扩展子节点
			score = minMaxSearch(AI_CHESS, depth+1, alpha, beta); // 递归进行极大极小搜索
			takeChess(x, y);		// 回溯

			if (score < beta)		// 对于玩家，更新极小值
			{
				beta = score;
			}
			if (alpha >= beta)   		// 进行 Beta 剪枝
			{
				return beta;
			}

			if (beta <= -C5score)		// 必胜剪枝
				return beta;

			++p_now;
		}
		return beta;				// 玩家从所有落子情况中选择分数最低值
	}
}



int AiThread::killSearch(Chessid cur_player, int depth, int alpha, int beta)
{
	int killed_score = evalKilledScore(cur_player, depth);
	if (killed_score)
	{
		return killed_score;			// 找到必杀棋，中断算杀的迭代加深，进行剪枝
	}

	if ( depth >= KILL_DEPTH )			// 超过算杀搜索最大深度，终止算杀的迭代加深，返回局面估值
	{
		return evalBoard();
	}

	int score, buffer_cnt;

	Choice choices_buffer[GRID_N*GRID_N];
	buffer_cnt = getKillSearchChoices(choices_buffer, cur_player);
	if (!buffer_cnt)				// 找不到杀棋，终止算杀搜索，返回局面估值
	{
		return evalBoard();
	}

	Choice *p_now = choices_buffer, *p_end = choices_buffer + buffer_cnt;
	int x, y;

	// 继续进行算杀搜索
	if ( cur_player == AI_CHESS )
	{
		while (p_now != p_end)
		{
			x = p_now->x;
			y = p_now->y;

			putChess(x, y, AI_CHESS);	// 扩展子节点
			score = killSearch(H1_CHESS, depth+1, alpha, beta); // 递归进行算杀搜索
			takeChess(x, y);		// 回溯

			if (score > alpha)		// 对于AI，更新极大值
			{
				alpha = score;
			}
			if (alpha >= beta)   		// 进行 Alpha 剪枝
			{
				return alpha;
			}
			if (alpha >= C5score)
				return alpha;
			++p_now;
		}
		return alpha;				// AI从所有落子情况中选择分数最高值
	}
	else // if ( NowPlayer == H1_CHESS )
	{
		while (p_now != p_end)
		{
			x = p_now->x;
			y = p_now->y;

			putChess(x, y, H1_CHESS);	// 扩展子节点
			score = killSearch(AI_CHESS, depth+1, alpha, beta); // 递归进行算杀搜索
			takeChess(x, y);		// 回溯

			if (score < beta)		// 对于玩家，更新极小值
			{
				beta = score ;
			}
			if (alpha >= beta)   		// 进行 Beta 剪枝
			{
				return beta ;
			}
			if (beta <= -C5score)
				return beta;
			++p_now;
		}
		return beta;				// 玩家从所有落子情况中选择分数最低值
	}
}



int AiThread::getMinMaxSearchChoices(Choice *choices_buffer, Chessid cur_player)
{
	if (cur_player == AI_CHESS && all_type[I_HU_P4])
	{
		for (int c=0; c<GRID_N; c++)
		{
			if (col_type[c] & F_HU_P4)  // 在第c列发现了冲四
			{
				for (__int32 temp_col_chess = col_chess[c], i=0; i<10; ++i, temp_col_chess>>=2)
				{
					if (M_TABLE[MASK & temp_col_chess] == F_HU_P4)  // 第i次扫描发现了冲四
					{
						choices_buffer->x = c;
						choices_buffer->y = i + P4_KEY_POS_TABLE[MASK & temp_col_chess];
						return 1; // 唯一的位置
					}
				}
			}
		}
		for (int r=0; r<GRID_N; r++)
		{
			if (row_type[r] & F_HU_P4)  // 在第r行找到了冲四
			{
				for (__int32 temp_row_chess = row_chess[r], i=0; i<10; ++i, temp_row_chess>>=2)
				{
					if (M_TABLE[MASK & temp_row_chess] == F_HU_P4)  // 第i次扫描发现了冲四
					{
						choices_buffer->x = i + P4_KEY_POS_TABLE[MASK & temp_row_chess];
						choices_buffer->y = r;
						return 1; // 唯一的位置
					}
				}
			}
		}
	}// else 出了bug
	else if (cur_player == H1_CHESS && all_type[I_AI_P4])
	{
		for (int c=0; c<GRID_N; c++)
		{
			if (col_type[c] & F_AI_P4)  // 在第c列发现了冲四
			{
				for (__int32 temp_col_chess = col_chess[c], i=0; i<10; ++i, temp_col_chess>>=2)
				{
					if (M_TABLE[MASK & temp_col_chess] == F_AI_P4)  // 第i次扫描发现了冲四
					{
						choices_buffer->x = c;
						choices_buffer->y = i + P4_KEY_POS_TABLE[MASK & temp_col_chess];
						return 1; // 唯一的位置
					}
				}
			}
		}
		for (int r=0; r<GRID_N; r++)
		{
			if (row_type[r] & F_AI_P4)  // 在第r行找到了冲四
			{
				for (__int32 temp_row_chess = row_chess[r], i=0; i<10; ++i, temp_row_chess>>=2)
				{
					if (M_TABLE[MASK & temp_row_chess] == F_AI_P4)  // 第i次扫描发现了冲四
					{
						choices_buffer->x = i + P4_KEY_POS_TABLE[MASK & temp_row_chess];
						choices_buffer->y = r;
						return 1; // 唯一的位置
					}
				}
			}
		}
	}
	else if (cur_player == AI_CHESS && all_type[I_HU_A3] && !all_type[I_AI_A3] && !all_type[I_AI_S3])
	{
		for (int c=0; c<GRID_N; c++)
		{
			if (col_type[c] & F_HU_A3)  // 在第c列发现了冲四
			{
				for (__int32 temp_col_chess = col_chess[c], i=0; i<10; ++i, temp_col_chess>>=2)
				{
					if (M_TABLE[MASK & temp_col_chess] == F_HU_A3)  // 第i次扫描发现了冲四
					{
						choices_buffer->x = c;
						choices_buffer->y = i + A3_KEY_POS_TABLE[MASK & temp_col_chess][0];
						choices_buffer[1].x = c;
						choices_buffer[1].y = i + A3_KEY_POS_TABLE[MASK & temp_col_chess][1];
						choices_buffer[2].x = c;
						choices_buffer[2].y = i + A3_KEY_POS_TABLE[MASK & temp_col_chess][2];
						return 3; // 唯一的位置
					}
				}
			}
		}
		for (int r=0; r<GRID_N; r++)
		{
			if (row_type[r] & F_HU_A3)  // 在第r行找到了冲四
			{
				for (__int32 temp_row_chess = row_chess[r], i=0; i<10; ++i, temp_row_chess>>=2)
				{
					if (M_TABLE[MASK & temp_row_chess] == F_HU_A3)  // 第i次扫描发现了冲四
					{
						choices_buffer->x = i + A3_KEY_POS_TABLE[MASK & temp_row_chess][0];
						choices_buffer->y = r;
						choices_buffer[1].x = i + A3_KEY_POS_TABLE[MASK & temp_row_chess][1];
						choices_buffer[1].y = r;
						choices_buffer[2].x = i + A3_KEY_POS_TABLE[MASK & temp_row_chess][2];
						choices_buffer[2].y = r;
						return 3; // 唯一的位置
					}
				}
			}
		}
	}
	else if (cur_player == H1_CHESS && all_type[I_AI_A3] && !all_type[I_HU_A3] && !all_type[I_HU_S3])
	{
		for (int c=0; c<GRID_N; c++)
		{
			if (col_type[c] & F_AI_A3)  // 在第c列发现了冲四
			{
				for (__int32 temp_col_chess = col_chess[c], i=0; i<10; ++i, temp_col_chess>>=2)
				{
					if (M_TABLE[MASK & temp_col_chess] == F_AI_A3)  // 第i次扫描发现了冲四
					{
						choices_buffer->x = c;
						choices_buffer->y = i + A3_KEY_POS_TABLE[MASK & temp_col_chess][0];
						choices_buffer[1].x = c;
						choices_buffer[1].y = i + A3_KEY_POS_TABLE[MASK & temp_col_chess][1];
						choices_buffer[2].x = c;
						choices_buffer[2].y = i + A3_KEY_POS_TABLE[MASK & temp_col_chess][2];
						return 3; // 唯一的位置
					}
				}
			}
		}
		for (int r=0; r<GRID_N; r++)
		{
			if (row_type[r] & F_AI_A3)  // 在第r行找到了冲四
			{
				for (__int32 temp_row_chess = row_chess[r], i=0; i<10; ++i, temp_row_chess>>=2)
				{
					if (M_TABLE[MASK & temp_row_chess] == F_AI_A3)  // 第i次扫描发现了冲四
					{
						choices_buffer->x = i + A3_KEY_POS_TABLE[MASK & temp_row_chess][0];
						choices_buffer->y = r;
						choices_buffer[1].x = i + A3_KEY_POS_TABLE[MASK & temp_row_chess][1];
						choices_buffer[1].y = r;
						choices_buffer[2].x = i + A3_KEY_POS_TABLE[MASK & temp_row_chess][2];
						choices_buffer[2].y = r;
						return 3; // 唯一的位置
					}
				}
			}
		}
	}
//	else
	{
		int i, j, k, l, cnt, kill_cnt;
		bool not_found;
		for (i=cnt=0; i<GRID_N; i++)
		{
			for (j=0; j<GRID_N; j++)
			{
				if ( mirror_board[i][j] == NO_CHESS )
				{
					not_found = true;
					for (k=i-2; k<=i+2&&not_found; k++)
					{
						for (l=j-2; l<=j+2&&not_found; l++)
						{
							if ( k>=0&&k<GRID_N && l>=0&&l<GRID_N && mirror_board[k][l] )
							{
								choices_buffer[cnt].y = i;
								choices_buffer[cnt].x = j;
								choices_buffer[cnt].prior =
										evalMinMaxPrior(j, i, AI_CHESS)
										+ evalMinMaxPrior(j, i, H1_CHESS);
								++cnt;
								not_found = false;
							}
						}
					}
				}
			}
		}

		myQuickSort(choices_buffer, cnt);

		for (i=kill_cnt=0; i<cnt; i++)
		{
			if (choices_buffer[i].prior >= KILL_PRIOR)
				++kill_cnt;
			else break;
		}

		if( kill_cnt>=3 && kill_cnt + KILL_NUM < cnt )			  // 出现杀棋时 进行剪枝
		{
			cnt = kill_cnt + KILL_NUM ;
		}

		if(choices_buffer->prior >= 16384)
			cnt = 1;

		return cnt;
	}
}


int AiThread::getKillSearchChoices(Choice *choices_buffer, Chessid cur_player)
{
	if (cur_player == AI_CHESS && all_type[I_HU_P4])
	{
		for (int c=0; c<GRID_N; c++)
		{
			if (col_type[c] & F_HU_P4)  // 在第c列发现了冲四
			{
				for (__int32 temp_col_chess = col_chess[c], i=0; i<10; ++i, temp_col_chess>>=2)
				{
					if (M_TABLE[MASK & temp_col_chess] == F_HU_P4)  // 第i次扫描发现了冲四
					{
						choices_buffer->x = c;
						choices_buffer->y = i + P4_KEY_POS_TABLE[MASK & temp_col_chess];
						return 1; // 唯一的位置
					}
				}
			}
		}
		for (int r=0; r<GRID_N; r++)
		{
			if (row_type[r] & F_HU_P4)  // 在第r行找到了冲四
			{
				for (__int32 temp_row_chess = row_chess[r], i=0; i<10; ++i, temp_row_chess>>=2)
				{
					if (M_TABLE[MASK & temp_row_chess] == F_HU_P4)  // 第i次扫描发现了冲四
					{
						choices_buffer->x = i + P4_KEY_POS_TABLE[MASK & temp_row_chess];
						choices_buffer->y = r;
						return 1; // 唯一的位置
					}
				}
			}
		}
	}
	else if (cur_player == H1_CHESS && all_type[I_AI_P4])
	{
		for (int c=0; c<GRID_N; c++)
		{
			if (col_type[c] & F_AI_P4)  // 在第c列发现了冲四
			{
				for (__int32 temp_col_chess = col_chess[c], i=0; i<10; ++i, temp_col_chess>>=2)
				{
					if (M_TABLE[MASK & temp_col_chess] == F_AI_P4)  // 第i次扫描发现了冲四
					{
						choices_buffer->x = c;
						choices_buffer->y = i + P4_KEY_POS_TABLE[MASK & temp_col_chess];
						return 1; // 唯一的位置
					}
				}
			}
		}
		for (int r=0; r<GRID_N; r++)
		{
			if (row_type[r] & F_AI_P4)  // 在第r行找到了冲四
			{
				for (__int32 temp_row_chess = row_chess[r], i=0; i<10; ++i, temp_row_chess>>=2)
				{
					if (M_TABLE[MASK & temp_row_chess] == F_AI_P4)  // 第i次扫描发现了冲四
					{
						choices_buffer->x = i + P4_KEY_POS_TABLE[MASK & temp_row_chess];
						choices_buffer->y = r;
						return 1; // 唯一的位置
					}
				}
			}
		}
	}
	else if (cur_player == AI_CHESS && all_type[I_HU_A3] && !all_type[I_AI_A3] && !all_type[I_AI_S3])
	{
		for (int c=0; c<GRID_N; c++)
		{
			if (col_type[c] & F_HU_A3)  // 在第c列发现了冲四
			{
				for (__int32 temp_col_chess = col_chess[c], i=0; i<10; ++i, temp_col_chess>>=2)
				{
					if (M_TABLE[MASK & temp_col_chess] == F_HU_A3)  // 第i次扫描发现了冲四
					{
						choices_buffer->x = c;
						choices_buffer->y = i + A3_KEY_POS_TABLE[MASK & temp_col_chess][0];
						choices_buffer[1].x = c;
						choices_buffer[1].y = i + A3_KEY_POS_TABLE[MASK & temp_col_chess][1];
						choices_buffer[2].x = c;
						choices_buffer[2].y = i + A3_KEY_POS_TABLE[MASK & temp_col_chess][2];
						return 3; // 唯一的位置
					}
				}
			}
		}
		for (int r=0; r<GRID_N; r++)
		{
			if (row_type[r] & F_HU_A3)  // 在第r行找到了冲四
			{
				for (__int32 temp_row_chess = row_chess[r], i=0; i<10; ++i, temp_row_chess>>=2)
				{
					if (M_TABLE[MASK & temp_row_chess] == F_HU_A3)  // 第i次扫描发现了冲四
					{
						choices_buffer->x = i + A3_KEY_POS_TABLE[MASK & temp_row_chess][0];
						choices_buffer->y = r;
						choices_buffer[1].x = i + A3_KEY_POS_TABLE[MASK & temp_row_chess][1];
						choices_buffer[1].y = r;
						choices_buffer[2].x = i + A3_KEY_POS_TABLE[MASK & temp_row_chess][2];
						choices_buffer[2].y = r;
						return 3; // 唯一的位置
					}
				}
			}
		}
	}// else 出了bug
	else if (cur_player == H1_CHESS && all_type[I_AI_A3] && !all_type[I_HU_A3] && !all_type[I_HU_S3])
	{
		for (int c=0; c<GRID_N; c++)
		{
			if (col_type[c] & F_AI_A3)  // 在第c列发现了冲四
			{
				for (__int32 temp_col_chess = col_chess[c], i=0; i<10; ++i, temp_col_chess>>=2)
				{
					if (M_TABLE[MASK & temp_col_chess] == F_AI_A3)  // 第i次扫描发现了冲四
					{
						choices_buffer->x = c;
						choices_buffer->y = i + A3_KEY_POS_TABLE[MASK & temp_col_chess][0];
						choices_buffer[1].x = c;
						choices_buffer[1].y = i + A3_KEY_POS_TABLE[MASK & temp_col_chess][1];
						choices_buffer[2].x = c;
						choices_buffer[2].y = i + A3_KEY_POS_TABLE[MASK & temp_col_chess][2];
						return 3; // 唯一的位置
					}
				}
			}
		}
		for (int r=0; r<GRID_N; r++)
		{
			if (row_type[r] & F_AI_A3)  // 在第r行找到了冲四
			{
				for (__int32 temp_row_chess = row_chess[r], i=0; i<10; ++i, temp_row_chess>>=2)
				{
					if (M_TABLE[MASK & temp_row_chess] == F_AI_A3)  // 第i次扫描发现了冲四
					{
						choices_buffer->x = i + A3_KEY_POS_TABLE[MASK & temp_row_chess][0];
						choices_buffer->y = r;
						choices_buffer[1].x = i + A3_KEY_POS_TABLE[MASK & temp_row_chess][1];
						choices_buffer[1].y = r;
						choices_buffer[2].x = i + A3_KEY_POS_TABLE[MASK & temp_row_chess][2];
						choices_buffer[2].y = r;
						return 3; // 唯一的位置
					}
				}
			}
		}
	}
	{
		int i, j, k, l, cnt, kill_prior;
		bool not_found;
		for (i=cnt=0; i<GRID_N; i++)
		{
			for (j=0; j<GRID_N; j++)
			{
				if ( mirror_board[i][j]==NO_CHESS )
				{
					not_found = true;
					for (k=i-2; k<=i+2&&not_found; k++)
					{
						for (l=j-2; l<=j+2&&not_found; l++)
						{
							if ( k>=0&&k<GRID_N && l>=0&&l<GRID_N && mirror_board[k][l] )
							{
								kill_prior = evalKillPrior(j, i, AI_CHESS) + evalKillPrior(j, i, H1_CHESS);
								if (kill_prior)
								{
									choices_buffer[cnt].y = i;
									choices_buffer[cnt].x = j;
									choices_buffer[cnt++].prior = kill_prior;
								}
								not_found = false;
							}
						}
					}
				}
			}
		}

		if (cnt)
		{
			myQuickSort(choices_buffer, cnt);
			if(choices_buffer->prior >= 100)
				cnt = 1;
		}

		return cnt;
	}
}

void AiThread::myQuickSort(Choice *a, int n)
{
	if (n <= 1)
		return;

	if (n < 6)
	{
		for (int i=1; i<n; i++)
		{
			int j = i;
			Choice tp = a[j];
			while (j && tp.prior > a[j-1].prior)
				a[j] = a[j-1], --j;
			a[j] = tp;
		}
		return;
	}

	int mid = n / 2;
	if (a[0].prior < a[mid].prior)
		swapChoice(a[0], a[mid]);
	if (a[0].prior < a[n-1].prior)
		swapChoice(a[0], a[n-1]);
	if (a[mid].prior < a[n-1].prior)
		swapChoice(a[mid], a[n-1]);
	swapChoice(a[mid], a[n-2]);
	int pivot = a[n-2].prior;

	int i = 0, j = n-2;
	while(1)
	{
		while ( a[++i].prior > pivot );
		while ( a[--j].prior < pivot );
		if (i<j)
			swapChoice(a[i], a[j]);
		else break;
	}
	swapChoice(a[i], a[n-2]);
	myQuickSort(a, i);
	myQuickSort(a+i+1, n-i-1);
}
