#include "aithread.h"

#define BASE_DEPTH	4
#define KILL_DEPTH	9
#define RES_DEPTH	11
#define NEAR_CNT	2

#define __OPEN_EP4	// 敌方冲四优化（在必胜判断函数之后，如果没有立即返回说明不存在必胜。此时如果对方有P4则必只有一个P4，并且我方必须防守。因为我方没有4。）
#define __OPEN_SA3	// 眠活三优化（当对方没有P4而有活三时，我方只能走：①对方活三的前二防守点 ②我方眠三进攻点 ③对方活三的第三防守点）

// 可能存在的必杀：执黑，按顺序落子：(10,11)(10,10)(9,10)(9,9)(8,9)

#define swapChoice(x, y) do{Choice tp=x; x=y; y=tp;}while(0)


int AiThread::minMaxSearch(const Chessid cur_player, const int depth, int alpha, int beta)
{
	if ( depth )					// depth必须>0，否则无法获取必杀点的坐标就返回了
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

	Choice choices_buffer[CHOICE_BUF_SIZE];
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
			takeChess(x, y);			// 回溯

			if (score > alpha)			// 对于AI，更新极大值
			{
				if (!depth)				// 在第0层进行搜索时，还需同时更新最优选择的坐标
				{
					ai_next_move.x = x;
					ai_next_move.y = y;

					emit findingNextMove(ai_next_move);
				}
				alpha = score;
			}
			if (alpha >= beta)   		// 进行 Alpha 剪枝
			{
				return alpha;
			}

			if (depth && alpha >= C5score)		// 必胜剪枝
				return alpha;

			++p_now;
		}
		return alpha;					// AI从所有落子情况中选择分数最高值
	}
	else // if ( cur_player == H1_CHESS )
	{
		while (p_now != p_end)
		{
			x = p_now->x;
			y = p_now->y;

			putChess(x, y, H1_CHESS);	// 扩展子节点
			score = minMaxSearch(AI_CHESS, depth+1, alpha, beta); // 递归进行极大极小搜索
			takeChess(x, y);			// 回溯

			if (score < beta)			// 对于玩家，更新极小值
			{
				beta = score;
			}
			if (alpha >= beta)   		// 进行 Beta 剪枝
			{
				return beta;
			}

			if (beta <= -C5score)		// 必败剪枝
				return beta;

			++p_now;
		}
		return beta;					// 玩家从所有落子情况中选择分数最低值
	}
}


int AiThread::killSearch(const Chessid cur_player, const int depth, int alpha, int beta)
{
	int killed_score = evalKilledScore(cur_player, depth);
	if (killed_score)
	{
		return killed_score;			// 找到必杀棋，中断算杀的迭代加深，进行剪枝
	}

	if ( depth >= KILL_DEPTH )			// 超过算杀搜索最大深度，终止算杀的迭代加深，返回局面估值
	{
#if RES_DEPTH > KILL_DEPTH
		return resSearch(cur_player, depth, alpha, beta);
#else
		return evalBoard();
#endif
	}

	int score, buffer_cnt;

	Choice choices_buffer[CHOICE_BUF_SIZE];
	buffer_cnt = getKillSearchChoices(choices_buffer, cur_player);
	if (!buffer_cnt)					// 找不到杀棋，终止算杀搜索，返回局面估值
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
			takeChess(x, y);			// 回溯

			if (score > alpha)			// 对于AI，更新极大值
			{
				alpha = score;
			}
			if (alpha >= beta)   		// 进行 Alpha 剪枝
			{
				return alpha;
			}
			if (alpha >= C5score)		// 必胜剪枝
				return alpha;
			++p_now;
		}
		return alpha;					// AI从所有落子情况中选择分数最高值
	}
	else // if ( cur_player == H1_CHESS )
	{
		while (p_now != p_end)
		{
			x = p_now->x;
			y = p_now->y;

			putChess(x, y, H1_CHESS);	// 扩展子节点
			score = killSearch(AI_CHESS, depth+1, alpha, beta); // 递归进行算杀搜索
			takeChess(x, y);			// 回溯

			if (score < beta)			// 对于玩家，更新极小值
			{
				beta = score ;
			}
			if (alpha >= beta)   		// 进行 Beta 剪枝
			{
				return beta ;
			}
			if (beta <= -C5score)		// 必败剪枝
				return beta;
			++p_now;
		}
		return beta;					// 玩家从所有落子情况中选择分数最低值
	}
}


int AiThread::resSearch(const Chessid cur_player, const int depth, int alpha, int beta)
{
	int killed_score = evalKilledScore(cur_player, depth);
	if (killed_score)
	{
		return killed_score;			// 找到必杀棋，中断算杀的迭代加深，进行剪枝
	}

	if ( depth >= RES_DEPTH )			// 超过算杀搜索最大深度，终止算杀的迭代加深，返回局面估值
	{
		return evalBoard();
	}

	int score, buffer_cnt = 0;

	Choice choices_buffer[CHOICE_BUF_SIZE];
	buffer_cnt = this->findEnemyP4(choices_buffer, cur_player);
	if (!buffer_cnt)
		buffer_cnt = this->findS3A3(choices_buffer, cur_player);
	if (!buffer_cnt)					// 找不到杀棋，终止算杀搜索，返回局面估值
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
			score = resSearch(H1_CHESS, depth+1, alpha, beta); // 递归进行算杀搜索
			takeChess(x, y);			// 回溯

			if (score > alpha)			// 对于AI，更新极大值
			{
				alpha = score;
			}
			if (alpha >= beta)   		// 进行 Alpha 剪枝
			{
				return alpha;
			}
			if (alpha >= C5score)		// 必胜剪枝
				return alpha;
			++p_now;
		}
		return alpha;					// AI从所有落子情况中选择分数最高值
	}
	else // if ( cur_player == H1_CHESS )
	{
		while (p_now != p_end)
		{
			x = p_now->x;
			y = p_now->y;

			putChess(x, y, H1_CHESS);	// 扩展子节点
			score = resSearch(AI_CHESS, depth+1, alpha, beta); // 递归进行算杀搜索
			takeChess(x, y);			// 回溯

			if (score < beta)			// 对于玩家，更新极小值
			{
				beta = score ;
			}
			if (alpha >= beta)   		// 进行 Beta 剪枝
			{
				return beta ;
			}
			if (beta <= -C5score)		// 必败剪枝
				return beta;
			++p_now;
		}
		return beta;					// 玩家从所有落子情况中选择分数最低值
	}
}


