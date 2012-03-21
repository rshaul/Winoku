#ifndef RANDOMAI_H
#define RANDOMAI_H

#include "Player.h"

class ExampleAI : public Player
{
	public:
	ExampleAI();
	void OpponentDidMove(int row, int col);
	void GetMove(int &row, int &col, int secondsRemaining);

	private:
	int board[19][19];
};

#endif