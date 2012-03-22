#ifndef PLAYER_H
#define PLAYER_H

class Player
{
	public:
	// Called by the interface to get your AI's next move
	// row & col:
	//		Set these to your AI's choice
	// secondsRemaining:
	//		How much time left on the clock your AI has to complete the game
	//		For example if the clock is 3 minutes, starts at 180 and lowers with each subsequent call
	virtual void GetMove(int &row, int &col, int secondsRemaining) = 0;

	// Called by the interface after your opponent has made a move
	// row & col: The position your opponent chose
	virtual void OpponentDidMove(int row, int col) = 0;

	// Note:
	// Either GetMove or OpponentDidMove may be called first
	// depending on if you are going first or second
};

#endif