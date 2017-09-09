#ifndef SPMINIMAX_H_
#define SPMINIMAX_H_

#include "SPCHESSGame.h"

#define Max_MINIMAX_HISTORY_SIZE 872//todo 218*4 depth limit?


/**
 * Given a game state, this function evaluates the best move according to
 * the current player. it has a recursive call that checks each move and according
 * to the minimx algorithm specified in the class, will decide on the best move
 *
 * @param currentGame - The current game state
 * @param depth - current depth in the minimax tree
 * @param maxDepth - The maximum depth of the miniMax algorithm
 * @return
 * a 4-int array:
 * representing the best move, if depth is 0 (= this is the first call)
 * the score of the leaf of current branch chosen by the algorithm, otherwise
 */
int MiniMax(SPCHESSGame* currentGame, unsigned int depth, unsigned int maxDepth, int resultMove[4]);

/**
 * Given a game state, this function evaluates the best move according to
 * the current player. The function initiates a MiniMax algorithm up to a
 * specified length given by maxDepth. The current game state doesn't change
 * by this function including the history of previous moves.
 *
 * @param currentGame - The current game state
 * @param maxDepth - The maximum depth of the miniMax algorithm
 * @return
 * -1 if either currentGame is NULL or maxDepth <= 0.
 * On success the function returns a number between [0,SP_CHESS_GAME_N_COLUMNS -1]
 * which is the best move for the current player.
 */
void spMinimaxSuggestMove(SPCHESSGame* currentGame,
		unsigned int maxDepth,int resultMove[4]);



#endif
