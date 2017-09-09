#include "SPMainAux.h"
//returns true if game over?

void settingsLoop(SPCHESSGame *game, char *buffer, int *diffLevel, int *mode, int *userColor, bool *memoryFailed,
				  SPCommand *com) {
	printf("Specify game setting or type 'start' to begin a game with the current setting:\n");
	while ((*com).cmd != SP_QUIT && (*com).cmd != SP_START) {
		switch ((*com).cmd) { //handle command type
			case SP_GAME_MODE  :
				if ((*com).arg != 1 && (*com).arg != 2) {
					printf("Wrong game mode\n");
				} else {
					if ((*com).arg == 1)
						printf("Game mode is set to 1 player\n");
					if ((*com).arg == 2)
						printf("Game mode is set to 2 players\n");
					(*mode) = (*com).arg;
				}
				break;
			case SP_DIFFICULTY  :
				if ((*mode) == 1) { //one player
					if ((*com).arg > 0 && (*com).arg < 6) {
						(*diffLevel) = (*com).arg;
					} else {
						printf("Wrong difficulty level. The value should be between 1 to 5\n");
					}
				} else {
					HandleInvalidCommand();
				}
				break;
			case SP_USER_COLOR  :
				if ((*mode) == 1) {//two playes
					if ((*com).arg != 0 && (*com).arg != 1) {
						printf("Wrong user color\n");//todo what should i print? - no data in pdf, look in forum
					}
					//0 black, 1 white
					(*userColor) = (*com).arg;
				} else{
					HandleInvalidCommand();
				}


				break;
			case SP_LOAD  :
				HandleLoad(com, game, memoryFailed, userColor);
				break;
			case SP_PRINT_SETTING :
				if ((*mode) == 2) {
					printf("SETTINGS:\nGAME_MODE: 2\n");
				} else {
					printf("SETTINGS:\nGAME_MODE: 1\nDIFFICULTY_LVL: %d\n", (*diffLevel));
					if ((*userColor) == SP_White) {
						printf("USER_CLR: WHITE\n");
					}
					if ((*userColor) == SP_Black) {
						printf("USER_CLR: BLACK\n");
					}
				}
				break;
			case SP_DEFAULT:
				(*diffLevel) = 2; // variable to save the current difficult level enterd from the user
				(*mode) = 1;
				(*userColor) = 1;
				break;//todo
			case SP_START  :
			case SP_Init:
				break;
			case SP_INVALID_LINE:
			default :
				//handle all others (and all non settings commands
				HandleInvalidCommand();
				break;
		}
		if ((*memoryFailed)) // if any of the functions changed the memoryFailed flag - there was a problem and we need to exit before reading from user.
			break;
		(*com) = readCommand(buffer, memoryFailed);
		if ((*memoryFailed))//just in case that read command failed to allocate memory
			break;
	}
}

void GameLoop(int diffLevel, int mode, int userColor, SPCHESSGame *game, char *buffer, bool *gameOver,
			  bool *memoryFailed, SPCommand *com) {
	//todo should print the starting board before playing?
	if (userColor == (SP_CHESS_GAME_PLAYER_2_SYMBOL - 1)) {//if user plays second, computer should start
		if (!PlayAITurn(game, diffLevel, memoryFailed)) {
			return;
		}
	}

	PrintNextTurn(game);// starting the game
	(*com) = readCommand(buffer, memoryFailed);
	if ((*memoryFailed))//just in case that read command failed to allocate memory
		return;


	while ((*com).cmd != SP_QUIT && (*com).cmd != SP_RESET) {
		switch ((*com).cmd) { //handle command type
			case SP_UNDO  :
				if (mode == 1) {
					HandleUndo(game);
				} else {
					printf("Undo command not avaialbe in 2 players mode\n");
				}
				break;
			case SP_MOVE  :
				(*gameOver) = HandleMove(com->move, game, diffLevel, memoryFailed);
				break;
			case SP_GET_MOVES  :
				//todo (*memoryFailed) = HandleSuggestMove(game, diffLevel);
				break;
			case SP_SAVE:
				(*memoryFailed) = HandleSave(com, game, mode, userColor);
				break;
			case SP_INVALID_LINE:
			default :
				//handle all others (and all settings commands
				HandleInvalidCommand();
				break;
		}
		if ((*memoryFailed) || (*gameOver)) {
			// if any of the functions changed the memoryFailed flag - there was a problem and we need to exit before reading from user.
			break;
		}
		(*com) = readCommand(buffer, memoryFailed);
		if ((*memoryFailed))//just in case that read command failed to allocate memory
			break;
	}
}



