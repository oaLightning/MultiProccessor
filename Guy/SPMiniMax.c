#include "SPMiniMax.h"

void spMinimaxSuggestMove(SPCHESSGame* currentGame, unsigned int maxDepth,int resultMove[4]){
    if(!currentGame || maxDepth<=0){
        return;
    }
    //copy the game to a new game so history won't get deleted
    SPCHESSGame* newGame = spCHESSGameCopy(currentGame, Max_MINIMAX_HISTORY_SIZE);
    if(!newGame){
        return; //there was a memory allocation problem
    }
    ;
    MiniMax(newGame, 0, maxDepth,resultMove);// call a recursive function that returns the best move
    spCHESSGameDestroy(newGame);// free memory used by this function
}


//todo test scoring and all minimax shit
int MiniMax(SPCHESSGame* currentGame, unsigned int depth, unsigned int maxDepth, int resultMove[4]) {
	//check if we are in max depth - and return the leaf score if yes
	int scoreFactor = -1;
	if (depth % 2 == 1) {
		scoreFactor = 1;
	}
	if (depth == maxDepth) {
		return scoreFactor * BoardScoringFunction(currentGame);
	}
	//check if there is a winner, and return infinte value with sign according to winner
	int currentBoardScore = BoardScoringFunction(currentGame);
	if (currentBoardScore == INT_MAX || currentBoardScore == -INT_MAX) {
		return (scoreFactor * currentBoardScore);
	}
	SPCHESSMove *possibleMoves;
	SPCHESSMove *possibleMovesStart;
	//go over all possible current moves - check for each col if move is possible and call recursively
	int min = INT_MAX;
	int minMove[4] = {-1,-1,-1,-1};
	bool isTerminalBoard = true;
	for (int i = 0; i < SP_CHESS_BOARD_SIZE; i++) {// go over all board
		for (int j = 0; j < SP_CHESS_BOARD_SIZE; j++) {
			if (isCurrentPlayerPiece(currentGame, i, j)) {// if current player piece - get all moves
				possibleMoves = spCHESSGameGetMoves(currentGame->gameBoard, i, j);
				possibleMovesStart = possibleMoves;
				while (possibleMoves != NULL) { // go over all moves and try them
					if (spChessGameIsLegalMove(currentGame, possibleMoves->move) == SP_CHESS_GAME_SUCCESS) {
						isTerminalBoard = false; //If we got here, it means we have at least one available legal move

						spCHESSGameSetMove(currentGame, possibleMoves->move);

						// the minus is handling the difference between max and min nodes
						int currentScore = -MiniMax(currentGame, depth + 1, maxDepth, resultMove);

						if (currentScore < min || minMove[0] == -1) {
							min = currentScore;
							//save move
							memcpy(minMove,possibleMoves->move, 4*sizeof(int));
						}

						//undo the move
						spCHESSGameUndoPrevMove(currentGame);
					}
					possibleMoves = possibleMoves->next;
				}
				FreeAllMoves(possibleMovesStart);
			}
		}
	}
	//if there is no legal move - compute the current state and return.
	if(isTerminalBoard){
		return scoreFactor * (-INT_MAX);//todo how to deal with tie? lost as well? also - if current player got terminal board, so last player wins - check
	}
	//if we are in the recursion base level - return the move needed and not the score
	if(depth==0){
		memcpy(resultMove,minMove, 4*sizeof(int));
	}

	return min;
}
