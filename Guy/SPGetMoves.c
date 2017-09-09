#include "SPGetMoves.h"

//possible moves functions
SPCHESSMove* spChessGameGetPawnMoves(char gameBoard[SP_CHESS_BOARD_SIZE][SP_CHESS_BOARD_SIZE], int location[]) {
	SPCHESSMove *moveObj = NULL;

	int rowFactor;
	bool isBlack = isupper(gameBoard[location[0]][location[1]]);
	bool isInitialMove;
	if (isBlack) {
		rowFactor = 1;
		isInitialMove = location[0] == 1;
	} else {
		rowFactor = -1;
		isInitialMove = location[0] == (SP_CHESS_BOARD_SIZE -2);
	}

	moveObj = addMoveToList(moveObj, location[0],location[1],(location[0]+rowFactor),(location[1]-1));
	moveObj = addMoveToList(moveObj, location[0],location[1],(location[0]+rowFactor),location[1]);
	moveObj = addMoveToList(moveObj, location[0],location[1],(location[0]+rowFactor),(location[1]+1));

	if(isInitialMove)
		moveObj = addMoveToList(moveObj, location[0],location[1],(location[0]+(rowFactor*2)),location[1]);

	return ValidateMovesInBorders(moveObj);
}

SPCHESSMove *spChessGameGetBishopMoves(int location[]) {
	SPCHESSMove *moveObj = NULL;
	for(int i = 0; i< SP_CHESS_BOARD_SIZE; i++)
		moveObj = addMoveToList(moveObj, location[0],location[1],(location[0] - location[1] + i), i);

	for(int i = 0; i< SP_CHESS_BOARD_SIZE; i++)
		moveObj = addMoveToList(moveObj, location[0],location[1],(location[0] - location[1] + i), (SP_CHESS_BOARD_SIZE - 1 -i));

	return ValidateMovesInBorders(moveObj);
}

SPCHESSMove *spChessGameGetRookMoves(int location[]) {
	SPCHESSMove *moveObj = NULL;
	for(int i = 0; i< SP_CHESS_BOARD_SIZE; i++)
		moveObj = addMoveToList(moveObj, location[0],location[1],location[0],i);

	for(int i = 0; i< SP_CHESS_BOARD_SIZE; i++)
		moveObj = addMoveToList(moveObj, location[0],location[1],i,location[1]);

	return ValidateMovesInBorders(moveObj);
}

SPCHESSMove *spChessGameGetKnightMoves(int location[]) {
	SPCHESSMove *moveObj = NULL;
	moveObj = addMoveToList(moveObj, location[0],location[1],(location[0]-2),(location[1] - 1));
	moveObj = addMoveToList(moveObj, location[0],location[1],(location[0]-2),(location[1] + 1));

	moveObj = addMoveToList(moveObj, location[0],location[1],(location[0]-1),(location[1] - 2));
	moveObj = addMoveToList(moveObj, location[0],location[1],(location[0]-1),(location[1] + 2));

	moveObj = addMoveToList(moveObj, location[0],location[1],(location[0]+1),(location[1] - 2));
	moveObj = addMoveToList(moveObj, location[0],location[1],(location[0]+1),(location[1] + 2));

	moveObj = addMoveToList(moveObj, location[0],location[1],(location[0]+2),(location[1] - 1));
	moveObj = addMoveToList(moveObj, location[0],location[1],(location[0]+2),(location[1] + 1));
	return ValidateMovesInBorders(moveObj);
}

SPCHESSMove *spChessGameGetQueenMoves(int location[]) {
	SPCHESSMove *moveObj = spChessGameGetRookMoves(location);
	return ConcatLists(moveObj, spChessGameGetBishopMoves(location));
}

