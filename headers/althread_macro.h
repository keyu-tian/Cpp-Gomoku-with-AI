#ifndef AITHREAD_MACRO_H
#define AITHREAD_MACRO_H


// **************** 宏常量： ****************

// 棋型掩码（ M -- Mask ）
#define M_0		0		// 空棋型
#define M_AI_C5		0x1		// AI  连五
#define M_HU_C5		0x2		// Man 连五
#define M_AI_A4		0x4		// AI  活四
#define M_HU_A4		0x8		// Man 活四
#define M_AI_P4		0x10		// AI  冲四
#define M_HU_P4		0x20		// Man 冲四
#define M_AI_A3		0x40		// AI  活三
#define M_HU_A3		0x80		// Man 活三
#define M_AI_S3		0x100		// AI  眠三
#define M_HU_S3		0x200		// Man 眠三
#define M_AI_A2		0x400		// AI  活二
#define M_HU_A2		0x800		// Man 活二
#define M_AI_S2		0x1000		// AI  眠二
#define M_HU_S2		0x2000		// Man 眠二
#define M_AI_A1		0x4000		// AI  活一
#define M_HU_A1		0x8000		// Man 活一

#define M_C5		0x3		// 任何玩家连五
#define M_A4		0xc		// 任何玩家活四
#define M_P4		0x30		// 任何玩家冲四
#define M_A3		0xc0		// 任何玩家活三
#define M_S3		0x300		// 任何玩家眠三
#define M_A2		0xc00		// 任何玩家活二

// 棋型反掩码（用于清除棋型）
#define M_DAI_P4	0xffef		// 消除 AI 的冲四
#define M_DHU_P4	0xffdf		// 消除 Man的冲四
#define M_DAI_A3	0xffbf		// 消除 AI 的活三
#define M_DHU_A3	0xff7f		// 消除 Man的活三
#define M_DAI_S3	0xfeff		// 消除 AI 的眠三
#define M_DHU_S3	0xfdff		// 消除 Man的眠三

// 棋型对应下标（ I -- Index ）
#define I_0		0		// 空棋型
#define I_AI_C5		1		// AI  连五
#define I_HU_C5		2		// Man 连五
#define I_AI_A4		3		// AI  活四
#define I_HU_A4		4		// Man 活四
#define I_AI_P4		5		// AI  冲四
#define I_HU_P4		6		// Man 冲四
#define I_AI_A3		7		// AI  活三
#define I_HU_A3		8		// Man 活三
#define I_AI_S3		9		// AI  眠三
#define I_HU_S3		10		// Man 眠三
#define I_AI_A2		11		// AI  活二
#define I_HU_A2		12		// Man 活二
#define I_AI_S2		13		// AI  眠二
#define I_HU_S2		14		// Man 眠二
#define I_AI_A1		15		// AI  活一
#define I_HU_A1		16		// Man 活一

// 棋型对应分数
#define C5score		135000		// 连五
#define A4score		25000		// 活四
#define P4score		5200		// 冲四
#define A3score		4990		// 活三
#define S3score		410		// 眠三 最初分
#define A2score		450		// 活二 最初分
#define S2score		70		// 眠二
#define A1score		30		// 活一

#define Plus44score	10000	 	// 双冲四奖励
#define Plus34score	6550	 	// 冲四活三或者双活三奖励

#define CHESS_TYPE_N	17		// 棋型种数

#define MAX_INDEX_OF_M_TABLE 0xaab	// 棋型哈希表最大下标

#define INF 9999999			// 极大极小搜索的初始化值

#define MASK 0xfff				// 一次性取出 6 个格子（12位）的 mask
#define WIN_SCORE_FIX C5score +10-depth		// AI 必胜时，AI 应优先选择最近杀棋，故层数越高分数越低
#define LOSE_SCORE_FIX -C5score -10+depth	// AI 必败时，AI 应延缓杀棋，故层数越高分数越高（绝对值越小）




// **************** 宏函数： ****************

#define RUSH3(x) for(x=0; x<3; ++x)
#define mem0(x) memset(x, 0, sizeof(x))
#define rand64() rand() ^ ((__int64)rand()<<15) ^ ((__int64)rand()<<30) ^ ((__int64)rand()<<45) ^ ((__int64)rand()<<60)




// 初始化棋型哈希表
#define _ini(x, QX) M_TABLE[x] = M_##QX





