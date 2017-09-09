#include "SPCHESSGame.h"

//object functions
SPCHESSGame* spCHESSGameCreate(bool is1PlayerGame, int historySize){
    int j;

	SPCHESSGame *newGame = (SPCHESSGame *)malloc(sizeof(SPCHESSGame)); //Allocate memory for a new object
	if(!newGame){ //memory allocation failed
		printf("Error: malloc has failed\n");
		return NULL;
	}

	//init game board
	int row, col;
	pieceType initialPieces[] = {rook, knight, bishop, queen, king, bishop, knight, rook}; //All pieces which have special initial location (everyone but pawn)
	//Placing initial non-pawn pieces on board:
	for(col=0; col<SP_CHESS_BOARD_SIZE; col++){
		newGame->gameBoard[0][col] = (char)toupper(initialPieces[col]);
		newGame->gameBoard[SP_CHESS_BOARD_SIZE-1][col] = initialPieces[col];
	}
	//Placing initial pawn pieces on board:
	for(j=0; j<SP_CHESS_BOARD_SIZE; j++){
		newGame->gameBoard[1][j] = (char)toupper((char)pawn);
		newGame->gameBoard[SP_CHESS_BOARD_SIZE-2][j] = (char)pawn;
	}
	for (row=2; row<SP_CHESS_BOARD_SIZE-2; row++)
	{
		for (col=0; col<SP_CHESS_BOARD_SIZE; col++)
		{
			newGame->gameBoard[row][col] = SP_CHESS_GAME_EMPTY_ENTRY;
		}
	}

	newGame->isOnePlayerGame = is1PlayerGame;
	if(newGame->isOnePlayerGame==true){ //Only save previous turns (=be able to undo) in case of a 1-player game 
		newGame->moveList = spArrayListCreate(historySize); //save a list of up to last 3 moves.
		if(!(newGame->moveList)){//memory allocation failed
			printf("Error: malloc has failed\n");
			return NULL;
		}
	}
	newGame->currentPlayer=SP_CHESS_GAME_PLAYER_2_SYMBOL;
	newGame->kingLocation[SP_CHESS_GAME_PLAYER_1_SYMBOL-1][0] = 0;
	newGame->kingLocation[SP_CHESS_GAME_PLAYER_1_SYMBOL-1][1] = 4;
	newGame->kingLocation[SP_CHESS_GAME_PLAYER_2_SYMBOL-1][0] = SP_CHESS_BOARD_SIZE-1;
	newGame->kingLocation[SP_CHESS_GAME_PLAYER_2_SYMBOL-1][1] = 4;

	//path is initialized when struct initialized, so we will init values
	newGame->isKingThreatened[0]=false;
	newGame->isKingThreatened[1]=false;
	return newGame;
}

SPCHESSGame* spCHESSGameCopy(SPCHESSGame* src, int historySize){
	if(!src){
		return NULL;
	}

	SPCHESSGame* ret = spCHESSGameCreate(src->isOnePlayerGame, historySize);
	if(!ret){//memory allocation failed
        return NULL;
	}

    //copy all members of the sturct
	ret->currentPlayer=src->currentPlayer;
	ret->moveList = spArrayListCopy(src->moveList);
	//for more complex members, we will copy the memory from the old member to the memory of the new member
	memcpy(ret->gameBoard, src->gameBoard, SP_CHESS_BOARD_SIZE*SP_CHESS_BOARD_SIZE*(sizeof(char)));
	memcpy(ret->isKingThreatened, src->isKingThreatened, 2*(sizeof(bool)));
	memcpy(ret->kingLocation, src->kingLocation, 2*2*(sizeof(int)));
    return ret;
}

void spCHESSGameDestroy(SPCHESSGame* src){
	if(src){//validate that input is not null
		spArrayListDestroy(src->moveList);//free members which are structures
		free(src);
	}
}



//game functions//todo verify input in all

SP_CHESS_GAME_MESSAGE spCHESSGameSetMove(SPCHESSGame* src, int move[]){
	if(!src){
		return SP_CHESS_GAME_INVALID_ARGUMENT;
	}
	//we will check legal in calling function
	if(isOtherKingThreatenedByMove(src, move)){
		src->isKingThreatened[getOtherPlayerSymbol(src) -1] = true;
	}
	//todo if king is not threatened any more?

	char sourcePiece = src->gameBoard[move[0]][move[1]];
	if(tolower(sourcePiece) == (char)king) {
		src->kingLocation[src->currentPlayer - 1][0] = move[2];
		src->kingLocation[src->currentPlayer - 1][1] = move[3];
	}

	addMoveToUndoList(src->moveList, move, MovePiece(src, move));

	//change player
	changePlayer(src);

	return SP_CHESS_GAME_SUCCESS;

}