bool HandleMove(int move[4], SPCHESSGame* game, int diffLevel, bool *memoryFailed) {
	//todo validate command args are in borders? are int? in parser?
	SP_CHESS_GAME_MESSAGE message = spChessGameIsLegalMove(game, move);
	switch (message) {
		case SP_CHESS_GAME_INVALID_POSITION:
			printf("Invalid position on the board\n");
			return false;
		case SP_CHESS_GAME_VOID_POSITION:
			printf("The specified position does not contain your piece\n");
			return false;
		case SP_CHESS_GAME_ILLEGAL_MOVE:
			printf("Illegal move\n");
			return false;
		default:
			break;
	}


	if(spCHESSGameSetMove(game, move) != SP_CHESS_GAME_SUCCESS){
		*memoryFailed = true;
		return true;//game is over, memory failed

	}

	//handle pawn promotion
	if ((move[2] == 0 || move[2] == (SP_CHESS_BOARD_SIZE - 1)) &&
		tolower(game->gameBoard[move[2]][move[3]]) == (char) pawn) {
		char buffer[SP_MAX_LINE_LENGTH];
		pieceType newPiece = none;
		while (newPiece == none) {
			printf("Pawn promotion - please replace the pawn by queen, rook, knight or bishop:\n");
			newPiece = readPieceType(buffer, memoryFailed);
			if (memoryFailed)//just in case that read command failed to allocate memory
				break;
		}
		//todo handle memory failed, handle black/white - add get current player piece, should handle king threatened as well
		game->gameBoard[move[2]][move[3]] = (char) newPiece;
	}

	if (CheckWinner(game)) {
		return true;
	}

	//handle check
	if (game->isKingThreatened[0] || game->isKingThreatened[1]) {
		printf("Check!\n");
	}

	if(!PlayAITurn(game, diffLevel,memoryFailed)){
		return false;
	}
	PrintNextTurn(game);

	return false;
}

//false if failed
bool PlayAITurn(SPCHESSGame* game, int diffLevel, bool *memoryFailed){
	//todo handle pawn promotion for AI?
	if (game->isOnePlayerGame) {
		int comp[4]={-1,-1,-1,-1};
		spMinimaxSuggestMove(game, (unsigned int) diffLevel, comp);// get the computer move using minimax algorithm
		if (comp[0] == -1) {
			*memoryFailed = true;//handle memory allocation problems
			return false;
		}
		spCHESSGameSetMove(game, comp);// play the computer move


		pieceType piece = (pieceType)(tolower(game->gameBoard[comp[2]][comp[3]]));
		printf("Computer: move ");
		switch (piece){
			case pawn:
				printf("pawn");
				break;
			case bishop:
				printf("bishop");
				break;
			case knight:
				printf("knight");
				break;
			case rook:
				printf("rook");
				break;
			case queen:
				printf("queen");
				break;
			case king:
				printf("king");
				break;
			default:
				break;
		}
		printf(" at <%d,%c> to <%d,%c>\n",comp[0]+1,(char)('A'+comp[1]),comp[2]+1,(char)('A'+comp[3]));

		if (CheckWinner(game)) {
			return true;
		}
		//handle check
		if (game->isKingThreatened[0] || game->isKingThreatened[1]) {
			printf("Check!\n");
		}
	}
	return true;
}