int AiThread::findEnemyP4(Choice *choices_buffer, const Chessid cur_player) const
{
	if (cur_player == AI_CHESS && all_type[I_HU_P4])
	{
		for (int c=0; c<GRID_N; ++c)
		{
			if (col_type[c] & F_HU_P4)  // 在第c列发现了冲四
			{
				for (__int32 temp_col_chess = col_chess[c], i=0; i<10; ++i, temp_col_chess>>=2)
				{
					if (M_TABLE[MASK & temp_col_chess] == F_HU_P4)  // 第i次扫描发现了冲四
					{
						choices_buffer->x = c;
						choices_buffer->y = i + P4_KEY_POS_TABLE[MASK & temp_col_chess];
						return 1; // 唯一P4
					}
				}
			}
		}
		for (int r=0; r<GRID_N; ++r)
		{
			if (row_type[r] & F_HU_P4)  // 在第r行找到了冲四
			{
				for (__int32 temp_row_chess = row_chess[r], i=0; i<10; ++i, temp_row_chess>>=2)
				{
					if (M_TABLE[MASK & temp_row_chess] == F_HU_P4)  // 第i次扫描发现了冲四
					{
						choices_buffer->x = i + P4_KEY_POS_TABLE[MASK & temp_row_chess];
						choices_buffer->y = r;
						return 1; // 唯一P4
					}
				}
			}
		}
		for (int l=0; l<GRID_DN; ++l) // l + y = x + 10
		{
			if (left_type[l] & F_HU_P4)
			{
				for (__int32 temp_left_chess = left_chess[l], i=0, count=getLeftCount(l); i<count; ++i, temp_left_chess>>=2)
				{
					if (M_TABLE[MASK & temp_left_chess] == F_HU_P4)
					{
						if (l <= 10)
						{
							choices_buffer->x = i + P4_KEY_POS_TABLE[MASK & temp_left_chess];
							choices_buffer->y = 10 - l + choices_buffer->x;
						}
						else
						{
							choices_buffer->y = i + P4_KEY_POS_TABLE[MASK & temp_left_chess];
							choices_buffer->x = l - 10 + choices_buffer->y;
						}

						return 1; // 唯一P4
					}
				}
			}
		}
		for (int r=0; r<GRID_DN; ++r) // r + 4 = x + y
		{
			if (right_type[r] & F_HU_P4)
			{
				for (__int32 temp_right_chess = right_chess[r], i=0, count=getRightCount(r); i<count; ++i, temp_right_chess>>=2)
				{
					if (M_TABLE[MASK & temp_right_chess] == F_HU_P4)
					{
						if (r <= 10)
						{
							choices_buffer->y = i + P4_KEY_POS_TABLE[MASK & temp_right_chess];
							choices_buffer->x = r + 4 - choices_buffer->y;
						}
						else
						{
							choices_buffer->x = 14 - i - P4_KEY_POS_TABLE[MASK & temp_right_chess];
							choices_buffer->y = r + 4 - choices_buffer->x;
						}

						return 1; // 唯一P4
					}
				}
			}
		}
	}
	else if (cur_player == H1_CHESS && all_type[I_AI_P4])
	{
		for (int c=0; c<GRID_N; ++c)
		{
			if (col_type[c] & F_AI_P4)  // 在第c列发现了冲四
			{
				for (__int32 temp_col_chess = col_chess[c], i=0; i<10; ++i, temp_col_chess>>=2)
				{
					if (M_TABLE[MASK & temp_col_chess] == F_AI_P4)  // 第i次扫描发现了冲四
					{
						choices_buffer->x = c;
						choices_buffer->y = i + P4_KEY_POS_TABLE[MASK & temp_col_chess];
						return 1; // 唯一P4
					}
				}
			}
		}
		for (int r=0; r<GRID_N; ++r)
		{
			if (row_type[r] & F_AI_P4)  // 在第r行找到了冲四
			{
				for (__int32 temp_row_chess = row_chess[r], i=0; i<10; ++i, temp_row_chess>>=2)
				{
					if (M_TABLE[MASK & temp_row_chess] == F_AI_P4)  // 第i次扫描发现了冲四
					{
						choices_buffer->x = i + P4_KEY_POS_TABLE[MASK & temp_row_chess];
						choices_buffer->y = r;
						return 1; // 唯一P4
					}
				}
			}
		}
		for (int l=0; l<GRID_DN; ++l) // l + y = x + 10
		{
			if (left_type[l] & F_AI_P4)
			{
				for (__int32 temp_left_chess = left_chess[l], i=0, count=getLeftCount(l); i<count; ++i, temp_left_chess>>=2)
				{
					if (M_TABLE[MASK & temp_left_chess] == F_AI_P4)
					{
						if (l <= 10)
						{
							choices_buffer->x = i + P4_KEY_POS_TABLE[MASK & temp_left_chess];
							choices_buffer->y = 10 - l + choices_buffer->x;
						}
						else
						{
							choices_buffer->y = i + P4_KEY_POS_TABLE[MASK & temp_left_chess];
							choices_buffer->x = l - 10 + choices_buffer->y;
						}

						return 1; // 唯一P4
					}
				}
			}
		}
		for (int r=0; r<GRID_DN; ++r) // r + 4 = x + y
		{
			if (right_type[r] & F_AI_P4)
			{
				for (__int32 temp_right_chess = right_chess[r], i=0, count=getRightCount(r); i<count; ++i, temp_right_chess>>=2)
				{
					if (M_TABLE[MASK & temp_right_chess] == F_AI_P4)
					{
						if (r <= 10)
						{
							choices_buffer->y = i + P4_KEY_POS_TABLE[MASK & temp_right_chess];
							choices_buffer->x = r + 4 - choices_buffer->y;
						}
						else
						{
							choices_buffer->x = 14 - i - P4_KEY_POS_TABLE[MASK & temp_right_chess];
							choices_buffer->y = r + 4 - choices_buffer->x;
						}

						return 1; // 唯一P4
					}
				}
			}
		}
	}

	return 0;
}