char MovePiece(SPCHESSGame* src, int move[]){
	if(!src){
		return '\0';
	}
	char oldPiece = src->gameBoard[move[2]][move[3]];
	src->gameBoard[move[2]][move[3]] = src->gameBoard[move[0]][move[1]]; //captured piece is replaced
	src->gameBoard[move[0]][move[1]] = SP_CHESS_GAME_EMPTY_ENTRY; //attacker's original position is vacated
	return oldPiece;
}

void UnMovePiece(SPCHESSGame* src, int move[], char oldPiece){
	if(!src){
		return;
	}
	src->gameBoard[move[0]][move[1]] = src->gameBoard[move[2]][move[3]]; //captured piece is replaced
	src->gameBoard[move[2]][move[3]] = oldPiece; //attacker's original position is vacated
}

SP_CHESS_GAME_MESSAGE spCHESSGameUndoPrevMove(SPCHESSGame* src){
	if(!src)// validate input
		return SP_CHESS_GAME_INVALID_ARGUMENT;

	if((src->moveList) && src->moveList->actualSize > 0){
		int move[5];
		GetLastMove(src, move);
		if(spArrayListRemoveLast(src->moveList)==SP_ARRAY_LIST_SUCCESS){ //remove last recorded move from list
			//change the board accordingly - find the top of the column and change it to empty
			UnMovePiece(src, move,(char)move[4]);
			changePlayer(src);

			char sourcePiece = src->gameBoard[move[2]][move[3]];//handle king location // todo validate
			if(tolower(sourcePiece) == (char)king) {
				src->kingLocation[src->currentPlayer - 1][0] = move[0];
				src->kingLocation[src->currentPlayer - 1][1] = move[1];
			}

			return SP_CHESS_GAME_SUCCESS;
		}
		else{
			return SP_CHESS_GAME_NO_HISTORY; // We got here if #(undos we intended to do) > #(valid moves we've made so far in the game)
		}
	}
	return SP_CHESS_GAME_NO_HISTORY; //We got here if undoCount > historySize(=rsc->moveList->maxSize)
}

SP_CHESS_GAME_MESSAGE spCHESSGamePrintBoard(SPCHESSGame* src){
	if(!src){
		return SP_CHESS_GAME_INVALID_ARGUMENT;
	}
	int col, row;
	//print board
	for (row=0; row<SP_CHESS_BOARD_SIZE; row++)
	{
		printf("%d| ", 8-row);
		for (col=0; col<SP_CHESS_BOARD_SIZE; col++)
		{
			printf("%c ", src->gameBoard[row][col]);
		}
		printf("|\n");
	}
	printf("  -----------------\n   ");
	//print columns numbers
	for (col=0; col<SP_CHESS_BOARD_SIZE; col++)
	{
		printf("%c ", 'A'+col);
	}
	printf(" \n");
	return SP_CHESS_GAME_SUCCESS;
}



bool spChessGameIsLegalMoveExist(SPCHESSGame* src) {
	if(!src){
		return false;
	}
	int row, col;
	//we create a new move from new piece location to the king location and check if it's legal - if yes so king is thretend

	//go over all gameboard, if one of the moves (from a piece location to king is legal, return true
	for (row=0; row<SP_CHESS_BOARD_SIZE; row++) {
		for (col = 0; col < SP_CHESS_BOARD_SIZE; col++) {
			if(src->gameBoard[row][col] != SP_CHESS_GAME_EMPTY_ENTRY){
				if(isPlayerPiece(src->gameBoard[row][col], src->currentPlayer)){
					SPCHESSMove* possibleMoves = spCHESSGameGetMoves(src->gameBoard, row, col);
					SPCHESSMove* temp = possibleMoves;
					bool result = false;
					while(temp != NULL){
						if(spChessGameIsLegalMove(src,temp->move) == SP_CHESS_GAME_SUCCESS){
							result = true;
						}
						temp=temp->next;
					}

					FreeAllMoves(possibleMoves);
					if(result)
						return true;
				}
			}
		}
	}
	return false;
}

