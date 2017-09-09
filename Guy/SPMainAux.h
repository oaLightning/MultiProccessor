#ifndef SPMAINAUX_H_
#define SPMAINAUX_H_

#include "SPMiniMax.h"
#include "SPCHESSParser.h"
#include "SPXML.h"
//Auxiliary functions and constants used by the main function
#define SP_White 1
#define SP_Black 0

//handle commands

/**
 * Given a game state and a command with parameters, the function
 * validates that the command is correct, and adds a disc according
 * to the input.
 * the function also calculates the best move for the computer using minimax algorithm
 * and perform this move and prints the current board
 *
 * @param com - The current user command
 * @param game - The current game state
 * @param diffLevel - this game difficult level entered by the user
 * @return
 * true if game is over
 * the function will set @param memoryFailed if a memory allocation error was thrown
 */
bool HandleMove(int move[4], SPCHESSGame* game, int diffLevel, bool *memoryFailed);

/**
 * Given a game state, this function validates that undo of the last two moves is
 * possible, and perform it with the correct prints to the console
 *
 * @param game - The current game state
 */
void HandleUndo(SPCHESSGame* game);

/**
 * Given a game state and a command with parameters, the function
 * save the game to a file according to format specified by assignment

 *
 * @param com - The current user command
 * @param game - The current game state
 * @return
 * true if succeeded
 * the function will set @param memoryFailed if a memory allocation error was thrown
 */
bool HandleSave(SPCommand* com, SPCHESSGame* game, int mode, int userColor);

/**
 * Given a game state and a command with parameters, the function
 * loads the game from a file according to format specified by assignment

 *
 * @param com - The current user command
 * @param game - The current game state
 * @return
 * true if succeeded
 * the function will set @param memoryFailed if a memory allocation error was thrown
 */
bool HandleLoad(SPCommand* com, SPCHESSGame* game, bool *memoryFailed, int* userColor);



//additional functions


/**
 * Given a game state, this function checks if one of the players has
 * four in a row -and prints the correct message to the console with
 * the winning board
 *
 * @param game - The current game state
 * @return
 * true if game is over
 */
bool CheckWinner(SPCHESSGame* game);

/**
 * Given a game state, this function prints the board and appropriate message
 *
 * @param game - The current game state
 */
void PrintNextTurn(SPCHESSGame* game);


void HandleInvalidCommand();

SPCHESSGame* CreateGame(bool gameInitialized, int diffLevel, SPCHESSGame* game, bool *memoryFailed, int mode );

void GameLoop(int diffLevel, int mode, int userColor, SPCHESSGame *game, char *buffer, bool *gameOver,bool *memoryFailed, SPCommand *com);
void settingsLoop(SPCHESSGame *game, char *buffer, int *diffLevel, int *mode, int *userColor, bool *memoryFailed,
				  SPCommand *com);
bool PlayAITurn(SPCHESSGame* game, int diffLevel, bool *memoryFailed);
#endif