// 眠活三优化（当对方没有P4而有活三（甚至双活三）时，我方只能走：①对方活三的前二防守点 ②我方眠三进攻点 ③对方活三的第三防守点）
int AiThread::findS3A3(Choice *choices_buffer, const Chessid cur_player) const
{
	if (cur_player == AI_CHESS && all_type[I_HU_A3])
	{
		int all_cnt = all_type[I_HU_A3] + all_type[I_AI_S3];
		int hu_a3_cnt = 0, ai_s3_cnt = 0;
		Choice hu_a3_pos[GRID_N * 3][3], ai_s3_pos[GRID_N * 3][2];

		for (int c=0; c<GRID_N; ++c)
		{
			if (col_type[c] & F_HU_A3)  // 在第c列发现了活三
			{
				for (__int32 temp_col_chess = col_chess[c], i=0; i<10; ++i, temp_col_chess>>=2)
				{
					if (M_TABLE[MASK & temp_col_chess] == F_HU_A3)  // 第i次扫描发现了活三
					{
						hu_a3_pos[hu_a3_cnt][0].x = c;
						hu_a3_pos[hu_a3_cnt][0].y = i + A3_KEY_POS_TABLE[MASK & temp_col_chess][0];
						hu_a3_pos[hu_a3_cnt][1].x = c;
						hu_a3_pos[hu_a3_cnt][1].y = i + A3_KEY_POS_TABLE[MASK & temp_col_chess][1];
						hu_a3_pos[hu_a3_cnt][2].x = c;
						hu_a3_pos[hu_a3_cnt][2].y = i + A3_KEY_POS_TABLE[MASK & temp_col_chess][2];
						++hu_a3_cnt;
						if (hu_a3_cnt + ai_s3_cnt == all_cnt)
							goto exit_hua3_ais3;
						break;
					}
				}
			}
		}
		for (int r=0; r<GRID_N; ++r)
		{
			if (row_type[r] & F_HU_A3)  // 在第r行找到了活三
			{
				for (__int32 temp_row_chess = row_chess[r], i=0; i<10; ++i, temp_row_chess>>=2)
				{
					if (M_TABLE[MASK & temp_row_chess] == F_HU_A3)  // 第i次扫描发现了活三
					{
						hu_a3_pos[hu_a3_cnt][0].x = i + A3_KEY_POS_TABLE[MASK & temp_row_chess][0];
						hu_a3_pos[hu_a3_cnt][0].y = r;
						hu_a3_pos[hu_a3_cnt][1].x = i + A3_KEY_POS_TABLE[MASK & temp_row_chess][1];
						hu_a3_pos[hu_a3_cnt][1].y = r;
						hu_a3_pos[hu_a3_cnt][2].x = i + A3_KEY_POS_TABLE[MASK & temp_row_chess][2];
						hu_a3_pos[hu_a3_cnt][2].y = r;
						++hu_a3_cnt;
						if (hu_a3_cnt + ai_s3_cnt == all_cnt)
							goto exit_hua3_ais3;
						break;
					}
				}
			}
		}
		for (int l=0; l<GRID_DN; ++l) // l + y = x + 10
		{
			if (left_type[l] & F_HU_A3)
			{
				for (__int32 temp_left_chess = left_chess[l], i=0, count=getLeftCount(l); i<count; ++i, temp_left_chess>>=2)
				{
					if (M_TABLE[MASK & temp_left_chess] == F_HU_A3)
					{
						if (l <= 10)
						{
							hu_a3_pos[hu_a3_cnt][0].x = i + A3_KEY_POS_TABLE[MASK & temp_left_chess][0];
							hu_a3_pos[hu_a3_cnt][0].y = 10 - l + hu_a3_pos[hu_a3_cnt][0].x;
							hu_a3_pos[hu_a3_cnt][1].x = i + A3_KEY_POS_TABLE[MASK & temp_left_chess][1];
							hu_a3_pos[hu_a3_cnt][1].y = 10 - l + hu_a3_pos[hu_a3_cnt][1].x;
							hu_a3_pos[hu_a3_cnt][2].x = i + A3_KEY_POS_TABLE[MASK & temp_left_chess][2];
							hu_a3_pos[hu_a3_cnt][2].y = 10 - l + hu_a3_pos[hu_a3_cnt][2].x;
						}
						else
						{
							hu_a3_pos[hu_a3_cnt][0].y = i + A3_KEY_POS_TABLE[MASK & temp_left_chess][0];
							hu_a3_pos[hu_a3_cnt][0].x = l - 10 + hu_a3_pos[hu_a3_cnt][0].y;
							hu_a3_pos[hu_a3_cnt][1].y = i + A3_KEY_POS_TABLE[MASK & temp_left_chess][1];
							hu_a3_pos[hu_a3_cnt][1].x = l - 10 + hu_a3_pos[hu_a3_cnt][1].y;
							hu_a3_pos[hu_a3_cnt][2].y = i + A3_KEY_POS_TABLE[MASK & temp_left_chess][2];
							hu_a3_pos[hu_a3_cnt][2].x = l - 10 + hu_a3_pos[hu_a3_cnt][2].y;
						}

						++hu_a3_cnt;
						if (hu_a3_cnt + ai_s3_cnt == all_cnt)
							goto exit_hua3_ais3;
						break;
					}
				}
			}
		}
		for (int r=0; r<GRID_DN; ++r) // r + 4 = x + y
		{
			if (right_type[r] & F_HU_A3)
			{
				for (__int32 temp_right_chess = right_chess[r], i=0, count=getRightCount(r); i<count; ++i, temp_right_chess>>=2)
				{
					if (M_TABLE[MASK & temp_right_chess] == F_HU_A3)
					{
						if (r <= 10)
						{
							hu_a3_pos[hu_a3_cnt][0].y = i + A3_KEY_POS_TABLE[MASK & temp_right_chess][0];
							hu_a3_pos[hu_a3_cnt][0].x = r + 4 - hu_a3_pos[hu_a3_cnt][0].y;
							hu_a3_pos[hu_a3_cnt][1].y = i + A3_KEY_POS_TABLE[MASK & temp_right_chess][1];
							hu_a3_pos[hu_a3_cnt][1].x = r + 4 - hu_a3_pos[hu_a3_cnt][1].y;
							hu_a3_pos[hu_a3_cnt][2].y = i + A3_KEY_POS_TABLE[MASK & temp_right_chess][2];
							hu_a3_pos[hu_a3_cnt][2].x = r + 4 - hu_a3_pos[hu_a3_cnt][2].y;
						}
						else
						{
							hu_a3_pos[hu_a3_cnt][0].x = 14 - i - A3_KEY_POS_TABLE[MASK & temp_right_chess][0];
							hu_a3_pos[hu_a3_cnt][0].y = r + 4 - hu_a3_pos[hu_a3_cnt][0].x;
							hu_a3_pos[hu_a3_cnt][1].x = 14 - i - A3_KEY_POS_TABLE[MASK & temp_right_chess][1];
							hu_a3_pos[hu_a3_cnt][1].y = r + 4 - hu_a3_pos[hu_a3_cnt][1].x;
							hu_a3_pos[hu_a3_cnt][2].x = 14 - i - A3_KEY_POS_TABLE[MASK & temp_right_chess][2];
							hu_a3_pos[hu_a3_cnt][2].y = r + 4 - hu_a3_pos[hu_a3_cnt][2].x;
						}

						++hu_a3_cnt;
						if (hu_a3_cnt + ai_s3_cnt == all_cnt)
							goto exit_hua3_ais3;
						break;
					}
				}
			}
		}


		for (int c=0; c<GRID_N; ++c)
		{
			if (col_type[c] & F_AI_S3)  // 在第c列发现了活三
			{
				for (__int32 temp_col_chess = col_chess[c], i=0; i<10; ++i, temp_col_chess>>=2)
				{
					if (M_TABLE[MASK & temp_col_chess] == F_AI_S3)  // 第i次扫描发现了活三
					{
						ai_s3_pos[ai_s3_cnt][0].x = c;
						ai_s3_pos[ai_s3_cnt][0].y = i + S3_KEY_POS_TABLE[MASK & temp_col_chess][0];
						ai_s3_pos[ai_s3_cnt][1].x = c;
						ai_s3_pos[ai_s3_cnt][1].y = i + S3_KEY_POS_TABLE[MASK & temp_col_chess][1];
						++ai_s3_cnt;
						if (hu_a3_cnt + ai_s3_cnt == all_cnt)
							goto exit_hua3_ais3;
						break;
					}
				}
			}
		}
		for (int r=0; r<GRID_N; ++r)
		{
			if (row_type[r] & F_AI_S3)  // 在第r行找到了活三
			{
				for (__int32 temp_row_chess = row_chess[r], i=0; i<10; ++i, temp_row_chess>>=2)
				{
					if (M_TABLE[MASK & temp_row_chess] == F_AI_S3)  // 第i次扫描发现了活三
					{
						ai_s3_pos[ai_s3_cnt][0].x = i + S3_KEY_POS_TABLE[MASK & temp_row_chess][0];
						ai_s3_pos[ai_s3_cnt][0].y = r;
						ai_s3_pos[ai_s3_cnt][1].x = i + S3_KEY_POS_TABLE[MASK & temp_row_chess][1];
						ai_s3_pos[ai_s3_cnt][1].y = r;
						++ai_s3_cnt;
						if (hu_a3_cnt + ai_s3_cnt == all_cnt)
							goto exit_hua3_ais3;
						break;
					}
				}
			}
		}
		for (int l=0; l<GRID_DN; ++l) // l + y = x + 10
		{
			if (left_type[l] & F_AI_S3)
			{
				for (__int32 temp_left_chess = left_chess[l], i=0, count=getLeftCount(l); i<count; ++i, temp_left_chess>>=2)
				{
					if (M_TABLE[MASK & temp_left_chess] == F_AI_S3)
					{
						if (l <= 10)
						{
							ai_s3_pos[ai_s3_cnt][0].x = i + S3_KEY_POS_TABLE[MASK & temp_left_chess][0];
							ai_s3_pos[ai_s3_cnt][0].y = 10 - l + ai_s3_pos[ai_s3_cnt][0].x;
							ai_s3_pos[ai_s3_cnt][1].x = i + S3_KEY_POS_TABLE[MASK & temp_left_chess][1];
							ai_s3_pos[ai_s3_cnt][1].y = 10 - l + ai_s3_pos[ai_s3_cnt][1].x;
						}
						else
						{
							ai_s3_pos[ai_s3_cnt][0].y = i + S3_KEY_POS_TABLE[MASK & temp_left_chess][0];
							ai_s3_pos[ai_s3_cnt][0].x = l - 10 + ai_s3_pos[ai_s3_cnt][0].y;
							ai_s3_pos[ai_s3_cnt][1].y = i + S3_KEY_POS_TABLE[MASK & temp_left_chess][1];
							ai_s3_pos[ai_s3_cnt][1].x = l - 10 + ai_s3_pos[ai_s3_cnt][1].y;
						}

						++ai_s3_cnt;
						if (hu_a3_cnt + ai_s3_cnt == all_cnt)
							goto exit_hua3_ais3;
						break;
					}
				}
			}
		}
		for (int r=0; r<GRID_DN; ++r) // r + 4 = x + y
		{
			if (right_type[r] & F_AI_S3)
			{
				for (__int32 temp_right_chess = right_chess[r], i=0, count=getRightCount(r); i<count; ++i, temp_right_chess>>=2)
				{
					if (M_TABLE[MASK & temp_right_chess] == F_AI_S3)
					{
						if (r <= 10)
						{
							ai_s3_pos[ai_s3_cnt][0].y = i + S3_KEY_POS_TABLE[MASK & temp_right_chess][0];
							ai_s3_pos[ai_s3_cnt][0].x = r + 4 - ai_s3_pos[ai_s3_cnt][0].y;
							ai_s3_pos[ai_s3_cnt][1].y = i + S3_KEY_POS_TABLE[MASK & temp_right_chess][1];
							ai_s3_pos[ai_s3_cnt][1].x = r + 4 - ai_s3_pos[ai_s3_cnt][1].y;
						}
						else
						{
							ai_s3_pos[ai_s3_cnt][0].x = 14 - i - S3_KEY_POS_TABLE[MASK & temp_right_chess][0];
							ai_s3_pos[ai_s3_cnt][0].y = r + 4 - ai_s3_pos[ai_s3_cnt][0].x;
							ai_s3_pos[ai_s3_cnt][1].x = 14 - i - S3_KEY_POS_TABLE[MASK & temp_right_chess][1];
							ai_s3_pos[ai_s3_cnt][1].y = r + 4 - ai_s3_pos[ai_s3_cnt][1].x;
						}

						++ai_s3_cnt;
						if (hu_a3_cnt + ai_s3_cnt == all_cnt)
							goto exit_hua3_ais3;
						break;
					}
				}
			}
		}
exit_hua3_ais3:

		int top = 0;

		for (int i=0; i<hu_a3_cnt; ++i)
		{
			choices_buffer[top].x = hu_a3_pos[i][0].x, choices_buffer[top].y = hu_a3_pos[i][0].y; ++top;
			choices_buffer[top].x = hu_a3_pos[i][1].x, choices_buffer[top].y = hu_a3_pos[i][1].y; ++top;
		}

		for (int i=0; i<ai_s3_cnt; ++i)
		{
			choices_buffer[top].x = ai_s3_pos[i][0].x, choices_buffer[top].y = ai_s3_pos[i][0].y; ++top;
			choices_buffer[top].x = ai_s3_pos[i][1].x, choices_buffer[top].y = ai_s3_pos[i][1].y; ++top;
		}

		for (int i=0; i<hu_a3_cnt; ++i)
		{
			choices_buffer[top].x = hu_a3_pos[i][2].x, choices_buffer[top].y = hu_a3_pos[i][2].y; ++top;
		}

		return top;
	}

	else if (cur_player == H1_CHESS && all_type[I_AI_A3])
	{
		int all_cnt = all_type[I_AI_A3] + all_type[I_HU_S3];
		int ai_a3_cnt = 0, hu_s3_cnt = 0;
		Choice ai_a3_pos[GRID_N * 3][3], hu_s3_pos[GRID_N * 3][2];

		for (int c=0; c<GRID_N; ++c)
		{
			if (col_type[c] & F_AI_A3)  // 在第c列发现了活三
			{
				for (__int32 temp_col_chess = col_chess[c], i=0; i<10; ++i, temp_col_chess>>=2)
				{
					if (M_TABLE[MASK & temp_col_chess] == F_AI_A3)  // 第i次扫描发现了活三
					{
						ai_a3_pos[ai_a3_cnt][0].x = c;
						ai_a3_pos[ai_a3_cnt][0].y = i + A3_KEY_POS_TABLE[MASK & temp_col_chess][0];
						ai_a3_pos[ai_a3_cnt][1].x = c;
						ai_a3_pos[ai_a3_cnt][1].y = i + A3_KEY_POS_TABLE[MASK & temp_col_chess][1];
						ai_a3_pos[ai_a3_cnt][2].x = c;
						ai_a3_pos[ai_a3_cnt][2].y = i + A3_KEY_POS_TABLE[MASK & temp_col_chess][2];
						++ai_a3_cnt;
						if (ai_a3_cnt + hu_s3_cnt == all_cnt)
							goto exit_aia3_hus3;
						break;
					}
				}
			}
		}
		for (int r=0; r<GRID_N; ++r)
		{
			if (row_type[r] & F_AI_A3)  // 在第r行找到了活三
			{
				for (__int32 temp_row_chess = row_chess[r], i=0; i<10; ++i, temp_row_chess>>=2)
				{
					if (M_TABLE[MASK & temp_row_chess] == F_AI_A3)  // 第i次扫描发现了活三
					{
						ai_a3_pos[ai_a3_cnt][0].x = i + A3_KEY_POS_TABLE[MASK & temp_row_chess][0];
						ai_a3_pos[ai_a3_cnt][0].y = r;
						ai_a3_pos[ai_a3_cnt][1].x = i + A3_KEY_POS_TABLE[MASK & temp_row_chess][1];
						ai_a3_pos[ai_a3_cnt][1].y = r;
						ai_a3_pos[ai_a3_cnt][2].x = i + A3_KEY_POS_TABLE[MASK & temp_row_chess][2];
						ai_a3_pos[ai_a3_cnt][2].y = r;
						++ai_a3_cnt;
						if (ai_a3_cnt + hu_s3_cnt == all_cnt)
							goto exit_aia3_hus3;
						break;
					}
				}
			}
		}
		for (int l=0; l<GRID_DN; ++l) // l + y = x + 10
		{
			if (left_type[l] & F_AI_A3)
			{
				for (__int32 temp_left_chess = left_chess[l], i=0, count=getLeftCount(l); i<count; ++i, temp_left_chess>>=2)
				{
					if (M_TABLE[MASK & temp_left_chess] == F_AI_A3)
					{
						if (l <= 10)
						{
							ai_a3_pos[ai_a3_cnt][0].x = i + A3_KEY_POS_TABLE[MASK & temp_left_chess][0];
							ai_a3_pos[ai_a3_cnt][0].y = 10 - l + ai_a3_pos[ai_a3_cnt][0].x;
							ai_a3_pos[ai_a3_cnt][1].x = i + A3_KEY_POS_TABLE[MASK & temp_left_chess][1];
							ai_a3_pos[ai_a3_cnt][1].y = 10 - l + ai_a3_pos[ai_a3_cnt][1].x;
							ai_a3_pos[ai_a3_cnt][2].x = i + A3_KEY_POS_TABLE[MASK & temp_left_chess][2];
							ai_a3_pos[ai_a3_cnt][2].y = 10 - l + ai_a3_pos[ai_a3_cnt][2].x;
						}
						else
						{
							ai_a3_pos[ai_a3_cnt][0].y = i + A3_KEY_POS_TABLE[MASK & temp_left_chess][0];
							ai_a3_pos[ai_a3_cnt][0].x = l - 10 + ai_a3_pos[ai_a3_cnt][0].y;
							ai_a3_pos[ai_a3_cnt][1].y = i + A3_KEY_POS_TABLE[MASK & temp_left_chess][1];
							ai_a3_pos[ai_a3_cnt][1].x = l - 10 + ai_a3_pos[ai_a3_cnt][1].y;
							ai_a3_pos[ai_a3_cnt][2].y = i + A3_KEY_POS_TABLE[MASK & temp_left_chess][2];
							ai_a3_pos[ai_a3_cnt][2].x = l - 10 + ai_a3_pos[ai_a3_cnt][2].y;
						}

						++ai_a3_cnt;
						if (ai_a3_cnt + hu_s3_cnt == all_cnt)
							goto exit_aia3_hus3;
						break;
					}
				}
			}
		}
		for (int r=0; r<GRID_DN; ++r) // r + 4 = x + y
		{
			if (right_type[r] & F_AI_A3)
			{
				for (__int32 temp_right_chess = right_chess[r], i=0, count=getRightCount(r); i<count; ++i, temp_right_chess>>=2)
				{
					if (M_TABLE[MASK & temp_right_chess] == F_AI_A3)
					{
						if (r <= 10)
						{
							ai_a3_pos[ai_a3_cnt][0].y = i + A3_KEY_POS_TABLE[MASK & temp_right_chess][0];
							ai_a3_pos[ai_a3_cnt][0].x = r + 4 - ai_a3_pos[ai_a3_cnt][0].y;
							ai_a3_pos[ai_a3_cnt][1].y = i + A3_KEY_POS_TABLE[MASK & temp_right_chess][1];
							ai_a3_pos[ai_a3_cnt][1].x = r + 4 - ai_a3_pos[ai_a3_cnt][1].y;
							ai_a3_pos[ai_a3_cnt][2].y = i + A3_KEY_POS_TABLE[MASK & temp_right_chess][2];
							ai_a3_pos[ai_a3_cnt][2].x = r + 4 - ai_a3_pos[ai_a3_cnt][2].y;
						}
						else
						{
							ai_a3_pos[ai_a3_cnt][0].x = 14 - i - A3_KEY_POS_TABLE[MASK & temp_right_chess][0];
							ai_a3_pos[ai_a3_cnt][0].y = r + 4 - ai_a3_pos[ai_a3_cnt][0].x;
							ai_a3_pos[ai_a3_cnt][1].x = 14 - i - A3_KEY_POS_TABLE[MASK & temp_right_chess][1];
							ai_a3_pos[ai_a3_cnt][1].y = r + 4 - ai_a3_pos[ai_a3_cnt][1].x;
							ai_a3_pos[ai_a3_cnt][2].x = 14 - i - A3_KEY_POS_TABLE[MASK & temp_right_chess][2];
							ai_a3_pos[ai_a3_cnt][2].y = r + 4 - ai_a3_pos[ai_a3_cnt][2].x;
						}

						++ai_a3_cnt;
						if (ai_a3_cnt + hu_s3_cnt == all_cnt)
							goto exit_aia3_hus3;
						break;
					}
				}
			}
		}


		for (int c=0; c<GRID_N; ++c)
		{
			if (col_type[c] & F_HU_S3)  // 在第c列发现了活三
			{
				for (__int32 temp_col_chess = col_chess[c], i=0; i<10; ++i, temp_col_chess>>=2)
				{
					if (M_TABLE[MASK & temp_col_chess] == F_HU_S3)  // 第i次扫描发现了活三
					{
						hu_s3_pos[hu_s3_cnt][0].x = c;
						hu_s3_pos[hu_s3_cnt][0].y = i + S3_KEY_POS_TABLE[MASK & temp_col_chess][0];
						hu_s3_pos[hu_s3_cnt][1].x = c;
						hu_s3_pos[hu_s3_cnt][1].y = i + S3_KEY_POS_TABLE[MASK & temp_col_chess][1];
						++hu_s3_cnt;
						if (ai_a3_cnt + hu_s3_cnt == all_cnt)
							goto exit_aia3_hus3;
						break;
					}
				}
			}
		}
		for (int r=0; r<GRID_N; ++r)
		{
			if (row_type[r] & F_HU_S3)  // 在第r行找到了活三
			{
				for (__int32 temp_row_chess = row_chess[r], i=0; i<10; ++i, temp_row_chess>>=2)
				{
					if (M_TABLE[MASK & temp_row_chess] == F_HU_S3)  // 第i次扫描发现了活三
					{
						hu_s3_pos[hu_s3_cnt][0].x = i + S3_KEY_POS_TABLE[MASK & temp_row_chess][0];
						hu_s3_pos[hu_s3_cnt][0].y = r;
						hu_s3_pos[hu_s3_cnt][1].x = i + S3_KEY_POS_TABLE[MASK & temp_row_chess][1];
						hu_s3_pos[hu_s3_cnt][1].y = r;
						++hu_s3_cnt;
						if (ai_a3_cnt + hu_s3_cnt == all_cnt)
							goto exit_aia3_hus3;
						break;
					}
				}
			}
		}
		for (int l=0; l<GRID_DN; ++l) // l + y = x + 10
		{
			if (left_type[l] & F_HU_S3)
			{
				for (__int32 temp_left_chess = left_chess[l], i=0, count=getLeftCount(l); i<count; ++i, temp_left_chess>>=2)
				{
					if (M_TABLE[MASK & temp_left_chess] == F_HU_S3)
					{
						if (l <= 10)
						{
							hu_s3_pos[hu_s3_cnt][0].x = i + S3_KEY_POS_TABLE[MASK & temp_left_chess][0];
							hu_s3_pos[hu_s3_cnt][0].y = 10 - l + hu_s3_pos[hu_s3_cnt][0].x;
							hu_s3_pos[hu_s3_cnt][1].x = i + S3_KEY_POS_TABLE[MASK & temp_left_chess][1];
							hu_s3_pos[hu_s3_cnt][1].y = 10 - l + hu_s3_pos[hu_s3_cnt][1].x;
						}
						else
						{
							hu_s3_pos[hu_s3_cnt][0].y = i + S3_KEY_POS_TABLE[MASK & temp_left_chess][0];
							hu_s3_pos[hu_s3_cnt][0].x = l - 10 + hu_s3_pos[hu_s3_cnt][0].y;
							hu_s3_pos[hu_s3_cnt][1].y = i + S3_KEY_POS_TABLE[MASK & temp_left_chess][1];
							hu_s3_pos[hu_s3_cnt][1].x = l - 10 + hu_s3_pos[hu_s3_cnt][1].y;
						}

						++hu_s3_cnt;
						if (ai_a3_cnt + hu_s3_cnt == all_cnt)
							goto exit_aia3_hus3;
						break;
					}
				}
			}
		}
		for (int r=0; r<GRID_DN; ++r) // r + 4 = x + y
		{
			if (right_type[r] & F_HU_S3)
			{
				for (__int32 temp_right_chess = right_chess[r], i=0, count=getRightCount(r); i<count; ++i, temp_right_chess>>=2)
				{
					if (M_TABLE[MASK & temp_right_chess] == F_HU_S3)
					{
						if (r <= 10)
						{
							hu_s3_pos[hu_s3_cnt][0].y = i + S3_KEY_POS_TABLE[MASK & temp_right_chess][0];
							hu_s3_pos[hu_s3_cnt][0].x = r + 4 - hu_s3_pos[hu_s3_cnt][0].y;
							hu_s3_pos[hu_s3_cnt][1].y = i + S3_KEY_POS_TABLE[MASK & temp_right_chess][1];
							hu_s3_pos[hu_s3_cnt][1].x = r + 4 - hu_s3_pos[hu_s3_cnt][1].y;
						}
						else
						{
							hu_s3_pos[hu_s3_cnt][0].x = 14 - i - S3_KEY_POS_TABLE[MASK & temp_right_chess][0];
							hu_s3_pos[hu_s3_cnt][0].y = r + 4 - hu_s3_pos[hu_s3_cnt][0].x;
							hu_s3_pos[hu_s3_cnt][1].x = 14 - i - S3_KEY_POS_TABLE[MASK & temp_right_chess][1];
							hu_s3_pos[hu_s3_cnt][1].y = r + 4 - hu_s3_pos[hu_s3_cnt][1].x;
						}

						++hu_s3_cnt;
						if (ai_a3_cnt + hu_s3_cnt == all_cnt)
							goto exit_aia3_hus3;
						break;
					}
				}
			}
		}
exit_aia3_hus3:

		int top = 0;

		for (int i=0; i<ai_a3_cnt; ++i)
		{
			choices_buffer[top].x = ai_a3_pos[i][0].x, choices_buffer[top].y = ai_a3_pos[i][0].y; ++top;
			choices_buffer[top].x = ai_a3_pos[i][1].x, choices_buffer[top].y = ai_a3_pos[i][1].y; ++top;
		}

		for (int i=0; i<hu_s3_cnt; ++i)
		{
			choices_buffer[top].x = hu_s3_pos[i][0].x, choices_buffer[top].y = hu_s3_pos[i][0].y; ++top;
			choices_buffer[top].x = hu_s3_pos[i][1].x, choices_buffer[top].y = hu_s3_pos[i][1].y; ++top;
		}

		for (int i=0; i<ai_a3_cnt; ++i)
		{
			choices_buffer[top].x = ai_a3_pos[i][2].x, choices_buffer[top].y = ai_a3_pos[i][2].y; ++top;
		}

		return top;
	}

	else
		return 0;
}

