#include "ExampleAI.h"
#include <stdlib.h>
#include <Windows.h>

#define BoardSize 19

ExampleAI::ExampleAI() {
	for (int row=0; row < BoardSize; row++)
		for (int col=0; col < BoardSize; col++)
			board[row][col] = 0;
}

void ExampleAI::OpponentDidMove(int row, int col) {
	// Record opponents move
	board[row][col] = 2;
}

void ExampleAI::GetMove(int &row, int &col, int secondsRemaining) {
	// Find unoccupied spot randomly
	do {
		row = rand() % BoardSize;
		col = rand() % BoardSize;
	} while (board[row][col] != 0);
	// Record this move
	board[row][col] = 1;
	// Pretend like this took some work
	Sleep(50);
}