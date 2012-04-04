#include "Platypus.h"
#include "AlphaBeta.h"
#include "PlatypusFeatures.h"

#include <Windows.h>

class PlatypusNode;
bool PlatypusNodeSorter(PlatypusNode *a, PlatypusNode *b);


// FIND EMPTY BOARD PIECES

int Clamp(int pos) {
	if (pos < 0) return 0;
	if (pos >= BoardSize) return BoardSize-1;
	return pos;
}

void GetEmptyAround(Board *board, int row, int col, bool empty[BoardSize][BoardSize]) {
	const int depth = 1;
	for (int r=Clamp(row-depth); r <= Clamp(row+depth); r++) {
		for (int c=Clamp(col-depth); c <= Clamp(col+depth); c++) {
			if (board->GetPiece(r, c) == PieceNone) {
				empty[r][c] = true;
			}
		}
	}
}

void GetEmptySpotsAroundPlayers(Board *board, bool empty[BoardSize][BoardSize]) {
	for (int row=0; row < BoardSize; row++)
		for (int col=0; col < BoardSize; col++)
			empty[row][col] = false;

	for (int row=0; row < BoardSize; row++) {
		for (int col=0; col < BoardSize; col++) {
			if (board->GetPiece(row, col) != PieceNone) {
				GetEmptyAround(board, row, col, empty);
			}
		}
	}
}

class PlatypusNode
{
	public:
	bool IsTerminal() {
		Winner win;
		return board->IsSolved(moveRow, moveCol, win);
	}

	float HeuristicForPlayer(Piece player) {
		PlatypusFeatures features(board, player);
		const float score2one = 1;
		const float score2both = 4;
		const float score3one = 2;
		const float score3both = 8;
		const float score4one = 0;
		const float score4both = 100;
		const float score5 = 10000;
		return (features.Row2One * score2one) + (features.Row2Both * score2both)
				+ (features.Row3One * score3one) + (features.Row3Both * score3both)
				+ (features.Row4One * score4one) + (features.Row4Both * score4both)
				+ (features.Row5 * score5);
	}

	float ValueHeuristic() {
		if (valueHeuristic == FLT_MAX) {
			float p1 = HeuristicForPlayer(PiecePlayer1);
			float p2 = HeuristicForPlayer(PiecePlayer2);
			valueHeuristic = p1 - p2;
		}
		return valueHeuristic;
	}

	int moveRow;
	int moveCol;
	Piece movePlayer;

	void Init(Board *board) {
		this->board = board;
		this->moveRow = 0;
		this->moveCol = 0;
		this->movePlayer = PiecePlayer1;
		this->valueHeuristic = FLT_MAX;
	}
	void Init(const PlatypusNode &other) {
		this->board = new Board(*other.board);
		this->moveRow = other.moveRow;
		this->moveCol = other.moveCol;
		this->movePlayer = other.movePlayer;
		this->valueHeuristic = FLT_MAX;
	}

	PlatypusNode() {
		Init(new Board());
	}
	PlatypusNode(const PlatypusNode &other) {
		Init(other);
	}
	PlatypusNode& operator=(const PlatypusNode &other) {
		delete board;
		assert(nodes.size() == 0);
		Init(other);
		return *this;
	}
	PlatypusNode(Board *board) {
		Init(new Board(*board));
	}
	~PlatypusNode() {
		delete board;
		for (unsigned int i=0; i < nodes.size(); i++) {
			delete nodes[i];
		}
	}

	private:
	Board *board;
	vector<PlatypusNode*> nodes;
	float valueHeuristic;

	Piece OtherPlayer() {
		return (movePlayer == PiecePlayer1) ? PiecePlayer2 : PiecePlayer1;
	}

	void BuildChildren() {
		bool empty[BoardSize][BoardSize];

		GetEmptySpotsAroundPlayers(board, empty);

		for (int row=0; row < BoardSize; row++) {
			for (int col=0; col < BoardSize; col++) {
				if (empty[row][col]) {
					PlatypusNode *node = new PlatypusNode(board);
					node->moveRow = row;
					node->moveCol = col;
					node->movePlayer = OtherPlayer();
					node->board->SetPiece(row, col, movePlayer);
					nodes.push_back(node);
				}
			}
		}

		//sort(nodes.begin(), nodes.end(), PlatypusNodeSorter);
	}
};

bool PlatypusNodeSorter(PlatypusNode *a, PlatypusNode *b) {
	return a->ValueHeuristic() < b->ValueHeuristic();
}

void Platypus::OpponentDidMove(int row, int col) {
	board.SetPiece(row, col, PiecePlayer2);
}

float AlphaBeta(Board *board, int depth, float alpha, float beta, bool maxNode) {
	return 0;
}

void Platypus::GetMove(int &row, int &col, int secondsLeft) {
	if (board.NumberOfPieces() == 0) {
		row = 9;
		col = 9;
	} else {
		boardsExamined = 0;
		/*
		bool empty[BoardSize][BoardSize];
		GetEmptySpotsAroundPlayers(&board, empty);

		PlatypusNode* maxChild = NULL;
		float maxValue = INT_MIN;
		for (int row=0; row < BoardSize; row++) {
			for (int col=0; col < BoardSize; col++) {
				if (empty[row][col]) {
					board.SetPiece(row, col, PiecePlayer1);
					float value = AlphaBeta(&board, 3, 
				}
			}
		}
		for (unsigned int i=0; i < children.size(); i++) {
			PlatypusNode *child = children[i];
			float value = AlphaBetaN(child, 3, false);
			if (value >= maxValue) {
				maxValue = value;
				maxChild = child;
			}
		}
		char *buffer = new char[1000];
		OutputDebugStringA("BOARDS EXAMINED: ");
		_itoa_s(boardsExamined, buffer, 1000, 10);
		OutputDebugStringA(buffer);
		OutputDebugStringA("\n");
		delete buffer;

		//PlatypusNode child = AlphaBetaSearch(&origin, 2);
		row = maxChild->moveRow;
		col = maxChild->moveCol;
		*/
	}
	board.SetPiece(row, col, PiecePlayer1);
}