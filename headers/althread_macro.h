#ifndef AITHREAD_MACRO_H
#define AITHREAD_MACRO_H


// **************** 宏常量： ****************


// 棋型标志位（ F -- Flag ）
#define F_0			0		// 空棋型
#define F_AI_C5		0x1		// AI  连五
#define F_HU_C5		0x2		// Man 连五
#define F_AI_A4		0x4		// AI  活四
#define F_HU_A4		0x8		// Man 活四
#define F_AI_P4		0x10		// AI  冲四
#define F_HU_P4		0x20		// Man 冲四
#define F_AI_A3		0x40		// AI  活三
#define F_HU_A3		0x80		// Man 活三
#define F_AI_S3		0x100		// AI  眠三
#define F_HU_S3		0x200		// Man 眠三
#define F_AI_A2		0x400		// AI  活二
#define F_HU_A2		0x800		// Man 活二
#define F_AI_S2		0x1000		// AI  眠二
#define F_HU_S2		0x2000		// Man 眠二
#define F_AI_A1		0x4000		// AI  活一
#define F_HU_A1		0x8000		// Man 活一

#define F_C5		0x3		// 任何玩家连五
#define F_A4		0xc		// 任何玩家活四
#define F_P4		0x30		// 任何玩家冲四
#define F_A3		0xc0		// 任何玩家活三
#define F_S3		0x300		// 任何玩家眠三
#define F_A2		0xc00		// 任何玩家活二

// 棋型反标志位（用于清除棋型）
#define F_DAI_P4	0xffef		// 消除 AI 的冲四
#define F_DHU_P4	0xffdf		// 消除 Man的冲四
#define F_DAI_A3	0xffbf		// 消除 AI 的活三
#define F_DHU_A3	0xff7f		// 消除 Man的活三
#define F_DAI_S3	0xfeff		// 消除 AI 的眠三
#define F_DHU_S3	0xfdff		// 消除 Man的眠三

// 棋型对应下标（ I -- Index ）
#define I_0			0			// 空棋型
#define I_AI_C5		1			// AI  连五
#define I_HU_C5		2			// Man 连五
#define I_AI_A4		3			// AI  活四
#define I_HU_A4		4			// Man 活四
#define I_AI_P4		5			// AI  冲四
#define I_HU_P4		6			// Man 冲四
#define I_AI_A3		7			// AI  活三
#define I_HU_A3		8			// Man 活三
#define I_AI_S3		9			// AI  眠三
#define I_HU_S3		10			// Man 眠三
#define I_AI_A2		11			// AI  活二
#define I_HU_A2		12			// Man 活二
#define I_AI_S2		13			// AI  眠二
#define I_HU_S2		14			// Man 眠二
#define I_AI_A1		15			// AI  活一
#define I_HU_A1		16			// Man 活一

// 棋型对应分数
#define C5score     135000	 // 连五
#define A4score     25000	 // 活四
#define P4score     5201	 // 冲四
#define A3score		4991	 // 活三
#define S3score		410		 // 眠三 最初分
#define A2score		450		 // 活二 最初分
//#define S3score		400		 // 眠三 曾经改成过这个分数
//#define A2score		520		 // 活二 曾经改成过这个分数
#define S2score		70		 // 眠二
#define A1score		30		 // 活一

#define Plus44score	8500	 // 双冲四奖励
//#define Plus44score	10000	 // 历史双冲四奖励
#define Plus34score	6550	 // 冲四活三或者双活三奖励

#define CHESS_TYPE_N			17			// 棋型种数

#define MAX_INDEX_OF_M_TABLE 0xeaa+1		// 棋型哈希表大小
											// 最大的下标是 111010101010B（eaaH），也就是OUT_CHESS加上5个H1_CHESS

#define INF 0xffffff						// 极大极小搜索的初始化值

#define MASK 0xfff							// 一次性取出 6 个格子（12位）的 mask
#define WIN_SCORE_FIX C5score +10-depth		// AI 必胜时，AI 应优先选择最近杀棋，故层数越高分数越低
#define LOSE_SCORE_FIX -C5score -10+depth	// AI 必败时，AI 应延缓杀棋，故层数越高分数越高（绝对值越小）

#define KILL_PRIOR 768						// 存在双活三或者更强的棋型


// **************** 宏函数： ****************


#define RUSH3(x) for(x=0; x<3; ++x)
#define mem0(x) memset(x, 0, sizeof(x))
#define memf1(x) memset(x, -1, sizeof(x))
#define rand64() rand() ^ ((__int64)rand()<<15) ^ ((__int64)rand()<<30) ^ ((__int64)rand()<<45) ^ ((__int64)rand()<<60)

#define _L_LC(l) L_TO_LEFT_COUNT[l]
#define _XY_LC(x, y) XY_TO_LEFT_COUNT[x][y]
#define _generLC(_1, _2, __, ...) __
#define getLeftCount(...) _generLC(__VA_ARGS__, _XY_LC, _L_LC, ...)(__VA_ARGS__)

