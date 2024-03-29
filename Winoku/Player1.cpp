
// Player 1 color = 'P' for purple
// Player 2 color = 'T'	for tan

#include <string>
#include <algorithm>
#include "Player1.h"

Player1::Player1() 
{
	test1 = 10;
	boardSize = 0;
	boardsExamined = 0;

	// initialize player 1 19x19 board with all 'E' for empty 
	for (int row=0; row < BoardSize; row++)
		for (int col=0; col < BoardSize; col++)
			pOneBoard[row][col] = 'E';
}


//--------------------------------------------------------------

// helper f() for checkEnds. finds value of a consecutive run
// of like colored moves. 2, 3, 4


int Player1::totalEnds(int &line, char &leftVal, char &rightVal)
{
	int ret; // added to alpha heur, subtracted from beta heur

	if (leftVal == 'B' && rightVal == 'B')
	{
		//meant to be bad for either player
		//ret = -75 * line;
		ret = 0;
	}
	if (leftVal == 'B' && rightVal == 'O') 
		ret = 25 * line;	//MAX 300
	if (leftVal == 'O' && rightVal == 'B')
		ret = 25 * line;	//MAX 300
	if (leftVal == 'O' && rightVal == 'O')
		ret = 75 * line;	//MAX 300

	return ret;
}

//--------------------------------------------------------------

//	*** IMPORTANT *** 

// row and col are the coord's of the last node to enter the
// consecutive run of like colored moves

// check for 3 different cases
// open on both ends	- OXXO
// open on one end		- YXXO or OXXY
// closed on both ends	- YXXY

// dir - direction line segment is facing
// H  - left (towards 0) is beginning, right is end
// V  - up (towards 0) is beg. , right is end
// D1 - BL - goes right and down
// D2 - TR - goes left and up
// D3 - BR - goes left and down
// D4 - TL - goes right and up

int Player1::checkEnds(char &dir, int &line, int &row, int &col)
{
	int rval;	char back, front;

	if (dir == 'H')			// Horizontal
	{						// front: right
		// back
		if ( (col - line) < 0)
			back = 'B';	// for blocked
		else if (pOneBoard[row][col-line] == 'E')
			back = 'O';
		else	// space filled by opponent 
			back = 'B';

		// front
		if ( (col + 1) == 19)
			front = 'B';	// for blocked
		else if (pOneBoard[row][col + 1] == 'E')
			front = 'O';
		else	// space filled by opponent 
			front = 'B';

		rval = totalEnds(line, back, front);
	}
	else if (dir == 'V')	// Vertical
	{						// front: down
		// back
		if ( (row - line) < 0)
			back = 'B';	// for blocked
		else if (pOneBoard[row-line][col] == 'E')
			back = 'O';
		else // space filled by opponent 
			back = 'B';

		// front
		if ( (row + 1) == 19)
			front = 'B';	// for blocked
		else if (pOneBoard[row + 1][col] == 'E')
			front = 'O';
		else	// space filled by opponent 
			front = 'B';

		rval = totalEnds(line, back, front);
	}
	else if (dir == '1')	// BL triangular quadrant 
	{						// front: down and right
		// back
		if ( (col - line) < 0)	// |<--
			back = 'B';	// for blocked	___ 0
		else if ( (row - line) < 0)	//   ^
			back = 'B';	// for blocked	 |	
		else if (pOneBoard[row-line][col-line] == 'E')
			back = 'O';
		else // space filled by opponent 
			back = 'B';

		// front
		if ( (row + 1) == 19)
			front = 'B';	// for blocked
		else if ( (col + 1) == 19)
			front = 'B';	// for blocked
		else if (pOneBoard[row + 1][col + 1] == 'E')
			front = 'O';
		else	// space filled by opponent 
			front = 'B';

		rval = totalEnds(line, back, front);
	}
	else if (dir == '2')	// TR triangular quadrant 
	{						// front: left and up
		// back
		if ( (col + line) < 0)		// right
			back = 'B';	// for blocked	
		else if ( (row + line) < 0)	// down  
			back = 'B';	// for blocked	 	
		else if (pOneBoard[row + line][col + line] == 'E')
			back = 'O';
		else // space filled by opponent 
			back = 'B';

		// front
		if ( (row - 1) == 19)		// up
			front = 'B';	// for blocked
		else if ( (col - 1) == 19)	// left
			front = 'B';	// for blocked
		else if (pOneBoard[row - 1][col - 1] == 'E')
			front = 'O';
		else	// space filled by opponent 
			front = 'B';

		rval = totalEnds(line, back, front);
	}
	else if (dir == '3')	// BR triangular quadrant 
	{						// front: left and down
		// back
		if ( (col + line) < 0)		// right
			back = 'B';	// for blocked	
		else if ( (row - line) < 0)	// up 
			back = 'B';	// for blocked	 	
		else if (pOneBoard[row - line][col+line] == 'E')
			back = 'O';
		else // space filled by opponent 
			back = 'B';

		// front
		if ( (row + 1) == 19)		// down
			front = 'B';	// for blocked
		else if ( (col - 1) == 19)	// left
			front = 'B';	// for blocked
		else if (pOneBoard[row + 1][col - 1] == 'E')
			front = 'O';
		else	// space filled by opponent 
			front = 'B';

		rval = totalEnds(line, back, front);
	}
	else if (dir == '4')	// TL triangular quadrant 
	{						// front: right and up
		// back
		if ( (col - line) < 0)		// left 
			back = 'B';	// for blocked	
		else if ( (row + line) < 0)	// down   
			back = 'B';	// for blocked	 	
		else if (pOneBoard[row + line][col - line] == 'E')
			back = 'O';
		else // space filled by opponent 
			back = 'B';

		// front
		if ( (row - 1) == 19)		// up
			front = 'B';	// for blocked
		else if ( (col + 1) == 19)	// right	
			front = 'B';	// for blocked
		else if (pOneBoard[row - 1][col + 1] == 'E')
			front = 'O';
		else	// space filled by opponent 
			front = 'B';

		rval = totalEnds(line, back, front);;
	}


	return rval;
}