int AiThread::findAllS3A3(Choice *choices_buffer, const Chessid cur_player) const
{
	if (cur_player == AI_CHESS)
	{
		int hu_a3_cnt = 0, ai_s3_cnt = 0;
		Choice hu_a3_pos[GRID_N * 3][3], ai_s3_pos[GRID_N * 3][2];

		for (int c=0; c<GRID_N; ++c)
		{
			if (col_type[c] & F_HU_A3)  // 在第c列发现了活三
			{
				for (__int32 temp_col_chess = col_chess[c], i=0; i<10; ++i, temp_col_chess>>=2)
				{
					if (M_TABLE[MASK & temp_col_chess] == F_HU_A3)  // 第i次扫描发现了活三
					{
						hu_a3_pos[hu_a3_cnt][0].x = c;
						hu_a3_pos[hu_a3_cnt][0].y = i + A3_KEY_POS_TABLE[MASK & temp_col_chess][0];
						hu_a3_pos[hu_a3_cnt][1].x = c;
						hu_a3_pos[hu_a3_cnt][1].y = i + A3_KEY_POS_TABLE[MASK & temp_col_chess][1];
						hu_a3_pos[hu_a3_cnt][2].x = c;
						hu_a3_pos[hu_a3_cnt][2].y = i + A3_KEY_POS_TABLE[MASK & temp_col_chess][2];
						++hu_a3_cnt;
						break;
					}
				}
			}
		}
		for (int r=0; r<GRID_N; ++r)
		{
			if (row_type[r] & F_HU_A3)  // 在第r行找到了活三
			{
				for (__int32 temp_row_chess = row_chess[r], i=0; i<10; ++i, temp_row_chess>>=2)
				{
					if (M_TABLE[MASK & temp_row_chess] == F_HU_A3)  // 第i次扫描发现了活三
					{
						hu_a3_pos[hu_a3_cnt][0].x = i + A3_KEY_POS_TABLE[MASK & temp_row_chess][0];
						hu_a3_pos[hu_a3_cnt][0].y = r;
						hu_a3_pos[hu_a3_cnt][1].x = i + A3_KEY_POS_TABLE[MASK & temp_row_chess][1];
						hu_a3_pos[hu_a3_cnt][1].y = r;
						hu_a3_pos[hu_a3_cnt][2].x = i + A3_KEY_POS_TABLE[MASK & temp_row_chess][2];
						hu_a3_pos[hu_a3_cnt][2].y = r;
						++hu_a3_cnt;
						break;
					}
				}
			}
		}
		for (int l=0; l<GRID_DN; ++l) // l + y = x + 10
		{
			if (left_type[l] & F_HU_A3)
			{
				for (__int32 temp_left_chess = left_chess[l], i=0, count=getLeftCount(l); i<count; ++i, temp_left_chess>>=2)
				{
					if (M_TABLE[MASK & temp_left_chess] == F_HU_A3)
					{
						if (l <= 10)
						{
							hu_a3_pos[hu_a3_cnt][0].x = i + A3_KEY_POS_TABLE[MASK & temp_left_chess][0];
							hu_a3_pos[hu_a3_cnt][0].y = 10 - l + hu_a3_pos[hu_a3_cnt][0].x;
							hu_a3_pos[hu_a3_cnt][1].x = i + A3_KEY_POS_TABLE[MASK & temp_left_chess][1];
							hu_a3_pos[hu_a3_cnt][1].y = 10 - l + hu_a3_pos[hu_a3_cnt][1].x;
							hu_a3_pos[hu_a3_cnt][2].x = i + A3_KEY_POS_TABLE[MASK & temp_left_chess][2];
							hu_a3_pos[hu_a3_cnt][2].y = 10 - l + hu_a3_pos[hu_a3_cnt][2].x;
						}
						else
						{
							hu_a3_pos[hu_a3_cnt][0].y = i + A3_KEY_POS_TABLE[MASK & temp_left_chess][0];
							hu_a3_pos[hu_a3_cnt][0].x = l - 10 + hu_a3_pos[hu_a3_cnt][0].y;
							hu_a3_pos[hu_a3_cnt][1].y = i + A3_KEY_POS_TABLE[MASK & temp_left_chess][1];
							hu_a3_pos[hu_a3_cnt][1].x = l - 10 + hu_a3_pos[hu_a3_cnt][1].y;
							hu_a3_pos[hu_a3_cnt][2].y = i + A3_KEY_POS_TABLE[MASK & temp_left_chess][2];
							hu_a3_pos[hu_a3_cnt][2].x = l - 10 + hu_a3_pos[hu_a3_cnt][2].y;
						}

						++hu_a3_cnt;
						break;
					}
				}
			}
		}
		for (int r=0; r<GRID_DN; ++r) // r + 4 = x + y
		{
			if (right_type[r] & F_HU_A3)
			{
				for (__int32 temp_right_chess = right_chess[r], i=0, count=getRightCount(r); i<count; ++i, temp_right_chess>>=2)
				{
					if (M_TABLE[MASK & temp_right_chess] == F_HU_A3)
					{
						if (r <= 10)
						{
							hu_a3_pos[hu_a3_cnt][0].y = i + A3_KEY_POS_TABLE[MASK & temp_right_chess][0];
							hu_a3_pos[hu_a3_cnt][0].x = r + 4 - hu_a3_pos[hu_a3_cnt][0].y;
							hu_a3_pos[hu_a3_cnt][1].y = i + A3_KEY_POS_TABLE[MASK & temp_right_chess][1];
							hu_a3_pos[hu_a3_cnt][1].x = r + 4 - hu_a3_pos[hu_a3_cnt][1].y;
							hu_a3_pos[hu_a3_cnt][2].y = i + A3_KEY_POS_TABLE[MASK & temp_right_chess][2];
							hu_a3_pos[hu_a3_cnt][2].x = r + 4 - hu_a3_pos[hu_a3_cnt][2].y;
						}
						else
						{
							hu_a3_pos[hu_a3_cnt][0].x = 14 - i - A3_KEY_POS_TABLE[MASK & temp_right_chess][0];
							hu_a3_pos[hu_a3_cnt][0].y = r + 4 - hu_a3_pos[hu_a3_cnt][0].x;
							hu_a3_pos[hu_a3_cnt][1].x = 14 - i - A3_KEY_POS_TABLE[MASK & temp_right_chess][1];
							hu_a3_pos[hu_a3_cnt][1].y = r + 4 - hu_a3_pos[hu_a3_cnt][1].x;
							hu_a3_pos[hu_a3_cnt][2].x = 14 - i - A3_KEY_POS_TABLE[MASK & temp_right_chess][2];
							hu_a3_pos[hu_a3_cnt][2].y = r + 4 - hu_a3_pos[hu_a3_cnt][2].x;
						}

						++hu_a3_cnt;
						break;
					}
				}
			}
		}


		for (int c=0; c<GRID_N; ++c)
		{
			if (col_type[c] & F_AI_S3)  // 在第c列发现了活三
			{
				for (__int32 temp_col_chess = col_chess[c], i=0; i<10; ++i, temp_col_chess>>=2)
				{
					if (M_TABLE[MASK & temp_col_chess] == F_AI_S3)  // 第i次扫描发现了活三
					{
						ai_s3_pos[ai_s3_cnt][0].x = c;
						ai_s3_pos[ai_s3_cnt][0].y = i + S3_KEY_POS_TABLE[MASK & temp_col_chess][0];
						ai_s3_pos[ai_s3_cnt][1].x = c;
						ai_s3_pos[ai_s3_cnt][1].y = i + S3_KEY_POS_TABLE[MASK & temp_col_chess][1];
						++ai_s3_cnt;
						break;
					}
				}
			}
		}
		for (int r=0; r<GRID_N; ++r)
		{
			if (row_type[r] & F_AI_S3)  // 在第r行找到了活三
			{
				for (__int32 temp_row_chess = row_chess[r], i=0; i<10; ++i, temp_row_chess>>=2)
				{
					if (M_TABLE[MASK & temp_row_chess] == F_AI_S3)  // 第i次扫描发现了活三
					{
						ai_s3_pos[ai_s3_cnt][0].x = i + S3_KEY_POS_TABLE[MASK & temp_row_chess][0];
						ai_s3_pos[ai_s3_cnt][0].y = r;
						ai_s3_pos[ai_s3_cnt][1].x = i + S3_KEY_POS_TABLE[MASK & temp_row_chess][1];
						ai_s3_pos[ai_s3_cnt][1].y = r;
						++ai_s3_cnt;
						break;
					}
				}
			}
		}
		for (int l=0; l<GRID_DN; ++l) // l + y = x + 10
		{
			if (left_type[l] & F_AI_S3)
			{
				for (__int32 temp_left_chess = left_chess[l], i=0, count=getLeftCount(l); i<count; ++i, temp_left_chess>>=2)
				{
					if (M_TABLE[MASK & temp_left_chess] == F_AI_S3)
					{
						if (l <= 10)
						{
							ai_s3_pos[ai_s3_cnt][0].x = i + S3_KEY_POS_TABLE[MASK & temp_left_chess][0];
							ai_s3_pos[ai_s3_cnt][0].y = 10 - l + ai_s3_pos[ai_s3_cnt][0].x;
							ai_s3_pos[ai_s3_cnt][1].x = i + S3_KEY_POS_TABLE[MASK & temp_left_chess][1];
							ai_s3_pos[ai_s3_cnt][1].y = 10 - l + ai_s3_pos[ai_s3_cnt][1].x;
						}
						else
						{
							ai_s3_pos[ai_s3_cnt][0].y = i + S3_KEY_POS_TABLE[MASK & temp_left_chess][0];
							ai_s3_pos[ai_s3_cnt][0].x = l - 10 + ai_s3_pos[ai_s3_cnt][0].y;
							ai_s3_pos[ai_s3_cnt][1].y = i + S3_KEY_POS_TABLE[MASK & temp_left_chess][1];
							ai_s3_pos[ai_s3_cnt][1].x = l - 10 + ai_s3_pos[ai_s3_cnt][1].y;
						}

						++ai_s3_cnt;
						break;
					}
				}
			}
		}
		for (int r=0; r<GRID_DN; ++r) // r + 4 = x + y
		{
			if (right_type[r] & F_AI_S3)
			{
				for (__int32 temp_right_chess = right_chess[r], i=0, count=getRightCount(r); i<count; ++i, temp_right_chess>>=2)
				{
					if (M_TABLE[MASK & temp_right_chess] == F_AI_S3)
					{
						if (r <= 10)
						{
							ai_s3_pos[ai_s3_cnt][0].y = i + S3_KEY_POS_TABLE[MASK & temp_right_chess][0];
							ai_s3_pos[ai_s3_cnt][0].x = r + 4 - ai_s3_pos[ai_s3_cnt][0].y;
							ai_s3_pos[ai_s3_cnt][1].y = i + S3_KEY_POS_TABLE[MASK & temp_right_chess][1];
							ai_s3_pos[ai_s3_cnt][1].x = r + 4 - ai_s3_pos[ai_s3_cnt][1].y;
						}
						else
						{
							ai_s3_pos[ai_s3_cnt][0].x = 14 - i - S3_KEY_POS_TABLE[MASK & temp_right_chess][0];
							ai_s3_pos[ai_s3_cnt][0].y = r + 4 - ai_s3_pos[ai_s3_cnt][0].x;
							ai_s3_pos[ai_s3_cnt][1].x = 14 - i - S3_KEY_POS_TABLE[MASK & temp_right_chess][1];
							ai_s3_pos[ai_s3_cnt][1].y = r + 4 - ai_s3_pos[ai_s3_cnt][1].x;
						}

						++ai_s3_cnt;
						break;
					}
				}
			}
		}

		int top = 0;

		for (int i=0; i<hu_a3_cnt; ++i)
		{
			choices_buffer[top].x = hu_a3_pos[i][0].x, choices_buffer[top].y = hu_a3_pos[i][0].y; ++top;
			choices_buffer[top].x = hu_a3_pos[i][1].x, choices_buffer[top].y = hu_a3_pos[i][1].y; ++top;
		}

		for (int i=0; i<ai_s3_cnt; ++i)
		{
			choices_buffer[top].x = ai_s3_pos[i][0].x, choices_buffer[top].y = ai_s3_pos[i][0].y; ++top;
			choices_buffer[top].x = ai_s3_pos[i][1].x, choices_buffer[top].y = ai_s3_pos[i][1].y; ++top;
		}

		for (int i=0; i<hu_a3_cnt; ++i)
		{
			choices_buffer[top].x = hu_a3_pos[i][2].x, choices_buffer[top].y = hu_a3_pos[i][2].y; ++top;
		}

		return top;
	}

	else if (cur_player == H1_CHESS)
	{
		int ai_a3_cnt = 0, hu_s3_cnt = 0;
		Choice ai_a3_pos[GRID_N * 3][3], hu_s3_pos[GRID_N * 3][2];

		for (int c=0; c<GRID_N; ++c)
		{
			if (col_type[c] & F_AI_A3)  // 在第c列发现了活三
			{
				for (__int32 temp_col_chess = col_chess[c], i=0; i<10; ++i, temp_col_chess>>=2)
				{
					if (M_TABLE[MASK & temp_col_chess] == F_AI_A3)  // 第i次扫描发现了活三
					{
						ai_a3_pos[ai_a3_cnt][0].x = c;
						ai_a3_pos[ai_a3_cnt][0].y = i + A3_KEY_POS_TABLE[MASK & temp_col_chess][0];
						ai_a3_pos[ai_a3_cnt][1].x = c;
						ai_a3_pos[ai_a3_cnt][1].y = i + A3_KEY_POS_TABLE[MASK & temp_col_chess][1];
						ai_a3_pos[ai_a3_cnt][2].x = c;
						ai_a3_pos[ai_a3_cnt][2].y = i + A3_KEY_POS_TABLE[MASK & temp_col_chess][2];
						++ai_a3_cnt;
						break;
					}
				}
			}
		}
		for (int r=0; r<GRID_N; ++r)
		{
			if (row_type[r] & F_AI_A3)  // 在第r行找到了活三
			{
				for (__int32 temp_row_chess = row_chess[r], i=0; i<10; ++i, temp_row_chess>>=2)
				{
					if (M_TABLE[MASK & temp_row_chess] == F_AI_A3)  // 第i次扫描发现了活三
					{
						ai_a3_pos[ai_a3_cnt][0].x = i + A3_KEY_POS_TABLE[MASK & temp_row_chess][0];
						ai_a3_pos[ai_a3_cnt][0].y = r;
						ai_a3_pos[ai_a3_cnt][1].x = i + A3_KEY_POS_TABLE[MASK & temp_row_chess][1];
						ai_a3_pos[ai_a3_cnt][1].y = r;
						ai_a3_pos[ai_a3_cnt][2].x = i + A3_KEY_POS_TABLE[MASK & temp_row_chess][2];
						ai_a3_pos[ai_a3_cnt][2].y = r;
						++ai_a3_cnt;
						break;
					}
				}
			}
		}
		for (int l=0; l<GRID_DN; ++l) // l + y = x + 10
		{
			if (left_type[l] & F_AI_A3)
			{
				for (__int32 temp_left_chess = left_chess[l], i=0, count=getLeftCount(l); i<count; ++i, temp_left_chess>>=2)
				{
					if (M_TABLE[MASK & temp_left_chess] == F_AI_A3)
					{
						if (l <= 10)
						{
							ai_a3_pos[ai_a3_cnt][0].x = i + A3_KEY_POS_TABLE[MASK & temp_left_chess][0];
							ai_a3_pos[ai_a3_cnt][0].y = 10 - l + ai_a3_pos[ai_a3_cnt][0].x;
							ai_a3_pos[ai_a3_cnt][1].x = i + A3_KEY_POS_TABLE[MASK & temp_left_chess][1];
							ai_a3_pos[ai_a3_cnt][1].y = 10 - l + ai_a3_pos[ai_a3_cnt][1].x;
							ai_a3_pos[ai_a3_cnt][2].x = i + A3_KEY_POS_TABLE[MASK & temp_left_chess][2];
							ai_a3_pos[ai_a3_cnt][2].y = 10 - l + ai_a3_pos[ai_a3_cnt][2].x;
						}
						else
						{
							ai_a3_pos[ai_a3_cnt][0].y = i + A3_KEY_POS_TABLE[MASK & temp_left_chess][0];
							ai_a3_pos[ai_a3_cnt][0].x = l - 10 + ai_a3_pos[ai_a3_cnt][0].y;
							ai_a3_pos[ai_a3_cnt][1].y = i + A3_KEY_POS_TABLE[MASK & temp_left_chess][1];
							ai_a3_pos[ai_a3_cnt][1].x = l - 10 + ai_a3_pos[ai_a3_cnt][1].y;
							ai_a3_pos[ai_a3_cnt][2].y = i + A3_KEY_POS_TABLE[MASK & temp_left_chess][2];
							ai_a3_pos[ai_a3_cnt][2].x = l - 10 + ai_a3_pos[ai_a3_cnt][2].y;
						}

						++ai_a3_cnt;
						break;
					}
				}
			}
		}
		for (int r=0; r<GRID_DN; ++r) // r + 4 = x + y
		{
			if (right_type[r] & F_AI_A3)
			{
				for (__int32 temp_right_chess = right_chess[r], i=0, count=getRightCount(r); i<count; ++i, temp_right_chess>>=2)
				{
					if (M_TABLE[MASK & temp_right_chess] == F_AI_A3)
					{
						if (r <= 10)
						{
							ai_a3_pos[ai_a3_cnt][0].y = i + A3_KEY_POS_TABLE[MASK & temp_right_chess][0];
							ai_a3_pos[ai_a3_cnt][0].x = r + 4 - ai_a3_pos[ai_a3_cnt][0].y;
							ai_a3_pos[ai_a3_cnt][1].y = i + A3_KEY_POS_TABLE[MASK & temp_right_chess][1];
							ai_a3_pos[ai_a3_cnt][1].x = r + 4 - ai_a3_pos[ai_a3_cnt][1].y;
							ai_a3_pos[ai_a3_cnt][2].y = i + A3_KEY_POS_TABLE[MASK & temp_right_chess][2];
							ai_a3_pos[ai_a3_cnt][2].x = r + 4 - ai_a3_pos[ai_a3_cnt][2].y;
						}
						else
						{
							ai_a3_pos[ai_a3_cnt][0].x = 14 - i - A3_KEY_POS_TABLE[MASK & temp_right_chess][0];
							ai_a3_pos[ai_a3_cnt][0].y = r + 4 - ai_a3_pos[ai_a3_cnt][0].x;
							ai_a3_pos[ai_a3_cnt][1].x = 14 - i - A3_KEY_POS_TABLE[MASK & temp_right_chess][1];
							ai_a3_pos[ai_a3_cnt][1].y = r + 4 - ai_a3_pos[ai_a3_cnt][1].x;
							ai_a3_pos[ai_a3_cnt][2].x = 14 - i - A3_KEY_POS_TABLE[MASK & temp_right_chess][2];
							ai_a3_pos[ai_a3_cnt][2].y = r + 4 - ai_a3_pos[ai_a3_cnt][2].x;
						}

						++ai_a3_cnt;
						break;
					}
				}
			}
		}


		for (int c=0; c<GRID_N; ++c)
		{
			if (col_type[c] & F_HU_S3)  // 在第c列发现了活三
			{
				for (__int32 temp_col_chess = col_chess[c], i=0; i<10; ++i, temp_col_chess>>=2)
				{
					if (M_TABLE[MASK & temp_col_chess] == F_HU_S3)  // 第i次扫描发现了活三
					{
						hu_s3_pos[hu_s3_cnt][0].x = c;
						hu_s3_pos[hu_s3_cnt][0].y = i + S3_KEY_POS_TABLE[MASK & temp_col_chess][0];
						hu_s3_pos[hu_s3_cnt][1].x = c;
						hu_s3_pos[hu_s3_cnt][1].y = i + S3_KEY_POS_TABLE[MASK & temp_col_chess][1];
						++hu_s3_cnt;
						break;
					}
				}
			}
		}
		for (int r=0; r<GRID_N; ++r)
		{
			if (row_type[r] & F_HU_S3)  // 在第r行找到了活三
			{
				for (__int32 temp_row_chess = row_chess[r], i=0; i<10; ++i, temp_row_chess>>=2)
				{
					if (M_TABLE[MASK & temp_row_chess] == F_HU_S3)  // 第i次扫描发现了活三
					{
						hu_s3_pos[hu_s3_cnt][0].x = i + S3_KEY_POS_TABLE[MASK & temp_row_chess][0];
						hu_s3_pos[hu_s3_cnt][0].y = r;
						hu_s3_pos[hu_s3_cnt][1].x = i + S3_KEY_POS_TABLE[MASK & temp_row_chess][1];
						hu_s3_pos[hu_s3_cnt][1].y = r;
						++hu_s3_cnt;
						break;
					}
				}
			}
		}
		for (int l=0; l<GRID_DN; ++l) // l + y = x + 10
		{
			if (left_type[l] & F_HU_S3)
			{
				for (__int32 temp_left_chess = left_chess[l], i=0, count=getLeftCount(l); i<count; ++i, temp_left_chess>>=2)
				{
					if (M_TABLE[MASK & temp_left_chess] == F_HU_S3)
					{
						if (l <= 10)
						{
							hu_s3_pos[hu_s3_cnt][0].x = i + S3_KEY_POS_TABLE[MASK & temp_left_chess][0];
							hu_s3_pos[hu_s3_cnt][0].y = 10 - l + hu_s3_pos[hu_s3_cnt][0].x;
							hu_s3_pos[hu_s3_cnt][1].x = i + S3_KEY_POS_TABLE[MASK & temp_left_chess][1];
							hu_s3_pos[hu_s3_cnt][1].y = 10 - l + hu_s3_pos[hu_s3_cnt][1].x;
						}
						else
						{
							hu_s3_pos[hu_s3_cnt][0].y = i + S3_KEY_POS_TABLE[MASK & temp_left_chess][0];
							hu_s3_pos[hu_s3_cnt][0].x = l - 10 + hu_s3_pos[hu_s3_cnt][0].y;
							hu_s3_pos[hu_s3_cnt][1].y = i + S3_KEY_POS_TABLE[MASK & temp_left_chess][1];
							hu_s3_pos[hu_s3_cnt][1].x = l - 10 + hu_s3_pos[hu_s3_cnt][1].y;
						}

						++hu_s3_cnt;
						break;
					}
				}
			}
		}
		for (int r=0; r<GRID_DN; ++r) // r + 4 = x + y
		{
			if (right_type[r] & F_HU_S3)
			{
				for (__int32 temp_right_chess = right_chess[r], i=0, count=getRightCount(r); i<count; ++i, temp_right_chess>>=2)
				{
					if (M_TABLE[MASK & temp_right_chess] == F_HU_S3)
					{
						if (r <= 10)
						{
							hu_s3_pos[hu_s3_cnt][0].y = i + S3_KEY_POS_TABLE[MASK & temp_right_chess][0];
							hu_s3_pos[hu_s3_cnt][0].x = r + 4 - hu_s3_pos[hu_s3_cnt][0].y;
							hu_s3_pos[hu_s3_cnt][1].y = i + S3_KEY_POS_TABLE[MASK & temp_right_chess][1];
							hu_s3_pos[hu_s3_cnt][1].x = r + 4 - hu_s3_pos[hu_s3_cnt][1].y;
						}
						else
						{
							hu_s3_pos[hu_s3_cnt][0].x = 14 - i - S3_KEY_POS_TABLE[MASK & temp_right_chess][0];
							hu_s3_pos[hu_s3_cnt][0].y = r + 4 - hu_s3_pos[hu_s3_cnt][0].x;
							hu_s3_pos[hu_s3_cnt][1].x = 14 - i - S3_KEY_POS_TABLE[MASK & temp_right_chess][1];
							hu_s3_pos[hu_s3_cnt][1].y = r + 4 - hu_s3_pos[hu_s3_cnt][1].x;
						}

						++hu_s3_cnt;
						break;
					}
				}
			}
		}

		int top = 0;

		for (int i=0; i<ai_a3_cnt; ++i)
		{
			choices_buffer[top].x = ai_a3_pos[i][0].x, choices_buffer[top].y = ai_a3_pos[i][0].y; ++top;
			choices_buffer[top].x = ai_a3_pos[i][1].x, choices_buffer[top].y = ai_a3_pos[i][1].y; ++top;
		}

		for (int i=0; i<hu_s3_cnt; ++i)
		{
			choices_buffer[top].x = hu_s3_pos[i][0].x, choices_buffer[top].y = hu_s3_pos[i][0].y; ++top;
			choices_buffer[top].x = hu_s3_pos[i][1].x, choices_buffer[top].y = hu_s3_pos[i][1].y; ++top;
		}

		for (int i=0; i<ai_a3_cnt; ++i)
		{
			choices_buffer[top].x = ai_a3_pos[i][2].x, choices_buffer[top].y = ai_a3_pos[i][2].y; ++top;
		}

		return top;
	}

	else
		return 0;
}