#define _R_RC(r) R_TO_RIGHT_COUNT[r]
#define _XY_RC(x, y) XY_TO_RIGHT_COUNT[x][y]
#define _generRC(_1, _2, __, ...) __
#define getRightCount(...) _generRC(__VA_ARGS__, _XY_RC, _R_RC, ...)(__VA_ARGS__)


// 初始化棋型哈希表
#define _init_M(index, QX) M_TABLE[index] = F_##QX
#define _init_M_P4(index, QX, p0) M_TABLE[index] = F_##QX, P4_KEY_POS_TABLE[index] = p0
#define _init_M_A3_2(index, QX, p0, p1) M_TABLE[index] = F_##QX, A3_KEY_POS_TABLE[index][0] = p0, A3_KEY_POS_TABLE[index][1] = p1
#define _init_M_A3_3(index, QX, p0, p1, p2) M_TABLE[index] = F_##QX, A3_KEY_POS_TABLE[index][0] = p0, A3_KEY_POS_TABLE[index][1] = p1, A3_KEY_POS_TABLE[index][2] = p2
#define _generInit(_1, _2, _3, _4, _5, __, ...) __
#define INIT(...) _generInit(__VA_ARGS__, _init_M_A3_3, _init_M_A3_2, _init_M_P4, _init_M, ...)(__VA_ARGS__)





// 在棋盘总棋型计数数组中 将某条直线上的所有棋型清除
#define _reset_(QX, temp_line_type, all)	if(temp_line_type & F_AI_##QX) --all[I_AI_##QX]; \
											if(temp_line_type & F_HU_##QX) --all[I_HU_##QX]
#define RESET(line_type, temp_line_type, all)		\
			_reset_(C5, temp_line_type, all);		\
			_reset_(A4, temp_line_type, all);		\
			_reset_(P4, temp_line_type, all);		\
			_reset_(A3, temp_line_type, all);		\
			_reset_(S3, temp_line_type, all);		\
			_reset_(A2, temp_line_type, all);		\
			_reset_(S2, temp_line_type, all);		\
			_reset_(A1, temp_line_type, all);		\
			line_type = 0





// 在棋盘总棋型计数数组中 加上某条直线上的所有棋型
#define _set_(QX, line_type, all)	if(line_type & F_AI_##QX) ++all[I_AI_##QX]; \
									if(line_type & F_HU_##QX) ++all[I_HU_##QX]
#define SET(line_type, all)					\
			_set_(C5, line_type, all);		\
			_set_(A4, line_type, all);		\
			_set_(P4, line_type, all);		\
			_set_(A3, line_type, all);		\
			_set_(S3, line_type, all);		\
			_set_(A2, line_type, all);		\
			_set_(S2, line_type, all);		\
			_set_(A1, line_type, all)





// 根据棋盘总棋型计数数组计算总分
#define _eval_(QX, sb, sw)		sb += all_type[I_AI_##QX] * QX##score; \
								sw += all_type[I_HU_##QX] * QX##score
#define EVAL(sb, sw)				\
			_eval_(C5, sb, sw);		\
			_eval_(A4, sb, sw);		\
			_eval_(P4, sb, sw);		\
			_eval_(A3, sb, sw);		\
			_eval_(S3, sb, sw);		\
			_eval_(A2, sb, sw);		\
			_eval_(S2, sb, sw);		\
			_eval_(A1, sb, sw)





// 避免棋型重复（有了活四则忽略冲四与活三，有了冲四则忽略活三与眠三）
#define __dl(line_type, BW)				\
	if (line_type & F_##BW##A4)			\
	{									\
		line_type &= F_D##BW##P4;		\
		if (line_type & F_##BW##S3)		\
			line_type &= F_D##BW##S3;	\
	}									\
	else if (line_type & F_##BW##P4)	\
	{									\
		if (line_type & F_##BW##A3)		\
			line_type &= F_D##BW##A3;	\
		if (line_type & F_##BW##S3)		\
			line_type &= F_D##BW##S3;	\
	}
#define DEAL_34(line_type)		\
	do							\
	{							\
		__dl(line_type, AI_)	\
		__dl(line_type, HU_)	\
	} while(0)





// 估计某条线上的 眠三及以上的棋型丰富程度
#define EVAL_MIN_MAX_PRIOR(line_chess)			\
		if(M_TABLE[line_chess & MASK] & F_C5)	\
			return 16384;						\
		if(M_TABLE[line_chess & MASK] & F_A4)	\
		{										\
			min_max_prior += 2048;				\
			break;								\
		}										\
		if(M_TABLE[line_chess & MASK] & F_P4)	\
			min_max_prior += 256;				\
		if(M_TABLE[line_chess & MASK] & F_A3)	\
			min_max_prior += 256;				\
		if(M_TABLE[line_chess & MASK] & F_A2)	\
			min_max_prior += 16;				\
		if(M_TABLE[line_chess & MASK] & F_S3)	\
			min_max_prior += 14
//		历史：min_max_prior += 30;




#endif // AITHREAD_MACRO_H