SP_CHESS_GAME_MESSAGE spChessGameIsLegalMove(SPCHESSGame* src, int move[]) {
	if(!src){
		return SP_CHESS_GAME_INVALID_MOVE;
	}
	//make sure to validate - in game borders (in set move now)
	if (!isInBorders(move[0]) || !isInBorders(move[1]) || !isInBorders(move[2]) || !isInBorders(move[3])) {
		return SP_CHESS_GAME_INVALID_POSITION;
	}

	//make sure moving current user piece
	if (!isCurrentPlayerPiece(src, move[0], move[1])) {
		return SP_CHESS_GAME_VOID_POSITION;
	}

	//move should move the piece(should not stay in place)
	if (move[0] == move[2] && move[1] == move[3]) {
		return SP_CHESS_GAME_ILLEGAL_MOVE;
	}

	//make sure - in dest cant be current player piece
	if (isCurrentPlayerPiece(src, move[2], move[3])) {
		return SP_CHESS_GAME_ILLEGAL_MOVE;
	}

	pieceType piece = (pieceType) tolower(src->gameBoard[move[0]][move[1]]);
	bool isLegal = false;
	switch (piece) {
		case pawn:
			isLegal = spChessGameIsLegalPawnMove(src, move);
			break;
		case bishop:
			isLegal = spChessGameIsLegalBishopMove(src, move);
			break;
		case rook :
			isLegal = spChessGameIsLegalRookMove(src, move);
			break;
		case knight :
			isLegal = spChessGameIsLegalKnightMove(move);
			break;
		case queen :
			isLegal = spChessGameIsLegalQueenMove(src, move);
			break;
		case king :
			isLegal = spChessGameIsLegalKingMove(move);
			break;
		default:
			break;
	}
	if (!isLegal)
		return SP_CHESS_GAME_ILLEGAL_MOVE;

	//make sure to validate - if king is threaten its not legal
	if (isMyKingThreatenedByMove(src, move)) {
		return SP_CHESS_GAME_ILLEGAL_MOVE;
	}
	return SP_CHESS_GAME_SUCCESS;
}


//legal moves functions

bool spChessGameIsLegalPawnMove(SPCHESSGame* src, int move[]) {
	int rowFactor;
	bool isBlack = isupper(src->gameBoard[move[0]][move[1]]);
	bool isInitialMove;
	if (isBlack) {
		rowFactor = 1;
		isInitialMove = move[0] == 1;
	} else {
		rowFactor = -1;
		isInitialMove = move[0] == (SP_CHESS_BOARD_SIZE -2);
	}

	if (!((move[2] - move[0]) == rowFactor//not - (advance one row or two (initial move)_
		||( (move[2] - move[0]) == (rowFactor*2) && isInitialMove ))) {
		return false;
	}

	if (move[3] - move[1] == 0) {
		//pawn does not capture - only row is changed,  verify no pieces there
		if (src->gameBoard[move[2]][move[3]] != SP_CHESS_GAME_EMPTY_ENTRY ) {
			return false;
		}
		return true;

	} else if (move[3] - move[1] == 1) {
		//capture - not current player piece and not empty
		if (!isCurrentPlayerPiece(src, move[2], move[3]) &&
			src->gameBoard[move[2]][move[3]] != SP_CHESS_GAME_EMPTY_ENTRY) {
			return true;
		}
		return false;

	} else {
		//pawn cannot change two cols
		return false;
	}
}

bool spChessGameIsLegalBishopMove(SPCHESSGame* src, int move[]){
	if( abs(move[3]-move[1]) != abs(move[2]-move[0]) )
		return false; //not allowed to move in any non-diagonal way

	//validate no piece in the way
	int row = move[0];
	int col = move[1];
	//lower row- upper in the board
	int rowFactor = (move[2]-move[0]) > 0 ? 1 : -1;
	int colFactor = (move[3]-move[1]) > 0 ? 1 : -1;
	row+=rowFactor;
	col+=colFactor;
	while(row != move[2]){ //check all cells excluding the last one.
		if(src->gameBoard[row][col] != SP_CHESS_GAME_EMPTY_ENTRY){
			return false; //if there is any piece in the way - not legal move
			// As described in specifications: "Bishop [..] may not leap over other pieces."
		}
		row+=rowFactor;
		col+=colFactor;
	}

	return true;
}