SPCHESSMove *spChessGameGetKingMoves(int location[]) {
	SPCHESSMove *moveObj = NULL;

	//if (location[0] > 0) {//up
	moveObj = addMoveToList(moveObj, location[0],location[1],(location[0]-1),location[1]);
	//}
	//if (location[0] < (SP_CHESS_BOARD_SIZE - 1)) {//down
	moveObj = addMoveToList(moveObj, location[0],location[1],(location[0]+1),location[1]);
	//}
	//if (location[1] > 0) {//left
	moveObj = addMoveToList(moveObj, location[0],location[1],location[0],(location[1] - 1));
	//}
	//if (location[1] < (SP_CHESS_BOARD_SIZE - 1)) {//right
	moveObj = addMoveToList(moveObj, location[0],location[1],location[0],(location[1] + 1));
	//}
	//if (location[0] > 0 && location[1] > 0) {//up left
	moveObj = addMoveToList(moveObj, location[0],location[1],(location[0]-1),(location[1] - 1));
	//}
	//if (location[0] > 0 && location[1] < (SP_CHESS_BOARD_SIZE - 1)) {//up right
	moveObj = addMoveToList(moveObj, location[0],location[1],(location[0]-1),(location[1] + 1));
	//}
	//if (location[0] < (SP_CHESS_BOARD_SIZE - 1) && location[1] > 0) {//down left
	moveObj = addMoveToList(moveObj, location[0],location[1],(location[0]+1),(location[1] - 1));
	//}
	//if (location[0] < (SP_CHESS_BOARD_SIZE - 1) && location[1]< (SP_CHESS_BOARD_SIZE - 1)) {//down right
	moveObj = addMoveToList(moveObj, location[0],location[1],(location[0]+1),(location[1] + 1));
	//}

	return ValidateMovesInBorders(moveObj);;
}

SPCHESSMove* ValidateMovesInBorders(SPCHESSMove* moveObj) {
	SPCHESSMove* temp = moveObj;
	while(moveObj!= NULL && !isMoveInBorders(moveObj->move)) {
		temp = moveObj->next;
		temp->last = moveObj->last;
		free(moveObj);
		moveObj = temp;
	}

	while(temp!= NULL && temp->next!= NULL){
		if(!isMoveInBorders(temp->next->move)){
			free(temp->next);
			temp->next = temp->next->next;
		}
		else {
			temp = temp->next;
		}
	}
	return moveObj;
}

SPCHESSMove* ConcatLists(SPCHESSMove* moveObj, SPCHESSMove* moveObj1){
	if(!moveObj)
		return moveObj1;//todo test
	else {
		moveObj->last->next = moveObj1;
		return moveObj;
	}
}

SPCHESSMove* addMoveToList(SPCHESSMove* moveObj, int x, int y, int m, int n) {
	int move[4] = {x, y, m, n};

	//add another move
	SPCHESSMove *newMoveObj = (SPCHESSMove *) malloc(sizeof(SPCHESSMove));
	memcpy(newMoveObj->move, move, 4 * sizeof(int));
	newMoveObj->next = NULL;

	if (!moveObj) {
		newMoveObj->last = newMoveObj;
		return newMoveObj;
	} else {
		moveObj->last->next = newMoveObj;
		moveObj->last = newMoveObj;
		return moveObj;
	}

}

bool isInBorders(int x){
	if(x>=0 && x < SP_CHESS_BOARD_SIZE)
		return true;
	return false;
}

bool isMoveInBorders(int x[4]){
	return isInBorders(x[0]) && isInBorders(x[1])&&isInBorders(x[2])&&isInBorders(x[3]);
}

SPCHESSMove* spCHESSGameGetMoves(char gameBoard[SP_CHESS_BOARD_SIZE][SP_CHESS_BOARD_SIZE], int row, int col) {
	if (!gameBoard) {
		return NULL;
	}
	char pieceChar = gameBoard[row][col];
	int location[2] = {row, col};
	pieceType piece = (pieceType) tolower(pieceChar);
	switch (piece) {
		case pawn:
			return spChessGameGetPawnMoves(gameBoard, location);
		case bishop:
			return spChessGameGetBishopMoves(location);
		case rook :
			return spChessGameGetRookMoves(location);
		case knight :
			return spChessGameGetKnightMoves(location);
		case queen :
			return spChessGameGetQueenMoves(location);
		case king :
			return spChessGameGetKingMoves(location);
		default:
			break;
	}
	return NULL;
}

void FreeAllMoves(SPCHESSMove* moveList){
	SPCHESSMove* temp = moveList;
	while(temp != NULL){
		moveList = temp->next;
		free(temp);
		temp=moveList;
	}


}