//--------------------------------------------------------------

// helper f() for isSolved()
// record runs of 2, 3, and 4 and check each for 3 cases:
//  - no ends (not blocked)		  = great move
//  - one end (partially blocked) = good move
//  - two ends (blocked)		  = bad move

void Player1::TALLY_heur_vars(int &line, int &lineValue, 
							  char &playerColor)
{
	if (line == 2)
	{
		if (playerColor == 'P')		
			two += lineValue; // better for a
		else
			two -= lineValue; // better for b
	}
	else if (line == 3)
	{
		if (playerColor == 'P')
			three += lineValue; // better for a
		else
			three -= lineValue; // better for b
	}
	else if (line == 4) // something drastic
	{
		if (playerColor == 'P')
			four += lineValue; // better for a
		else
			four -= lineValue; // better for b
	}
}


//--------------------------------------------------------------


bool Player1::isSolved(char &whoWon) // 5 in a row?
{
	int l, m, line, row;	bool victory = false;
	int lineVal;			// how much a run / line is worth
	int helpfull;			// for diagonal board evaluation
	char evaluate = 'X';	// alternate between 'P' and 'T'
	char direction = '@';	// dir of run / line of 2, 3, 4 ...

	two = three = four = 0;	// heur vars

// HORIZONTAL check for ROWS for win ---------------------
	
	direction = 'H';

	for (l = 0; l < 19; l++)
	{
		line = 0;	// reset every new row

		for (m = 0; m < 19; m++)
		{
			if (pOneBoard[l][m] == 'E') 
				line = 0; // holes reset line

			if (pOneBoard[l][m] == 'P')
			{
				if (evaluate == 'P') 
					line++;	// line already started
				else				
				{
					evaluate = 'P';	 // start new line
					line = 1;		
				}
			}
			else if (pOneBoard[l][m] == 'T')
			{
				if (evaluate == 'T') 
					line++; // line already started
				else				{
					evaluate = 'T';	 // start new line
					line = 1;		}
			}
//--------- TALLY heuristic variables --------------------

			if (2 <= line && line <= 4)
			{
				// check ends : OPPO or OPPT or TPPO or TPPT
				//checkEnds(direction, line, row, col);
				lineVal = checkEnds(direction, line, l, m);
				TALLY_heur_vars(line, lineVal, evaluate);
				//				3	   
			}
			else if (line == 5)			
			{
				victory = true;	
				whoWon = evaluate;
				return victory;		
			}

//--------- END TALLY -------------------------------------

		}
	}

// VERTICAL check for COLUMNS for wins -----------------------

	direction = 'V';
	
	for (l = 0; l < 19; l++)
	{
		line = 0;	// reset every new column

		for (m = 0; m < 19; m++)
		{
			if (pOneBoard[m][l] == 'E')	 line = 0; // holes reset line

			if (pOneBoard[m][l] == 'P')
			{
				if (evaluate == 'P') // line already started
					line++;
				else				{
					evaluate = 'P';	 // start new line
					line = 1;		}	
			}
			else if (pOneBoard[m][l] == 'T')
			{
				if (evaluate == 'T') // line already started
					line++;
				else				{
					evaluate = 'T';	 // start new line
					line = 1;		}
			}

//--------- TALLY heuristic variables --------------------

			if (2 <= line && line <= 4)
			{
				//checkEnds(direction, line, row, col);
				lineVal = checkEnds(direction, line, m, l);
				TALLY_heur_vars(line, lineVal, evaluate);
			}
			else if (line == 5)			
			{
				victory = true;	line = 0;	
				whoWon = evaluate;
				return victory;		
			}

//--------- END TALLY -------------------------------------
		}
	}

// 1 DIAGONAL BL, board cut into 2 triangles w/ 45 angle at BL

	direction = '1';

	for (l = 14; l >= 0; l--) // diminishing rows
	{
		line = 0;	row = l;

		for (m = 0; m < 19 - l; m++) // 5 diagonals at row 14
		{
			if (pOneBoard[row][m] == 'E')	 // holes reset line
				line = 0;
			else if (pOneBoard[row][m] == 'P')
			{
				if (evaluate == 'P') // line already started
					line++;
				else 
				{
					evaluate = 'P';	 // start new line
					line = 1;
				}
			}
			else if (pOneBoard[row][m] == 'T')
			{
				if (evaluate == 'T') // line already started
					line++;
				else {
					evaluate = 'T';	 // start new line
					line = 1;}
			}

//--------- TALLY heuristic variables --------------------
			
			if (2 <= line && line <= 4)
			{
				//checkEnds(direction, line, row, col);
				lineVal = checkEnds(direction, line, row, m);
				TALLY_heur_vars(line, lineVal, evaluate);
			}
			if (line == 5)
			{
				victory = true;
				whoWon = evaluate;
				return victory;
			}

//--------- END TALLY -------------------------------------

			row++; // ***************************
		}
	}

// 2 DIAGONAL TR, board cut into 2 triangles w/ 45 angle at TR

	direction = '2';	helpfull = 14;
	
	for (l = 4; l < 18; l++) // not l > 19 - fixed overlap w/ BL
	{
		line = 0;	helpfull--;	row = l;

		for (m = 18; m > helpfull; m--) // 5 diagonals at row 14
		{
			if (pOneBoard[row][m] == 'E')	 // holes reset line
				line = 0;

			if (pOneBoard[row][m] == 'P')
			{
				if (evaluate == 'P') // line already started
					line++;
				else {
					evaluate = 'P';	 // start new line
					line = 1;}
			}
			else if (pOneBoard[row][m] == 'T')
			{
				if (evaluate == 'T') // line already started
					line++;
				else {
					evaluate = 'T';	 // start new line
					line = 1;}
			}

//--------- TALLY heuristic variables --------------------
			
			if (2 <= line && line <= 4)
			{
				//checkEnds(direction, line, row, col);
				lineVal = checkEnds(direction, line, row, m);
				TALLY_heur_vars(line, lineVal, evaluate);
			}
			if (line == 5)
			{
				victory = true;
				whoWon = evaluate;
				return victory;
			}

//--------- END TALLY -------------------------------------

			row--; // ***************************
		}
	}

// 3 DIAGONAL BR, board cut into 2 triangles where 45 angle is at BR
	
	direction = '3';	helpfull = 14;
	
	for (l = 14; l > 0; l--) // not l >= 0 - fixed overlap w/ TL
	{
		line = 0;	helpfull--;	row = l;

		for (m = 18; m > helpfull; m--) // 5 diagonals at row 14
		{
			if (pOneBoard[row][m] == 'E')	 // holes reset line
				line = 0;

			if (pOneBoard[row][m] == 'P')
			{
				if (evaluate == 'P') // line already started
					line++;
				else {
					evaluate = 'P';	 // start new line
					line = 1;}
			}
			else if (pOneBoard[row][m] == 'T')
			{
				if (evaluate == 'T') // line already started
					line++;
				else {
					evaluate = 'T';	 // start new line
					line = 1;}
			}

//--------- TALLY heuristic variables --------------------
			
			if (2 <= line && line <= 4)
			{
				//checkEnds(direction, line, row, col);
				lineVal = checkEnds(direction, line, row, m);
				TALLY_heur_vars(line, lineVal, evaluate);
			}
			if (line == 5)
			{
				victory = true;
				whoWon = evaluate;
				return victory;
			}

//--------- END TALLY -------------------------------------

			row++;	// diagonal goes down and to the left
		}}

// 4 DIAGONAL TL, board cut into 2 triangles w/ 45 angle at TL
	
	direction = '4';	helpfull = 4;
	
	for (l = 4; l < 19; l++)
	{
		line = 0;	helpfull++;		row = l;

		for (m = 0; m < helpfull; m++) // 5 diagonals at row 14
		{
			if (pOneBoard[row][m] == 'E')	 // holes reset line
				line = 0;

			if (pOneBoard[row][m] == 'P')
			{
				if (evaluate == 'P') // line already started
					line++;
				else {
					evaluate = 'P';	 // start new line
					line = 1;}
			}
			else if (pOneBoard[row][m] == 'T')
			{
				if (evaluate == 'T') // line already started
					line++;
				else {
					evaluate = 'T';	 // start new line
					line = 1;}
			}

//--------- TALLY heuristic variables --------------------
			
			if (2 <= line && line <= 4)
			{
				//checkEnds(direction, line, row, col);
				lineVal = checkEnds(direction, line, row, m);
				TALLY_heur_vars(line, lineVal, evaluate);
			}
			if (line == 5)
			{
				victory = true;
				whoWon = evaluate;
				return victory;
			}

//--------- END TALLY -------------------------------------

			row--;	// ***************************
		}
	}

	return victory; // false, but heur vals come back
}

