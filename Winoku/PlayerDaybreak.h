#ifndef PLAYERDAYBREAK_H
#define PLAYERDAYBREAK_H

#include "Player.h"
#include "Board.h"

class PlayerDaybreak : public Player
{
	public:
	PlayerDaybreak();
	void OpponentDidMove(int row, int col);
	void GetMove(int &row, int &col, int secondsRemaining);

	private:
	Board board;
	bool IsPlayerRow3(int &row, int &col);
	void GetRandom(int &row, int &col);
};

#endif