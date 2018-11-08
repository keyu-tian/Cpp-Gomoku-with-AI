#include "main_window.h"

#include <QMessageBox>

void MainWindow::startPVEGame()
{
	now_player_id = black_player_id;
	chess_board.setBlackPlayerID(black_player_id);
	if (black_player_id == AI_CHESS)
	{
		ai_thread.setIsBlack();
		ai_thread.start();
	}
	else if (black_player_id == H1_CHESS)
	{
		ai_thread.setNotBlack();
	}
}

void MainWindow::startPVPGame()
{
	now_player_id = black_player_id;
	chess_board.setBlackPlayerID(black_player_id);
}

void MainWindow::resetPVEGame()
{
	chess_cnt = 0;
	mouse_cursor.x = mouse_cursor.y = INVALID_FLAG;

	chess_board.initBoard();
	ai_thread.initBoard();
	this->update();
}

void MainWindow::resetPVPGame()
{
	chess_cnt = 0;
	mouse_cursor.x = mouse_cursor.y = INVALID_FLAG;

	chess_board.initBoard();
	this->update();
}

void MainWindow::retractPVEGame()
{
	if (chess_cnt >= 3)
	{
		const Grid &last_hu_move = history[chess_cnt-2];
		const Grid &last_ai_move = history[chess_cnt-1];
		ai_thread.takeChess(last_hu_move.x, last_hu_move.y);
		ai_thread.takeChess(last_ai_move.x, last_ai_move.y);
		chess_board.takeChess(last_hu_move.x, last_hu_move.y);
		chess_board.takeChess(last_ai_move.x, last_ai_move.y);
		this->update();
		ai_thread.setNotWarned();

		if (chess_cnt == 3)
			ai_thread.resetRound();

		chess_cnt -= 2;
	}
	else
	{
		this->resetPVEGame();
		this->startPVEGame();
	}
}

void MainWindow::retractPVPGame()
{
	if (chess_cnt > 2)
	{
		const Grid &last_h1_move = history[chess_cnt-2];
		const Grid &last_h2_move = history[chess_cnt-1];
		chess_board.takeChess(last_h1_move.x, last_h1_move.y);
		chess_board.takeChess(last_h2_move.x, last_h2_move.y);
		chess_cnt -= 2;
		this->update();
	}
	else
	{
		this->resetPVPGame();
		this->startPVPGame();
	}
}

void MainWindow::PVERound()
{
	this->PVEPutChess(mouse_cursor, H1_CHESS);
	this->update();
	if (ai_thread.lose())
	{
		qDebug() << "ai lose";
		QMessageBox::information(this, "  真香警告(｀・ω・´)  ", "   你居然打败了窝~ (=´ω｀=)   ");
		this->resetPVEGame();
		this->startPVEGame();
	}
	else  // 如果人没有获胜，那就马上轮到AI落子
	{
		now_player_id = AI_CHESS;
		ai_thread.start();  // 副线程结束后再通过connect调用aiMove
	}
}

void MainWindow::aiMove(const Grid &ai_next_move)
{
	this->PVEPutChess(ai_next_move, AI_CHESS);
	this->update();
	if (ai_thread.win())
	{
		qDebug() << "ai win";
		QMessageBox::information(this, "  真香警告(｀・ω・´)  ", "   你已被本宝宝打败~ (*/ω＼*)  ");
		this->resetPVEGame();
		this->startPVEGame();
	}
	else now_player_id = H1_CHESS;
}

void MainWindow::PVPRound()
{
	this->PVPPutChess(mouse_cursor, now_player_id);
	this->update();
	if (now_player_id == H1_CHESS)
	{
		if (chess_board.win(mouse_cursor, H1_CHESS))
		{
			QMessageBox::information(this, "  真香警告(｀・ω・´)  ", "   黑子赢辣~ (=´ω｀=)   ");
			this->resetPVPGame();
			this->startPVPGame();
		}
		else now_player_id = H2_CHESS;
	}

	else if (now_player_id == H2_CHESS)
	{
		if (chess_board.win(mouse_cursor, H2_CHESS))
		{
			QMessageBox::information(this, "  真香警告(｀・ω・´)  ", "   白子赢辣~ (=´ω｀=)   ");
			this->resetPVPGame();
			this->startPVPGame();
		}
		else now_player_id = H1_CHESS;
	}
}

void MainWindow::PVEPutChess(const Grid &next_move, const Chessid id)
{
	if ( chess_board.isAvaliable(next_move) )
	{
		chess_board.putChess(next_move.x, next_move.y, id);
		ai_thread.putChess(next_move.x, next_move.y, id);
		history[chess_cnt++] = next_move;
	}
}

void MainWindow::PVPPutChess(const Grid &next_move, const Chessid id)
{
	if ( chess_board.isAvaliable(next_move) )
	{
		chess_board.putChess(next_move.x, next_move.y, id);
		history[chess_cnt++] = next_move;
	}
}

void MainWindow::sweetWarning()
{
	QMessageBox::information(this, "  真香警告(｀・ω・´)  ", "  真香警告(｀・ω・´)  ");
}