//--------------------------------------------------------------
//--------------------------------------------------------------


// restrict possible game moves to be within n blocks of a move 
// already made by either player

//col 1  2  3		radius		searchspace
//	|  |  |	 |		1			1+(origin)+1 = 3 by 3
//	|   p,q  |		3			3+1+3		 = 7 by 7
//	|  |  |	 |		5			5+1+5		 = 11 by 11
//	  -  -  - 

bool Player1::playerDetected(int row, int col)
{
	int radius = 1; // origin of search square at (p,q)

	for (int p = row - radius; p <= row + radius; p++){
		for (int q = col - radius; q <= col + radius; q++)
		{
			if (0 <= p && p < 19) {		// within matrix
				if (0 <= q && q < 19)	// within matrix
				{
					if (pOneBoard[p][q] == 'T' || 
						pOneBoard[p][q] == 'P')
						return true;
				}}}}

	return false;
}

//--------------------------------------------------------------
//				   START
//		<------------a------------|
//	-infin ---- optimal move ---- infin
//      |------------b------------>

//				MOVES TOWARDS
//		<-----a--------|
//	-infin --------move-------------infin
//				  |------b------------>

//			 depth 0	1	 2	  3	   4
// time complexity 8 x 16 x 24 x 32 x 40 = 
// 128 x 16 = 3072 x 24 x 32 = 98,304

