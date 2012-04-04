#include "Board.h"
#include <algorithm>
#include <assert.h>
#include <iostream>

#include <Windows.h>

using namespace std;

Board::Board() {
	Clear();
}

void Board::Print() {
	for (int row=0; row < BoardSize; row++) {
		OutputDebugStringA("{");
		for (int col=0; col < BoardSize; col++) {
			if (col > 0) cout << " ";
			switch (GetPiece(row, col)) {
				case PiecePlayer1:
					OutputDebugStringA("1");
					break;
				case PiecePlayer2:
					OutputDebugStringA("2");
					break;
				default:
					OutputDebugStringA(" ");
					break;
			}
		}
		OutputDebugStringA("}\n");
	}
}

Board& Board::operator=(const Board &other) {
	pieceCount = other.pieceCount;
	memcpy(pieces, other.pieces, BoardSize*BoardSize*sizeof(Piece));
	return *this;
}

Board::Board(const Board &other) {
	pieceCount = other.pieceCount;
	memcpy(pieces, other.pieces, BoardSize*BoardSize*sizeof(Piece));
}

void Board::Clear() {
	pieceCount = 0;
	memset(pieces, PieceNone, BoardSize*BoardSize*sizeof(Piece));
}

/*
void BoundsCheck(int row, int col) {
	assert(row >= 0 && row < BoardSize);
	assert(col >= 0 && col < BoardSize);
}
*/

void Board::SetPiece(int row, int col, Piece piece) {
	//BoundsCheck(row, col);
	if (pieces[row*BoardSize+col] == PieceNone && piece != PieceNone) pieceCount++;
	pieces[row*BoardSize+col] = piece;
}

Piece Board::GetPiece(int row, int col) const {
	//BoundsCheck(row, col);
	return pieces[row*BoardSize+col];
}

int Board::NumberOfPieces() {
	return pieceCount;
}

bool Board::IsSolved(int row, int col, Winner &whoWon) const {
	//BoundsCheck(row, col);
	whoWon = WinnerNone;
	if (IsSolved(row, col, PiecePlayer1)) {
		whoWon = WinnerPlayer1;
	}
	if (IsSolved(row, col, PiecePlayer2)) {
		whoWon = WinnerPlayer2;
	}
	if (pieceCount == BoardSize*BoardSize) {
		whoWon = WinnerDraw;
	}
	return whoWon != WinnerNone;
}

bool Board::CheckCount(int row, int col, Piece piece, int &count) const {
	if (GetPiece(row, col) == piece) count++;
	else count = 0;
	return (count == WinCount);
}

bool Board::IsSolved(int pRow, int pCol, Piece piece) const {
	int count, row, col;

	// Check horizontal
	count = 0;
	for (int col=0; col < BoardSize; col++) {
		if (CheckCount(pRow, col, piece, count)) return true;
	}

	// Check vertical
	count = 0;
	for (int row=0; row < BoardSize; row++) {
		if (CheckCount(row, pCol, piece, count)) return true;
	}

	// Check Diagonal \ -- Start at the top left
	count = 0;
	col = max(pCol - pRow, 0);
	row = pRow - (pCol - col);
	while (row < BoardSize && col < BoardSize) {
		if (CheckCount(row, col, piece, count)) return true;
		row++;
		col++;
	}

	// Check Diagonal / -- Start at the top right
	count = 0;
	col = min(pCol + pRow, BoardSize-1);
	row = pRow - (col - pCol);
	while (row < BoardSize && col >= 0) {
		if (CheckCount(row, col, piece, count)) return true;
		row++;
		col--;
	}

	return false;
}