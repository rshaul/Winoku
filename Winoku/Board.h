#ifndef BOARD_H
#define BOARD_H

typedef enum {
	PieceNone,
	PiecePlayer1,
	PiecePlayer2
} Piece;

typedef enum {
	WinnerNone,
	WinnerPlayer1,
	WinnerPlayer2,
	WinnerDraw
} Winner;

// Board class is used to check for winners

static const int BoardSize = 19;
static const int WinCount = 5;

class Board
{
	public:
	Board();
	Board(const Board &other);
	Board& operator= (const Board &other);

	void SetPiece(int row, int col, Piece piece);
	Piece GetPiece(int row, int col) const;
	void Clear();
	int NumberOfPieces();
	
	// Checks to see if there was a winner at a position
	// Returns true if game is over
	bool IsSolved(int row, int col, Winner &whoWon) const;

	void Print();

	private:
	int pieceCount;
	Piece pieces[BoardSize*BoardSize];
	bool IsSolved(int row, int col, Piece piece) const;
	bool CheckCount(int row, int col, Piece piece, int &count) const;
};

#endif