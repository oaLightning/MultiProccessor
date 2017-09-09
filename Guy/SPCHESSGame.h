#ifndef SPCHESSGAME_H_
#define SPCHESSGAME_H_
#include <limits.h>
#include "SPArrayList.h"
#include "SPGetMoves.h"

/**
 * SPChessGame Summary:
 *
 * A container that represents a classic chess game, a two players 8 by 8
 * board game (rows X columns). The container supports the following functions.
 *
 * spChessGameCreate           - Creates a new game board
 * spChessGameCopy             - Copies a game board
 * spChessGameDestroy          - Frees all memory resources associated with a game
 * spChessGameSetMove          - Sets a move on a game board
 * spChessGameIsValidMove      - Checks if a move is valid
 * spChessGameUndoPrevMove     - Undoes previous move made by the last player
 * spChessGamePrintBoard       - Prints the current board
 * spChessGameGetCurrentPlayer - Returns the current player
 *
 **/
 

//Definitions

#define SP_CHESS_GAME_PLAYER_1_SYMBOL 1 //black - upper case
#define SP_CHESS_GAME_PLAYER_2_SYMBOL 2 //white - lowercase
#define SP_CHESS_GAME_EMPTY_ENTRY '_'
#define SP_NULL_CHAR '\0'

#define which_color(x) ((x>1)? ("White"): ("Black"))

typedef struct _sp_CHESS_game_t {
	char gameBoard[SP_CHESS_BOARD_SIZE][SP_CHESS_BOARD_SIZE];
	int currentPlayer;
	SPArrayList* moveList;
	bool isOnePlayerGame;
	//in the following, first entry for black, second for white
	bool isKingThreatened[2];
	int kingLocation[2][2];
	int diff;
} SPCHESSGame;

/**
 * Type used for returning error codes from game functions
 */

typedef enum sp_CHESS_game_message_t {
	SP_CHESS_GAME_INVALID_POSITION = 1, //If returned in 'spCHESSGameSetMove', should result in printing "Invalid position on the board\n"
	SP_CHESS_GAME_VOID_POSITION, //If returned in 'spCHESSGameSetMove', should result in printing "The specified position does not contain your piece\n"
	SP_CHESS_GAME_ILLEGAL_MOVE,

	SP_CHESS_GAME_INVALID_MOVE,
	SP_CHESS_GAME_INVALID_ARGUMENT,
	SP_CHESS_GAME_NO_HISTORY,
	SP_CHESS_GAME_SUCCESS,
} SP_CHESS_GAME_MESSAGE;

/**
 * Creates a new game with a specified history size. The history size is a
 * parameter which specifies the number of previous moves to store. If the number
 * of moves played so far exceeds this parameter, then first moves stored will
 * be discarded in order for new moves to be stored.
 *
 * @param historySize - The total number of moves to undo,
 *        a player can undo at most historySizeMoves turns.
 * @return
 * NULL if either a memory allocation failure occurs or historySize <= 0.
 * Otherwise, a new game instant is returned.
 */
SPCHESSGame* spCHESSGameCreate(bool is1PlayerGame, int historySize);

/**
 *	Creates a copy of a given game.
 *	The new copy has the same status as the src game.
 *
 *	@param src - the source game which will be copied
 *	@return
 *	NULL if either src is NULL or a memory allocation failure occurred.
 *	Otherwise, an new copy of the source game is returned.
 *
 */
SPCHESSGame* spCHESSGameCopy(SPCHESSGame* src, int historySize);

/**
 * Frees all memory allocation associated with a given game. If src==NULL
 * the function does nothing.
 *
 * @param src - the source game
 */
void spCHESSGameDestroy(SPCHESSGame* src);


//game functions


/**
 * Validates current move in given game, by checking current player's identity
 * and matching it with source and target pieces' association.
 * Calls spChessGameIsLegalMove, for further inspection of move,
 * according to source piece type's rules of movement and capturing.
 *
 * @param src - The target game
 * @param move - The 4-int array that contains col. and row indices (0-based) of source and destination
 * The indices are 0-based and in the range [0,SP_CHESS_GAME_N_COLUMNS-1].
 * @return
 * SP_CHESS_GAME_INVALID_ARGUMENT - if src is NULL or col is out-of-range
 * SP_CHESS_GAME_INVALID_MOVE - if the given column is full.
 * SP_CHESS_GAME_SUCCESS - otherwise
 */