int AiThread::getMinMaxSearchChoices(Choice *choices_buffer, Chessid cur_player) const
{
#ifdef __OPEN_EP4
	int enemy_p4_cnt = this->findEnemyP4(choices_buffer, cur_player);
	if (enemy_p4_cnt)
		return enemy_p4_cnt;
#endif

#ifdef __OPEN_SA3
	int s3a3_cnt = this->findS3A3(choices_buffer, cur_player);
	if (s3a3_cnt)
		return s3a3_cnt;
#endif

	int r, c, i, j, cnt;
	bool not_found;
	int near_cnt;

	for (r=cnt=0; r<GRID_N; r++)
	{
		for (c=0; c<GRID_N; c++)
		{
			if (mirror_board[r][c] == NO_CHESS)
			{
				not_found = true;
				near_cnt = 0;
				for (i=r-2; i<=r+2&&not_found; i++)
				{
					for (j=c-2; j<=c+2; j++)
					{
						if (i>=0&&i<GRID_N && j>=0&&j<GRID_N && mirror_board[i][j])
							++near_cnt;
						if (near_cnt == NEAR_CNT)
						{
							choices_buffer[cnt].x = c;
							choices_buffer[cnt].y = r;
							choices_buffer[cnt].prior =
									evalMinMaxPrior(c, r, AI_CHESS)
									+ evalMinMaxPrior(c, r, H1_CHESS);
							++cnt;
							not_found = false;
							break;
						}
					}
				}
			}
		}
	}

	myQuickSort(choices_buffer, cnt);

	return cnt;
}