bool spChessGameIsLegalRookMove(SPCHESSGame* src,int move[]) {
	if(move[0] != move[2] && move[1] != move[3]){ // if both row and column are changing - not legal
		return false;
	}

	//validate no piece in the way
	int row = move[0];
	int col = move[1];
	//lower row- upper in the board
	int rowFactor = 0;
	int colFactor = 0;
	if(move[2]-move[0] != 0){
		rowFactor = (move[2]-move[0])/abs(move[2]-move[0]);
	}
	if(move[3]-move[1] != 0){
		colFactor = (move[3]-move[1])/abs(move[3]-move[1]);
	}
	row+=rowFactor;
	col+=colFactor;
	while(row != move[2] || col != move[3]){
		if(src->gameBoard[row][col] != SP_CHESS_GAME_EMPTY_ENTRY){
			return false; //if there is any piece in the way - not legal move
			// As described in specifications: "Bishop [..] may not leap over other pieces."
		}
		row+=rowFactor;
		col+=colFactor;
	}

	//else - just one of them is changing (none is handled in main legal func)
	return true;
}

bool spChessGameIsLegalKnightMove(int move[]) {
	int diff1 = abs(move[2]-move[0]);
	int diff2 = abs(move[3]-move[1]);
	if((diff1 == 1 && diff2 ==2) || (diff1 == 2 && diff2 ==1)){
		return true;
	}
	return false;
}

bool spChessGameIsLegalQueenMove(SPCHESSGame* src, int move[]){
	if(spChessGameIsLegalRookMove(src, move) || spChessGameIsLegalBishopMove(src,move)){
		return true;
	}
	return false;
}

bool spChessGameIsLegalKingMove(int move[]){
	int diff1 = abs(move[2]-move[0]);
	int diff2 = abs(move[3]-move[1]);
	if((diff1 <= 1 && diff2 <=1) ){
		return true;
	}
	return false;
}


//king threaten functions
bool isOtherKingThreatenedByMove(SPCHESSGame* src, int move[]){
	if(!src){
		return true;
	}

	//this is the same as set move without undo list - todo?

	char sourcePiece = src->gameBoard[move[0]][move[1]];
	if(tolower(sourcePiece) == (char)king) {
		src->kingLocation[src->currentPlayer - 1][0] = move[2];
		src->kingLocation[src->currentPlayer - 1][1] = move[3];
	}
	char oldPiece = MovePiece(src, move);
	bool result = isKingThreatened(src, getOtherPlayerSymbol(src));
	UnMovePiece(src, move, oldPiece);

	if(tolower(sourcePiece) == (char)king) {
		src->kingLocation[src->currentPlayer - 1][0] = move[0];
		src->kingLocation[src->currentPlayer - 1][1] = move[1];
	}

	return result;
}

bool isMyKingThreatenedByMove(SPCHESSGame* src, int move[]) {
	if(!src){
		return true;
	}
	//this is the same as set move without undo list -
	char sourcePiece = src->gameBoard[move[0]][move[1]];
	if(tolower(sourcePiece) == (char)king) {
		src->kingLocation[src->currentPlayer - 1][0] = move[2];
		src->kingLocation[src->currentPlayer - 1][1] = move[3];
	}

	char oldPiece = MovePiece(src, move);
	bool result = isKingThreatened(src, src->currentPlayer);
	UnMovePiece(src, move, oldPiece);

	if(tolower(sourcePiece) == (char)king) {
		src->kingLocation[src->currentPlayer - 1][0] = move[0];
		src->kingLocation[src->currentPlayer - 1][1] = move[1];
	}

	return result;
}
bool isKingThreatened(SPCHESSGame* src, int kingPlayerSymbol) {
	if(!src){
		return true;
	}
	int row, col;
	//we create a new move from new piece location to the king location and check if it's legal - if yes so king is thretend

	//go over all gameboard, if one of the moves (from a piece location to king is legal, return true
	for (row=0; row<SP_CHESS_BOARD_SIZE; row++) {
		for (col = 0; col < SP_CHESS_BOARD_SIZE; col++) {
			if(src->gameBoard[row][col] != SP_CHESS_GAME_EMPTY_ENTRY){
				if(!isPlayerPiece(src->gameBoard[row][col], kingPlayerSymbol)){
					char pieceChar = src->gameBoard[row][col];

					int newMove[4] = {row,col,src->kingLocation[kingPlayerSymbol-1][0],src->kingLocation[kingPlayerSymbol-1][1]};

					pieceType piece = (pieceType) tolower(pieceChar);
					switch (piece) {
						case pawn:
							if(spChessGameIsLegalPawnMove(src, newMove))
								return true;
							break;
						case bishop:
							if( spChessGameIsLegalBishopMove(src, newMove))
								return true;
							break;
						case rook :
							if( spChessGameIsLegalRookMove(src, newMove))
								return true;
							break;
						case knight :
							if( spChessGameIsLegalKnightMove(newMove))
								return true;
							break;
						case queen :
							if( spChessGameIsLegalQueenMove(src, newMove))
								return true;
							break;
						case king :
							if( spChessGameIsLegalKingMove(newMove))
								return true;
							break;
						default:
							break;
					}
				}
			}
		}
	}
	return false;
}