/**
 * Replaces two pieces in a given game, by specifying two couples of row and column indices:
 * one couple for source (original position of piece to be moved) and the other for destination
 * (position of piece to be replaced and discarded).
 * The indices are 0-based and in the range [0,SP_CHESS_GAME_N_COLUMNS-1].
 *
 * @param src - The target game
 * @param move - The 4-int array that contains col. and row (0-based) of source and destination
*/

SP_CHESS_GAME_MESSAGE spCHESSGameSetMove(SPCHESSGame* src, int move[]);



/**
 * Checks if a piece can be taken from given source and moved to specified target,
 * by observing the piece type.
 *
 * @param src - The source game
 * @param move - The 4-int array that contains col. and row (0-based) of source and destination
 * @return
 * true  - if the the piece in row move[0], column move[1], can be moved to target (move[2],move[3])
 * false - otherwise.
 */
SP_CHESS_GAME_MESSAGE spChessGameIsLegalMove(SPCHESSGame* src, int move[]);

/**
 * Removes a piece that was put in the previous move and changes the current
 * player's turn. If the user invoked this command more than historySize times
 * in a row then an error occurs.
 *
 * @param src - The source game
 * @return
 * SP_CHESS_GAME_INVALID_ARGUMENT - if src == NULL
 * SP_CHESS_GAME_NO_HISTORY       - if the user invoked this function more then
 *                                 historySize in a row.
 * SP_CHESS_GAME_SUCCESS          - on success. The last disc that was put on the
 *                                 board is removed and the current player is changed
 */
SP_CHESS_GAME_MESSAGE spCHESSGameUndoPrevMove(SPCHESSGame* src);

/**
 * On success, the function prints the board game. If an error occurs, then the
 * function does nothing. The characters 'm', 'b', 'r', 'n', 'q', 'k' are used to represent
 * the pieces of player 1 (white) and the matching uppercase characters sre used to represent player 2's (black) pieces.
 *
 * @param src - the target game
 * @return
 * SP_CHESS_GAME_INVALID_ARGUMENT - if src==NULL
 * SP_CHESS_GAME_SUCCESS - otherwise
 *
 */
SP_CHESS_GAME_MESSAGE spCHESSGamePrintBoard(SPCHESSGame* src);

/**
 * Returns the current player of the specified game.
 * @param src - the source game
 * @return
 * SP_CHESS_GAME_PLAYER_1_SYMBOL - if it's player one's turn
 * SP_CHESS_GAME_PLAYER_2_SYMBOL - if it's player two's turn
 * SP_CHESS_GAME_EMPTY_ENTRY     - otherwise
 */
int spCHESSGameGetCurrentPlayer(SPCHESSGame* src);


//legal functions


/**
 * Checks if a pawn piece can be taken from given source and moved to specified target,
 * according to pawn movement and capturing rules, as described in this project's specifications.
 *
 * @param src - The source game
 * @param move - The 4-int array that contains col. and row (0-based) of source and destination
 * @return
 * true  - if the the piece in row move[0], column move[1], can be moved to target (move[2],move[3])
 * false - otherwise.
 */
bool spChessGameIsLegalPawnMove(SPCHESSGame* src, int move[]);

/**
 * Checks if a bishop piece can be taken from given source and moved to specified target,
 * according to bishop movement and capturing rules, as described in this project's specifications.
 *
 * @param src - The source game
 * @param move - The 4-int array that contains col. and row (0-based) of source and destination
 * @return
 * true  - if the the piece in row move[0], column move[1], can be moved to target (move[2],move[3])
 * false - otherwise.
 */
bool spChessGameIsLegalBishopMove(SPCHESSGame* src, int move[]);

/**
 * Checks if a rook piece can be taken from given source and moved to specified target,
 * according to rook movement and capturing rules, as described in this project's specifications.
 *
 * @param src - The source game
 * @param move - The 4-int array that contains col. and row (0-based) of source and destination
 * @return
 * true  - if the the piece in row move[0], column move[1], can be moved to target (move[2],move[3])
 * false - otherwise.
 */
