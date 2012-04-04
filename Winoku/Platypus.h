#ifndef PLATYPUS_H
#define PLATYPUS_H

#include "Board.h"
#include "Player.h"

class PlatypusNode;

class Platypus : public Player
{
	void GetMove(int &row, int &col, int secondsLeft);
	void OpponentDidMove(int row, int col);

	private:
	Board board;
};

#endif