// 在棋盘总棋型计数数组中 将某条直线上的所有棋型清除
#define _reset_(QX, line_i_type)	if(line_i_type & M_AI_##QX) --all_type[I_AI_##QX]; \
					if(line_i_type & M_HU_##QX) --all_type[I_HU_##QX]
#define RESET(line, x, line_i_type)				\
			_reset_(C5, line_i_type);		\
			_reset_(A4, line_i_type);		\
			_reset_(P4, line_i_type);		\
			_reset_(A3, line_i_type);		\
			_reset_(S3, line_i_type);		\
			_reset_(A2, line_i_type);		\
			_reset_(S2, line_i_type);		\
			_reset_(A1, line_i_type);		\
			line##_type[x] = 0





// 在棋盘总棋型计数数组中 加上某条直线上的所有棋型
#define _set_(QX, line_i_type)	if(line_i_type & M_AI_##QX) ++all_type[I_AI_##QX]; \
				if(line_i_type & M_HU_##QX) ++all_type[I_HU_##QX]
#define SET(line_i_type)				\
			_set_(C5, line_i_type);		\
			_set_(A4, line_i_type);		\
			_set_(P4, line_i_type);		\
			_set_(A3, line_i_type);		\
			_set_(S3, line_i_type);		\
			_set_(A2, line_i_type);		\
			_set_(S2, line_i_type);		\
			_set_(A1, line_i_type)





// 统计某条直线上的所有棋型 并计算总分
#define _plus_(QX, line_i_type, sb, sw)	if(line_i_type & M_AI_##QX) sb += QX##score;	\
					if(line_i_type & M_HU_##QX) sw += QX##score
#define PLUS(line_i_type, sb, sw)				\
			_plus_(C5, line_i_type, sb, sw);	\
			_plus_(A4, line_i_type, sb, sw);	\
			_plus_(P4, line_i_type, sb, sw);	\
			_plus_(A3, line_i_type, sb, sw);	\
			_plus_(S3, line_i_type, sb, sw);	\
			_plus_(A2, line_i_type, sb, sw);	\
			_plus_(S2, line_i_type, sb, sw);	\
			_plus_(A1, line_i_type, sb, sw)





// 根据棋盘总棋型计数数组计算总分
#define _eval_(QX, sb, sw)		sb += all_type[I_AI_##QX] * QX##score; \
					sw += all_type[I_HU_##QX] * QX##score
#define EVAL(sb, sw)					\
			_eval_(C5, sb, sw);		\
			_eval_(A4, sb, sw);		\
			_eval_(P4, sb, sw);		\
			_eval_(A3, sb, sw);		\
			_eval_(S3, sb, sw);		\
			_eval_(A2, sb, sw);		\
			_eval_(S2, sb, sw);		\
			_eval_(A1, sb, sw)





// 避免棋型重复（有了活四则忽略冲四与活三，有了冲四则忽略活三与眠三）
#define __dl(line, x, BW)				\
	if (line##_type[x] & M_##BW##A4)		\
	{						\
		line##_type[x] &= M_D##BW##P4;		\
		if (line##_type[x] & M_##BW##S3)	\
			line##_type[x] &= M_D##BW##S3;	\
	}						\
	else if (line##_type[x] & M_##BW##P4)		\
	{						\
		if (line##_type[x] & M_##BW##A3)	\
			line##_type[x] &= M_D##BW##A3;	\
		if (line##_type[x] & M_##BW##S3)	\
			line##_type[x] &= M_D##BW##S3;	\
	}
#define DEAL_34(line, x)				\
	do {						\
		__dl(line, x, AI_)			\
		__dl(line, x, HU_)			\
	} while(0)





// 估计某条线上的 眠三及以上的棋型丰富程度
#define EVAL_PRIOR(line)				\
		if(M_TABLE[now_##line & MASK] & M_C5)	\
			return 16384;			\
		if(M_TABLE[now_##line & MASK] & M_A4)	\
		{					\
			sum_score += 1024;		\
			break;				\
		}					\
		if(M_TABLE[now_##line & MASK] & M_P4)	\
		{					\
			sum_score += 256;		\
			break;				\
		}					\
		if(M_TABLE[now_##line & MASK] & M_A3)	\
		{					\
			sum_score += 256;		\
			break;				\
		}					\
		if(M_TABLE[now_##line & MASK] & M_A2)	\
		{					\
			sum_score += 40;		\
			break;				\
		}					\
		if(M_TABLE[now_##line & MASK] & M_S3)	\
		{					\
			sum_score += 30;		\
			break;				\
		}





// 估计某条线上的杀棋棋型丰富程度
#define EVAL_KILL_PRIOR(line)				\
		if(M_TABLE[now_##line & MASK] & M_C5)	\
			return 100;			\
		if(M_TABLE[now_##line & MASK] & M_A4)	\
		{					\
			kill_num += 16;			\
			break;				\
		}					\
		if(M_TABLE[now_##line & MASK] & M_P4)	\
		{					\
			++kill_num;			\
			break;				\
		}					\
		if(M_TABLE[now_##line & MASK] & M_A3)	\
		{					\
			++kill_num;			\
			break;				\
		}



#endif // AITHREAD_MACRO_H