bool spChessGameIsLegalRookMove(SPCHESSGame* src,int move[]);

/**
 * Checks if a knight piece can be taken from given source and moved to specified target,
 * according to knight movement and capturing rules, as described in this project's specifications.
 *
 * @param src - The source game
 * @param move - The 4-int array that contains col. and row (0-based) of source and destination
 * @return
 * true  - if the the piece in row move[0], column move[1], can be moved to target (move[2],move[3])
 * false - otherwise.
 */
bool spChessGameIsLegalKnightMove(int move[]);

/**
 * Checks if a queen piece can be taken from given source and moved to specified target,
 * according to queen movement and capturing rules, as described in this project's specifications.
 *
 * @param src - The source game
 * @param move - The 4-int array that contains col. and row (0-based) of source and destination
 * @return
 * true  - if the the piece in row move[0], column move[1], can be moved to target (move[2],move[3])
 * false - otherwise.
 */
bool spChessGameIsLegalQueenMove(SPCHESSGame* src,int move[]);

/**
 * Checks if a king piece can be taken from given source and moved to specified target,
 * according to king movement and capturing rules, as described in this project's specifications.
 *
 * @param src - The source game
 * @param move - The 4-int array that contains col. and row (0-based) of source and destination
 * @return
 * true  - if the the piece in row move[0], column move[1], can be moved to target (move[2],move[3])
 * false - otherwise.
 */
bool spChessGameIsLegalKingMove(int move[]);



//other functions


/**
 * Adds 'move', i.e. the given 4-int array (two couples of row and column indices:
 * one couple for source and the other for destination), to the end of the given SPArrayList.
 * Called by spCHESSGameSetMove
 * @param lst - SPArrayList which is the moveList of the game
 * @param col - the column number of the move to be added to the list
 */
void addMoveToUndoList(SPArrayList *lst, int *move, char piece);

/**
* Checks whether given position on game board contains current player's piece.
* Called by spCHESSGameSetMove
* @param src - the source game
* @param row - row from which a piece is to be moved by current player
* @param col - column from which a piece is to be moved by current player
* @return
* SP_CHESS_GAME_SUCCESS, if position on board contains current player's piece
* SP_CHESS_GAME_VOID_POSITION, if position on board does not contain current player's piece
* SP_CHESS_GAME_INVALID_MOVE, in case of error
*/
bool isCurrentPlayerPiece(SPCHESSGame* src, int row, int col);

/**
* Switches current player - from user to computer or vice versa
* @param src - the source game
*/
void changePlayer(SPCHESSGame* src);

/**
* Computes and returns current game's board score, according to the
* function presented in the specifications for this assignment.
* 
* @ param currentGame - current game...
* @ return current game's board score
*/
int BoardScoringFunction(SPCHESSGame* currentGame);

/**
* Checks whether there is a Check (possibly CheckMate) or not.
* @param src - current game
* @return
* true - last move made threatens next player's king
* false - if both kings are safe
*/
bool isMyKingThreatenedByMove(SPCHESSGame* src, int move[]);

/**
* Checks whether there is a Check (possibly CheckMate) or not.
* @param src - current game
* @return
* true - last move made threatens next player's king
* false - if both kings are safe
*/
bool isOtherKingThreatenedByMove(SPCHESSGame* src, int move[]);

/**
* Checks whether it is possible to undo two moves (one of user and one of computer) in current game, and returns result
* @param src - source game
* @return
* true - if there are at least 2 saved moves in src->moveList
* false - otherwise
*/
bool CanUndoTwoMoves(SPCHESSGame* src);

/**
* returns last recorded move in source game, if there is such
* @param src - source game
* @return
* last recorded move (column num.) - if there is at least 1 saved move in src->moveList
* -1 - otherwise
*/
char GetLastMove(SPCHESSGame *src, int *move);


char MovePiece(SPCHESSGame* src, int move[]);
void UnMovePiece(SPCHESSGame* src, int move[], char oldPiece);
bool isKingThreatened(SPCHESSGame* src, int kingPlayerSymbol);
int getOtherPlayerSymbol(SPCHESSGame* src);
	bool isPlayerPiece(char piece, int playerSymbol);
bool spChessGameIsLegalMoveExist(SPCHESSGame* src) ;
#endif