// -400 ---------------0---------------- 400
//       worse for a	  better for a
//       better for b	  worse for b


int Player1::getHeuristic (int depth, char winner, bool MAXplayer)
{
	int heu;	int totalOffset;
	
	//values for two + three + four calculated in totalEnds()
	totalOffset = two + three + four;

	if (winner == 'P') 
	{
		heu = 4000 - depth * 2;  // shallower the better
	}
	else if (winner == 'T')
	{
		heu = -4000 + depth * 2;
	}
	else 
	{
		heu = totalOffset; // intermediate board
	}

 	return heu;

}

//--------------------------------------------------------------

// depth - a cap on how low the search reaches in the tree

// function alphabeta(node, depth, α, β, Player)      
int Player1::alphaBeta (int depth, int alpha, int beta, bool MAXplayer)
{
	int heuristic = 0;	bool go = false;
	int temp;			char winner = 'N'; // 'N' for NULL
	
	go = isSolved(winner); // get heur info 

    if (depth == 2 || go ) // or node is a terminal node
	{
		heuristic = getHeuristic(depth, winner, !(MAXplayer));
        return heuristic;// value of the terminal game tree node
	}
	
    if (MAXplayer)	//for each child of node
	{ 
		for (int p = 0; p < 19; p++){
			for (int q = 0; q < 19; q++)
			{
				if (pOneBoard[p][q] == 'E' && playerDetected(p,q))
				{
					pOneBoard[p][q] = 'P';	// make move on board
					temp = alphaBeta(depth+1, alpha, beta, !(MAXplayer)); 

					// total recursive calls
					boardsExamined++; 

// -- maxmax start-
					//alpha = max
					if (temp > alpha) alpha = temp;
					pOneBoard[p][q] = 'E'; //unmake move on board
// -- maxmax end---

// -- ab code start-
/*					if (temp > alpha) //alpha = max(alpha, alphaBeta	
						alpha = temp;

					if (beta <= alpha) { // (* Beta cut-off *)
						pOneBoard[p][q] = 'E';break;	} 
					else  //unmake move on board
						pOneBoard[p][q] = 'E';	*/
// -- ab cod end ---	
				
				}}}

		return alpha; // α
	}
    else	//MINplayer	//for each child of node
	{    
		for (int p = 0; p < 19; p++){
			for (int q = 0; q < 19; q++)
			{
				if (pOneBoard[p][q] == 'E' && playerDetected(p,q))
				{
					pOneBoard[p][q] = 'T'; // make move on board
					temp = alphaBeta(depth+1, alpha, beta, !(MAXplayer)); 

					// total recursive calls
					boardsExamined++;

// -- maxmax start-
					//beta = max
					if (temp > beta) beta = temp;
					pOneBoard[p][q] = 'E'; //unmake move on board
// -- maxmax end---

// -- ab code start- 
/*					if (temp < beta) // β := min(β, alphabeta(
						beta = temp;
          
					if (beta <= alpha) { // (* Alpha cut-off *)
						pOneBoard[p][q] = 'E'; break; }
					else // unmake move on board and continue
						pOneBoard[p][q] = 'E';		*/
// -- ab cod end --- 	

				}}}

		return beta;	// β 
	}
}

