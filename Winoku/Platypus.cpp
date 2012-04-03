#include "Platypus.h"
#include "AlphaBeta.h"
#include "PlatypusFeatures.h"

#include <Windows.h>

class PlatypusNode;
bool PlatypusNodeSorter(PlatypusNode *a, PlatypusNode *b);

class PlatypusNode : public SearchNode<PlatypusNode>
{
	public:
	bool IsTerminal() {
		Winner win;
		return board->IsSolved(moveRow, moveCol, win);
	}

	float ValueHeuristic() {
		if (valueHeuristic < 0) {
			PlatypusFeatures otherFeatures(board, OtherPlayer());
			PlatypusFeatures features(board, movePlayer);

			if (otherFeatures.Row5 > 0) {
				valueHeuristic = -FLT_MAX;
			} else if (otherFeatures.Row4Both > 0) {
				valueHeuristic = -FLT_MAX/100;
			} else if (otherFeatures.Row3Both > 0) {
				valueHeuristic = -FLT_MAX/1000;
			} else if (features.Row5 > 0) {
				valueHeuristic = FLT_MAX;
			} else if (features.Row4Both > 0) {
				valueHeuristic = FLT_MAX/100;
			} else {
				float score2one = 1;
				float score2both = 4;
				float score3one = 2;
				float score3both = 8;
				float score4one = 0;

				valueHeuristic =
					(features.Row2One * score2one) + (features.Row2Both * score2both)
					+ (features.Row3One * score3one) + (features.Row3Both * score3both)
					+ (features.Row4One * score4one);

				/* DEBUG
				board->Print();
				char buffer[20];
				if (movePlayer == PiecePlayer1) {
					OutputDebugStringA("P1 SCORE: ");
				} else {
					OutputDebugStringA("P2 SCORE: ");
				}
				_itoa_s((int)valueHeuristic, buffer, 20, 10);
				OutputDebugStringA(buffer);
				OutputDebugStringA("\n");
				//*/
			}
		}
		return valueHeuristic;
	}

	vector<PlatypusNode*> Children() {
		if (nodes.size() == 0) BuildChildren();
		return nodes;
	}

	int moveRow;
	int moveCol;
	Piece movePlayer;

	void Init(Board *board) {
		this->board = board;
		this->moveRow = 0;
		this->moveCol = 0;
		this->movePlayer = PiecePlayer1;
		this->valueHeuristic = -1;
	}
	void Init(const PlatypusNode &other) {
		this->board = new Board(*other.board);
		this->moveRow = other.moveRow;
		this->moveCol = other.moveCol;
		this->movePlayer = other.movePlayer;
		this->valueHeuristic = -1;
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
		for (int row=0; row < BoardSize; row++)
			for (int col=0; col < BoardSize; col++)
				empty[row][col] = false;

		GetEmptySpotsAroundPlayers(empty);

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

		sort(nodes.begin(), nodes.end(), PlatypusNodeSorter);
	}

	void GetEmptySpotsAroundPlayers(bool empty[BoardSize][BoardSize]) {
		for (int row=0; row < BoardSize; row++) {
			for (int col=0; col < BoardSize; col++) {
				if (board->GetPiece(row, col) != PieceNone) {
					GetEmptyAround(row, col, empty);
				}
			}
		}
	}

	int Clamp(int pos) const {
		if (pos < 0) return 0;
		if (pos >= BoardSize) return BoardSize-1;
		return pos;
	}

	void GetEmptyAround(int row, int col, bool empty[BoardSize][BoardSize]) {
		const int depth = 1;
		for (int r=Clamp(row-depth); r <= Clamp(row+depth); r++) {
			for (int c=Clamp(col-depth); c <= Clamp(col+depth); c++) {
				if (board->GetPiece(r, c) == PieceNone) {
					empty[r][c] = true;
				}
			}
		}
	}
};

bool PlatypusNodeSorter(PlatypusNode *a, PlatypusNode *b) {
	return a->ValueHeuristic() > b->ValueHeuristic();
}

void Platypus::OpponentDidMove(int row, int col) {
	board.SetPiece(row, col, PiecePlayer2);
}

void Platypus::GetMove(int &row, int &col, int secondsLeft) {
	if (board.NumberOfPieces() == 0) {
		row = 9;
		col = 9;
	} else {
		PlatypusNode origin(&board);
		PlatypusNode child = AlphaBetaSearch(&origin, 2);
		row = child.moveRow;
		col = child.moveCol;
	}
	board.SetPiece(row, col, PiecePlayer1);
}