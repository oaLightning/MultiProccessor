#ifndef SPGETMOVES_H_
#define SPGETMOVES_H_
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include<string.h>
#define SP_CHESS_BOARD_SIZE 8


typedef enum sp_chess_game_piece_type {
	pawn = 'm',
	bishop = 'b',
	rook = 'r',
	knight = 'n',
	queen = 'q',
	king = 'k',
	none
}pieceType;


typedef struct sp_CHESS_move_t {
	int move[4];
	struct sp_CHESS_move_t* next;
	struct sp_CHESS_move_t* last;
} SPCHESSMove;

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
SPCHESSMove* spCHESSGameGetMoves(char src[SP_CHESS_BOARD_SIZE][SP_CHESS_BOARD_SIZE], int row, int col);

SPCHESSMove* addMoveToList(SPCHESSMove* moveObj, int x, int y, int m, int n);
SPCHESSMove*  ConcatLists(SPCHESSMove* moveObj, SPCHESSMove* moveObj1);

bool isInBorders(int x);
bool isMoveInBorders(int x[4]);
SPCHESSMove* ValidateMovesInBorders(SPCHESSMove* moveObj);
void FreeAllMoves(SPCHESSMove* moveList);

#endif //SPGETMOVES_H_