int AiThread::getKillSearchChoices(Choice *choices_buffer, Chessid cur_player) const
{
#ifdef __OPEN_EP4
	int enemy_p4_cnt = this->findEnemyP4(choices_buffer, cur_player);
	if (enemy_p4_cnt)
		return enemy_p4_cnt;
#endif

#ifdef __OPEN_SA3
	int s3a3_cnt = this->findS3A3(choices_buffer, cur_player);
	if (s3a3_cnt)
		return s3a3_cnt;
#endif

	int r, c, i, j, cnt, kill_prior;
	bool not_found;
	int near_cnt;

	for (r=cnt=0; r<GRID_N; r++)
	{
		for (c=0; c<GRID_N; c++)
		{
			if (mirror_board[r][c] == NO_CHESS)
			{
				not_found = true;
				near_cnt = 0;
				for (i=r-2; i<=r+2&&not_found; i++)
				{
					for (j=c-2; j<=c+2; j++)
					{
						if (i>=0&&i<GRID_N && j>=0&&j<GRID_N && mirror_board[i][j])
							++near_cnt;
						if (near_cnt == NEAR_CNT)
						{
							kill_prior = evalKillPrior(c, r, H1_CHESS) + evalKillPrior(c, r, AI_CHESS);
							if (kill_prior)
							{
								choices_buffer[cnt].x = c;
								choices_buffer[cnt].y = r;
								choices_buffer[cnt++].prior = kill_prior;
							}
							not_found = false;
							break;
						}
					}
				}
			}
		}
	}

	if (cnt)
	{
		myQuickSort(choices_buffer, cnt);
	}

	return cnt;
}

void AiThread::myQuickSort(Choice *a, int n) const
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

void AiThread::algoDebuging() const
{
	qDebug("Dep: %d %d %d", BASE_DEPTH, KILL_DEPTH, RES_DEPTH);
	qDebug("NC : %d", NEAR_CNT);

#ifdef __OPEN_EP4
	qDebug("OPEN EP4");
#else
	qDebug("CLOSE EP4");
#endif

#ifdef __OPEN_SA3
	qDebug("OPEN SA3");
#else
	qDebug("CLOSE SA3");
#endif

#if RES_DEPTH > KILL_DEPTH
	qDebug("OPEN Res");
#else
	qDebug("CLOSE Res");
#endif

}