//--------------------------------------------------------------


//http://en.wikipedia.org/wiki/Alpha-beta_pruning

void Player1::OpponentDidMove(int row, int col) 
{
	pOneBoard[row][col] = 'T';	// record opponents move
	boardSize++;
}


//--------------------------------------------------------------


// f() assigns the desired move to variables roW and coL and
// returns x and y vals ranging 0-18 representing coordinates
// for gameBoard[][]


void Player1::GetMove(int &roW, int &coL, int secondsRemaining)
{	
	int terminal;	// alphabeta return value. terminal node
	int depth;		// determines search height in tree
	int alpha = -5000;	int beta = 5000;
	bool maxPlayer = true;
	int possibeMoves[365][3];	int moveCount = 0;

	recordMove bestMove;
	bestMove.row = bestMove.col = bestMove.heur = -1;
	boardsExamined = 0;	// reset total between moves

	if (boardSize == 0) // first game move
	{
		roW = coL = 9;	// make first move in center of board
	}
	else // for every 'E' on board
	{
		for (int e = 0; e < 19; e++)		{
			for (int f = 0; f < 19; f++)		
			{
				bool detected = playerDetected(e,f);

				if (pOneBoard[e][f] == 'E' && detected)
				{
					pOneBoard[e][f] = 'P';

					depth = 0;
					terminal = alphaBeta(depth+1, alpha, beta, (maxPlayer)); 				
					
					pOneBoard[e][f] = 'E';

					// total recursive calls
					boardsExamined++; 

					// search for best move
					if (terminal > bestMove.heur || bestMove.col == -1)
					{
						bestMove.row = e;
						bestMove.col = f;
						bestMove.heur = terminal;
					}

					// update best move
					possibeMoves[moveCount][0] = terminal;
					possibeMoves[moveCount][1] = e;
					possibeMoves[moveCount][2] = f;
					moveCount++;
				
				}	// end if
			}	// end for
		}	// end for

		roW = bestMove.row;		coL = bestMove.col;

	} // end else
	
	pOneBoard[roW][coL] = 'P'; // record your move
	boardSize++;
}

