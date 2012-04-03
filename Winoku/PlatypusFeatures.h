#ifndef PLATYPUSFEATURES_H
#define PLATYPUSFEATURES_H

#include "Board.h"

class PlatypusFeatures
{
	public:
	PlatypusFeatures(const Board *board, Piece forPlayer) {
		this->board = board;
		this->player = forPlayer;
		Row2One = 0;
		Row2Both = 0;
		Row3One = 0;
		Row3Both = 0;
		Row4One = 0;
		Row4Both = 0;
		Row5 = 0;
		CountRows();
	}

	Piece player;
	int Row2One;
	int Row2Both;
	int Row3One;
	int Row3Both;
	int Row4One;
	int Row4Both;
	int Row5;

	private:
	const Board *board;
	static const int WinCount = 5;

	void Tally(int count, int leftCount, int rightCount) {
		int needed = WinCount - count;
		bool openLeft = leftCount >= needed;
		bool openRight = rightCount >= needed;
		if (openLeft && openRight) {
			switch (count) {
				case 2:
					Row2Both++;
					break;
				case 3:
					Row3Both++;
					break;
				case 4:
					Row4Both++;
					break;
			}
		} else if (openLeft || openRight) {
			switch (count) {
				case 2:
					Row2One++;
					break;
				case 3:
					Row3One++;
					break;
				case 4:
					Row4One++;
					break;
			}
		}
		if (count == 5) Row5++;
	}

	// Add 1 to count if the position is player
	bool CheckCount(int row, int col, int &count, bool visited[][BoardSize]) {
		if (row < 0 || row >= BoardSize) return false;
		if (col < 0 || col >= BoardSize) return false;
		if (count == WinCount) return false;

		if (board->GetPiece(row, col) == player) {
			visited[row][col] = true;
			count++;
			return true;
		}
		
		return false;
	}

	// Add 1 to open if the position is player or empty
	bool CheckOpen(int row, int col, int &openCount) {
		if (row < 0 || row >= BoardSize) return false;
		if (col < 0 || col >= BoardSize) return false;

		Piece piece = board->GetPiece(row, col);
		if (piece == player || piece == PieceNone) {
			openCount++;
			return true;
		}

		return false;
	}

	void CountRows(int Row, int Col, bool visited[][BoardSize]) {
		int count, r, c;

		// Check horizontal
		count = 0;
		c = Col;
		while (CheckCount(Row, c, count, visited)) c--;
		c = Col+1;
		while (CheckCount(Row, c, count, visited)) c++;
		
		int leftOpen = 0;
		c = Col-1;
		while (CheckOpen(Row, c, leftOpen)) c--;
		int rightOpen = 0;
		c = Col+count;
		while (CheckOpen(Row, c, rightOpen)) c++;
		
		Tally(count, leftOpen, rightOpen);
		
		// Check vertical
		count = 0;
		r = Row;
		while (CheckCount(r, Col, count, visited)) r--;
		r = Row+1;
		while (CheckCount(r, Col, count, visited)) r++;

		int topOpen = 0;
		r = Row-1;
		while (CheckOpen(r, Col, topOpen)) r--;
		int bottomOpen = 0;
		r = Row+count;
		while (CheckOpen(r, Col, bottomOpen)) r++;

		Tally(count, topOpen, bottomOpen);

		// Check diagonal \ 
		count = 0;
		r = Row;
		c = Col;
		while (CheckCount(r, c, count, visited)) {
			r--; c--;
		}
		r = Row+1;
		c = Col+1;
		while (CheckCount(r, c, count, visited)) {
			r++; c++;
		}
		int topLeftOpen = 0;
		r = Row-1;
		c = Col-1;
		while (CheckOpen(r, c, topLeftOpen)) {
			r--; c--;
		}
		int bottomRightOpen = 0;
		r = Row+count;
		c = Col+count;
		while (CheckOpen(r, c, bottomRightOpen)) {
			r++; c++;
		}

		Tally(count, topLeftOpen, bottomRightOpen);

		// Check diagonal /
		count = 0;
		r = Row;
		c = Col;
		while (CheckCount(r, c, count, visited)) {
			r--; c++;
		}
		r = Row+1;
		c = Col-1;
		while (CheckCount(r, c, count, visited)) {
			r++; c--;
		}
		int topRightOpen = 0;
		r = Row-1;
		c = Col+1;
		while (CheckOpen(r, c, topRightOpen)) {
			r--; c++;
		}
		int bottomLeftOpen = 0;
		r = Row+count;
		c = Col-count;
		while (CheckOpen(r, c, bottomLeftOpen)) {
			r++; c--;
		}

		Tally(count, topRightOpen, bottomLeftOpen);
	}

	void CountRows() {
		bool visited[BoardSize][BoardSize];
		for (int row = 0; row < BoardSize; row++)
			for (int col = 0; col < BoardSize; col++)
				visited[row][col] = false;

		for (int row=0; row < BoardSize; row++) {
			for (int col=0; col < BoardSize; col++) {
				if (board->GetPiece(row, col) == player && !visited[row][col]) {
					CountRows(row, col, visited);
				}
			}
		}
	}
};

#endif