void HandleUndo(SPCHESSGame* game){

	if(!CanUndoTwoMoves(game)){
		//validate that we have enough moves in the history list
		printf("Empty history, move cannot be undone\n");
		return;
	}

	int move[5];
	GetLastMove(game, move); //Column picked by last player (used to print) (+1 because the printing is 1-based)
	if(spCHESSGameUndoPrevMove(game) == SP_CHESS_GAME_SUCCESS){
		if (game->currentPlayer == SP_CHESS_GAME_PLAYER_1_SYMBOL) {
			printf("Undo move for player black : <%d,%c> -> <%d,%c>\n",move[2]+1,(char)('A'+move[3]),move[0]+1,(char)('A'+move[1]));
		}
		if (game->currentPlayer == SP_CHESS_GAME_PLAYER_2_SYMBOL) {
			printf("Undo move for player white : <%d,%c> -> <%d,%c>\n",move[2]+1,(char)('A'+move[3]),move[0]+1,(char)('A'+move[1]));
		}

	}
	else{
		printf("Error: cannot undo previous move!\n");
	}

	GetLastMove(game, move);//Column picked by last player (used to print) (+1 because the printing is 1-based)
	if(spCHESSGameUndoPrevMove(game) == SP_CHESS_GAME_SUCCESS){
		if (game->currentPlayer == SP_CHESS_GAME_PLAYER_1_SYMBOL) {
			printf("Undo move for player black : <%d,%c> -> <%d,%c>\n",move[2]+1,(char)('A'+move[3]),move[0]+1,(char)('A'+move[1]));
		}
		if (game->currentPlayer == SP_CHESS_GAME_PLAYER_2_SYMBOL) {
			printf("Undo move for player white : <%d,%c> -> <%d,%c>\n",move[2]+1,(char)('A'+move[3]),move[0]+1,(char)('A'+move[1]));
		}
	}
	else{
		printf("Error: cannot undo previous move!\n");
	}

	PrintNextTurn(game);
}

bool HandleSave(SPCommand* com, SPCHESSGame* game, int mode, int userColor){
	//todo handle memory, validate inputs
	FILE *fb = fopen(com->path, "w");
	if (fb == NULL){
		printf("File cannot be created or modified\n");
		return true;
	}

	create_xml(fb,game,mode, userColor);

	if (fclose(fb)){
		return true;
	}

	return false;
}

bool HandleLoad(SPCommand* com, SPCHESSGame* game, bool *memoryFailed, int* userColor){
	//todo validate inputs
	//read diff
	FILE *fb = fopen(com->path, "r");
	if (fb == NULL){
		printf("Error: File doesn’t exist or cannot be opened\n");
		return true;
	}

	*userColor = read_xml(fb,game);
	if(game == NULL)
		*memoryFailed = true;

	if (fclose(fb) == 0){//success
		return true;
	}

	return false;
}




bool CheckWinner(SPCHESSGame* game){
	if (!spChessGameIsLegalMoveExist(game)) {
		spCHESSGamePrintBoard(game);
		//should check if there is any legal move now for new current player(has changed in set move)
		if (game->isKingThreatened[game->currentPlayer - 1]) { //if no move for current player and king is threaten
			if (game->currentPlayer == SP_CHESS_GAME_PLAYER_1_SYMBOL) {
				printf("Mate! White player wins the game\n");
			}
			if (game->currentPlayer == SP_CHESS_GAME_PLAYER_2_SYMBOL) {
				printf("Mate! Black player wins the game\n");
			}

		} else {
			printf("The game is tied\\n");
		}

		return true;
	}
	return false;
}

void PrintNextTurn(SPCHESSGame* game) {
	spCHESSGamePrintBoard(game);
	printf("%s player - enter your move:\n", which_color(game->currentPlayer));//todo:black is upper or lower first char
}

void HandleInvalidCommand(){
	printf("Error: invalid command\n");
}

SPCHESSGame* CreateGame(bool gameInitialized, int diffLevel, SPCHESSGame* game, bool *memoryFailed, int mode ){
	if (gameInitialized) {
		spCHESSGameDestroy(game); //free memory if this is not the first game
	}
	game = spCHESSGameCreate((mode == 1), HISTORY_SIZE); // create a new game
	if (!game) {
		//handle memory allocation problems
		*memoryFailed = true;
		return NULL;
	}
	game->diff = diffLevel;
	return game;
}


