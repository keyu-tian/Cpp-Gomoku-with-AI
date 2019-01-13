#ifndef AITHREAD_H
#define AITHREAD_H

#include <QThread>
#include "base.h"
#include "aithread_macro.h"

class AiThread : public QThread
{
	Q_OBJECT

	private:

		// 记录棋局轮次 和 经过计算得到的ai下一次落子位置
		int round;
		bool is_black;
		bool warned;
		Grid ai_next_move;

		int prev_score;

		// 棋盘的备份（为了更快读写）
		Chessid mirror_board[GRID_N][GRID_N];

		/* 记录某条直线上的棋子，每2个bit表示一颗棋子
		 *  列：最左的是第0列，每列靠上的是低位，扫描次数10
		 *  行：最上的是第0行，每行靠左的是低位，扫描次数10
		 *  左斜（左上右下）：最左下的是第0左斜列，靠左上的是低位，扫描次数与位置有关
		 *  右斜（左下右上）：最左上的是第0右斜列，靠右上的是低位，扫描次数与位置有关
		 */
		__int32 col_chess[GRID_N];
		__int32 row_chess[GRID_N];
		__int32 left_chess[GRID_DN];
		__int32 right_chess[GRID_DN];

		// 记录每条直线上的棋型，每 1 个 __int32 表示一条直线上的棋型
		__int32 col_type[GRID_N];
		__int32 row_type[GRID_N];
		__int32 left_type[GRID_DN];
		__int32 right_type[GRID_DN];

		// 记录整个棋盘的棋型，每 1 个 int 表示一种棋型的个数
		int all_type[CHESS_TYPE_N];

		// 棋型标志位映射表
		int M_TABLE[MAX_INDEX_OF_M_TABLE];
		int P4_KEY_POS_TABLE[MAX_INDEX_OF_M_TABLE];
		int A3_KEY_POS_TABLE[MAX_INDEX_OF_M_TABLE][3];

		// 获取斜列扫描次数映射表
		int XY_TO_LEFT_COUNT[GRID_N][GRID_N];
		int XY_TO_RIGHT_COUNT[GRID_N][GRID_N];
		int L_TO_LEFT_COUNT[GRID_DN];
		int R_TO_RIGHT_COUNT[GRID_DN];

	protected:

		// run函数
		virtual void run();

	public:

		// 构造函数
		explicit AiThread(QObject *parent = nullptr);

		// 初始化接口函数
		void initHashTable();
		void initBoard();
		void setIsBlack();
		void setNotBlack();
		void setNotWarned();
		void resetRound();

		// 棋盘接口函数
		void putChess(const int x, const int y, const Chessid id);
		void takeChess(const int x, const int y);
		bool win();
		bool lose();
		void printChessType();

		void findP4(int &x, int &y, const Chessid cur_player);

	private:

		// 初始化函数：
		void initMTable();
		void initCountTable();

		// 基础函数：
		void updateChessType(const int x, const int y);

		// 估值函数：
		int evalBoard();
		int evalMinMaxPrior(const int x, const int y, const Chessid id);
		int evalKillPrior(const int x, const int y, const Chessid id);
		int evalKilledScore(Chessid cur_player, int depth);

		// 算法函数：
		int minMaxSearch(Chessid cur_player, int depth, int alpha, int beta);
		int killSearch(Chessid cur_player, int depth, int alpha, int beta);
		int getMinMaxSearchChoices(Choice *choices_buffer, Chessid cur_player);
		int getKillSearchChoices(Choice *choices_buffer, Chessid cur_player);
		void myQuickSort(Choice *a, int n);

		// 特殊局面函数：
		void firstRound();
		void secondRound();
		void highestPriorityAttack();
		void highestPriorityDefence();

	signals:

		// 线程间信号传输
		void foundNextMove(const Grid &ai_next_move);
		void sweetWarning();

};

#endif // AITHREAD_H
