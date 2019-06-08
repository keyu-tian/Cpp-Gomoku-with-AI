#include "aithread.h"

int AiThread::evalBoard()
{
	int ai_sum = 0, hu_sum = 0;
	EVAL(ai_sum, hu_sum);

	if (all_type[I_AI_P4]>=2)
		ai_sum += Plus44score;
	else if (all_type[I_AI_P4]+all_type[I_AI_A3] >= 2)
		ai_sum += Plus34score;

	if (all_type[I_HU_P4]>=2)
		hu_sum += Plus44score;
	else if (all_type[I_HU_P4]+all_type[I_HU_A3] >= 2)
		hu_sum += Plus34score;

	return ai_sum - hu_sum;
}


int AiThread::evalMinMaxPrior(const int x, const int y, const Chessid id) const
{
	int min_max_prior = 0;
	__int32 idl2y = id << 2*y;
	__int32 idl2x = id << 2*x;
	__int32 now_col_chess = col_chess[x] | idl2y,
			now_row_chess = row_chess[y] | idl2x,
			now_left_chess = 0, now_right_chess = 0;

	int i, count;
	for(i=0; i<10; ++i, now_col_chess>>=2)
	{
		EVAL_MIN_MAX_PRIOR(now_col_chess);
	}

	for(i=0; i<10; ++i, now_row_chess>>=2)
	{
		EVAL_MIN_MAX_PRIOR(now_row_chess);
	}

	int l = x - y + 10;
	if (l>=0 && l<GRID_DN)
	{
		if (l<10)
			now_left_chess = left_chess[l] | idl2x;
		else
			now_left_chess = left_chess[l] | idl2y;

		for(i=0, count=getLeftCount(x, y); i<count; ++i, now_left_chess>>=2)
		{
			EVAL_MIN_MAX_PRIOR(now_left_chess);
		}
	}

	int r = x + y - 4;
	if (r>=0 && r<GRID_DN)
	{
		if (r<10)
			now_right_chess = right_chess[r] | idl2y;
		else
			now_right_chess = right_chess[r] | (id << (28-2*x));

		for(i=0, count=getRightCount(x, y); i<count; ++i, now_right_chess>>=2)
		{
			EVAL_MIN_MAX_PRIOR(now_right_chess);
		}
	}
	return min_max_prior;
}


int AiThread::evalKillPrior(const int x, const int y, const Chessid id) const
{
	int kill_prior = 0;
	__int32 idl2y = id << 2*y;
	__int32 idl2x = id << 2*x;

	__int32 now_col_chess = col_chess[x] | idl2y,
			now_row_chess = row_chess[y] | idl2x,
			now_left_chess = 0, now_right_chess = 0;

	int i, count;
	for(i=0; i<10; ++i, now_col_chess>>=2)
	{
		EVAL_KILL_PRIOR(now_col_chess)
	}

	for(i=0; i<10; ++i, now_row_chess>>=2)
	{
		EVAL_KILL_PRIOR(now_row_chess)
	}

	int l = x - y + 10;
	if (l>=0 && l<GRID_DN)
	{
		if (l<10)
			now_left_chess = left_chess[l] | idl2x;
		else
			now_left_chess = left_chess[l] | idl2y;

		for(i=0, count=getLeftCount(x, y); i<count; ++i, now_left_chess>>=2)
		{
			EVAL_KILL_PRIOR(now_left_chess);
		}
	}

	int r = x + y - 4;
	if (r>=0 && r<GRID_DN)
	{
		if (r<10)
			now_right_chess = right_chess[r] | idl2y;
		else
			now_right_chess = right_chess[r] | (id << (28-2*x));

		for(i=0, count=getRightCount(x, y); i<count; ++i, now_right_chess>>=2)
		{
			EVAL_KILL_PRIOR(now_right_chess);
		}
	}
	return kill_prior;
}


int AiThread::evalKilledScore(Chessid cur_player, int depth) const
{
	if (cur_player==AI_CHESS)
	{
//	①双方可有任何棋型
		if (all_type[I_AI_C5])
			return WIN_SCORE_FIX+4;			// 我方早就有5了，必胜
		if (all_type[I_HU_C5])
			return LOSE_SCORE_FIX-4;		// 对方有5，我方必败
		if (all_type[I_AI_A4] || all_type[I_AI_P4])
			return WIN_SCORE_FIX+3;			// 我方有四必胜
		if (all_type[I_HU_A4] || all_type[I_HU_P4]>=2)
			return LOSE_SCORE_FIX-3;		// 对方有活4或双冲四，我方必败

//	②双方没有5、对方可能有冲四、我方没有四
		if (all_type[I_HU_P4])
		{
			if (all_type[I_HU_A3] && !all_type[I_AI_A3] && !all_type[I_AI_S3])
				return LOSE_SCORE_FIX-2;	// 对方冲四活三，我方无三，我方必败
				// 注意，如果我方有眠三，下一步则可能会形成双冲四或者冲四活三再同时堵住对面的冲四，然后我方必胜；
				// 就算我方眠三下一步不能P4A3/2P4，也有可能我方下一步P4能顺带封住对面的P4，然后就不一定必败了
				// 又如果我方有活三，那显然更不必败了
		}

//	③双方没有5、4
		else
		{
			if (all_type[I_AI_A3])
				return WIN_SCORE_FIX+2;		// 对方没有4，我方有活三，我方必胜
			else if (all_type[I_HU_A3]>=2 && !all_type[I_AI_A3] && !all_type[I_AI_S3])
				return LOSE_SCORE_FIX-1;	// 对方有双活三，我方无三，我方必败
				// 注意，如果我方有眠三，下一步则可能会形成双冲四或者冲四活三，然后我方必胜；
				// 就算我方眠三下一步不能P4A3/2P4，也有可能我方下一步P4能顺带封住对面一个A3，然后就不一定必败了
				// 又如果我方有活三，那显然更不必败了
		}
	}
	else
	{
		if (all_type[I_HU_C5])
			return LOSE_SCORE_FIX-4;// 我方早就有5了，必胜
		if (all_type[I_AI_C5])
			return WIN_SCORE_FIX+4;// 对方有5，我方必败
		if (all_type[I_HU_A4] || all_type[I_HU_P4])
			return LOSE_SCORE_FIX-3;// 我方有四必胜
		if (all_type[I_AI_A4] || all_type[I_AI_P4]>=2)
			return WIN_SCORE_FIX+3;// 对方有活4或双冲四，我方必败

//	②双方没有5、对方可能有冲四、我方没有四
		if (all_type[I_AI_P4])
		{
			if (all_type[I_AI_A3] && !all_type[I_HU_A3] && !all_type[I_HU_S3])
				return WIN_SCORE_FIX+2;// 对方冲四活三，我方无三，我方必败
		}

//	③双方没有5、4
		else
		{
			if (all_type[I_HU_A3])
				return LOSE_SCORE_FIX-2;	// 对方没有4，我方有活三，我方必胜
			else if (all_type[I_AI_A3]>=2 && !all_type[I_HU_A3] && !all_type[I_HU_S3])
				return WIN_SCORE_FIX+1;		// 对方有双活三，我方无三，我方必败
		}
	}
	return 0;
}