bool isPlayerPiece(char piece, int playerSymbol){
	if((isupper(piece) && playerSymbol == SP_CHESS_GAME_PLAYER_1_SYMBOL)||
			(islower(piece) && playerSymbol == SP_CHESS_GAME_PLAYER_2_SYMBOL)) {//player 1 is black
		return true;
	}
	return false;
}

int getOtherPlayerSymbol(SPCHESSGame* src){
	if(!src){
		return 0;
	}
	if(src->currentPlayer == SP_CHESS_GAME_PLAYER_1_SYMBOL)
		return SP_CHESS_GAME_PLAYER_2_SYMBOL;
	else
		return SP_CHESS_GAME_PLAYER_1_SYMBOL;
}

//other functions

bool isCurrentPlayerPiece(SPCHESSGame* src, int row, int col){
	if(!src){
		return false;
	}
	//if is empty
	if(src->gameBoard[row][col] == SP_CHESS_GAME_EMPTY_ENTRY){
		return false;
	}

	//check case
	if((isupper(src->gameBoard[row][col]) && src->currentPlayer == SP_CHESS_GAME_PLAYER_1_SYMBOL)
	   || (islower(src->gameBoard[row][col]) && src->currentPlayer == SP_CHESS_GAME_PLAYER_2_SYMBOL)){
		return true;
	}
	else {
		return false;
	}
}

void changePlayer(SPCHESSGame* src){
	if(!src){
		return;
	}
    if(src->currentPlayer == SP_CHESS_GAME_PLAYER_1_SYMBOL){
        src->currentPlayer = SP_CHESS_GAME_PLAYER_2_SYMBOL;
    }
    else{
        src->currentPlayer = SP_CHESS_GAME_PLAYER_1_SYMBOL;
    }
}




int BoardScoringFunction(SPCHESSGame* currentGame){
	if(!currentGame){
		return 0;
	}
	int row, col;
	int score = 0;
    //go over all places(from top)
    for (row=0; row<SP_CHESS_BOARD_SIZE; row++)
    {
        for (col=0; col<SP_CHESS_BOARD_SIZE; col++)
        {
			char currentCell = currentGame->gameBoard[row][col];
			if(currentCell != SP_CHESS_GAME_EMPTY_ENTRY)
			{
				//get minus/plus if current user or not
				int factor;
				if(isCurrentPlayerPiece(currentGame, row, col)){
					factor = 1;
				}
				else{
					factor=-1;
				}
				currentCell = (char) tolower(currentCell);
				int pieceScore = 0;
				switch ((pieceType)currentCell ) {
					case pawn:
						pieceScore=1;
						break;
					case knight :
						pieceScore=4;
						break;
					case bishop:
						pieceScore=4;
						break;
					case rook :
						pieceScore=16;
						break;
					case queen :
						pieceScore=64;
						break;
					case king :
						pieceScore=400;
						break;
					default:
						break;
				}
				score += factor*pieceScore;
			}
        }
    }

	return score;
}

//undo
void addMoveToUndoList(SPArrayList *lst, int *move, char piece){
	if(!lst){
		return;
	}

	if(spArrayListIsFull(lst)==true){
		//if list is full - remove the first move (FIFO)
		if(spArrayListRemoveFirst(lst)!=SP_ARRAY_LIST_SUCCESS){
			printf("Problem with removing first move");
		}
	}

	//save the new move
	if(spArrayListAddLast(lst,move, piece)!=SP_ARRAY_LIST_SUCCESS){
		printf("Problem with adding last move");
	}
}

char GetLastMove(SPCHESSGame *src, int *move){
	if(!src){
		return SP_NULL_CHAR;
	}

	if(src->moveList->actualSize > 0){
		//validate we have the last move, and return the column played
		int* lastMove = spArrayListGetLast(src->moveList);
		memcpy(move, lastMove, MOVE_SIZE* sizeof(int));
		return (char) lastMove[4];
	}
	return SP_NULL_CHAR;
}

bool CanUndoTwoMoves(SPCHESSGame* src){
	if(!src){
		return false;
	}
	if(src->moveList->actualSize >= 2){
		return true;
	}
	return false;
}
