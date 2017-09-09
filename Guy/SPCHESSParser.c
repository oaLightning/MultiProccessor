#include "SPCHESSParser.h"

bool spParserIsInt(const char* str)
{
	int i=0, j;
	if(str[i]=='+' || str[i]=='-'){
			//check if there is a sign in the beggining of the string
			i++;
		}

	for(j=i; j<((int)strlen(str)); j++){ //i will hold the first digit place(whether there is a sign or not)
			if(str[j]<'0' || str[j]>'9'){
					//check that every digit is a number, if one of them is not - return false.
					return false;
				}
		}
	return true;
}

SPCommand spParserParseLine(const char* str)
{
    //initialize command
	SPCommand com;
	com.cmd = SP_INVALID_LINE;
	com.cmd=-1;
    char *token;
    char localString[SP_MAX_LINE_LENGTH];
    strcpy(localString, str);//todo should free? or free token?

    if(str) { // if any input was read - return invalid
		token = strtok((char *) localString, STRTOK_DELIMITER); //split the line using a delimiter
		if (token != NULL) { // if could not split -  return invalid
			//check that there is a first argument.

			//we need to get the command
			com.cmd = ParseCmdFromString(token);


			token = strtok(NULL, STRTOK_DELIMITER);

				switch (com.cmd) {
					case SP_GAME_MODE  :
					case SP_DIFFICULTY  :
					case SP_USER_COLOR  :
						if (token != NULL && spParserIsInt(token)) {
							com.arg = atoi(token);
						} else {
							com.cmd = SP_INVALID_LINE;
							return com;
						}
						token = strtok(NULL, STRTOK_DELIMITER);
						if (token != NULL) {
							//validate that there is no additional token
							com.cmd = SP_INVALID_LINE;
							while(strtok(NULL, STRTOK_DELIMITER) != NULL);//todo-validate all
							return com;
						}
						break;

					case SP_LOAD  :
					case SP_SAVE:
						//commands should get path as parameter (todo-space is ok?)
						if (token != NULL) {
							com.path = token;

							token = strtok(NULL, STRTOK_DELIMITER);
							if (token != NULL) {
								//validate that there is no additional token
								com.cmd = SP_INVALID_LINE;
								while(strtok(NULL, STRTOK_DELIMITER) != NULL);
								return com;
							}
							return com;
						}
						//if we didnt return any valid command - notenough tokens
						com.cmd = SP_INVALID_LINE;
						return com;
					case SP_MOVE  :
						//syntax: move <7,1> to <6,1>
						if (token != NULL) {
							//token is <x,y>
							//todo validate token is in length, write more readable?
							if(token[0] == '<' && token[2] == ',' && token[4] == '>') {
								com.move[0] = (SP_CHESS_BOARD_SIZE - 1) - (token[1] - '0' - 1);
								com.move[1] = token[3] - 'A';
								token = strtok(NULL, STRTOK_DELIMITER);//read to
								if (token != NULL && strcmp(token, "to") == 0) {
									token = strtok(NULL, STRTOK_DELIMITER);//read next numbers
									if (token != NULL) {
										if(token[0] == '<' && token[2] == ',' && token[4] == '>') {
											com.move[2] = (SP_CHESS_BOARD_SIZE - 1) - (token[1] - '0' - 1);
											com.move[3] = token[3] - 'A';

											token = strtok(NULL, STRTOK_DELIMITER);
											if (token != NULL) {
												//validate that there is no additional token
												com.cmd = SP_INVALID_LINE;
												while (strtok(NULL, STRTOK_DELIMITER) != NULL);
												return com;
											}
											return com;
										}
									}
								}
							}
						}
						//if we didnt return any valid command - notenough tokens
						com.cmd = SP_INVALID_LINE;
						return com;
					case SP_GET_MOVES  :
						//syntax: get_moves <x,y>
						if (token != NULL) {
							//token is <x,y>
							//todo validate token is in length, write more readable?
							if(token[0] == '<' && token[2] == ',' && token[4] == '>') {
								com.move[0] = (SP_CHESS_BOARD_SIZE - 1) - (token[1] - '0' - 1);
								com.move[1] = token[3] - 'A';
								token = strtok(NULL, STRTOK_DELIMITER);//read next numbers
								if (token != NULL) {
									//validate that there is no additional token
									com.cmd = SP_INVALID_LINE;
									while (strtok(NULL, STRTOK_DELIMITER) != NULL);
									return com;
								}
							}
						}
						//if we didnt return any valid command - notenough tokens
						com.cmd = SP_INVALID_LINE;
						return com;
					case SP_PRINT_SETTING :
					case SP_DEFAULT:
					case SP_START  :
					case SP_QUIT:
					case SP_UNDO  :
					case SP_RESET  :
						//if there wasn't suppose to be another token - handle
						if(token != NULL) {
							com.cmd = SP_INVALID_LINE;
							while(strtok(NULL, STRTOK_DELIMITER) != NULL);
						}
						return com;
					case SP_INVALID_LINE:
					default:
						while(strtok(NULL, STRTOK_DELIMITER) != NULL);
						com.cmd = SP_INVALID_LINE;
						break;
				}
		}
	}
	return com;
}

SP_COMMAND ParseCmdFromString(char* token){
	if(strcmp(token,"move") == 0)
		return SP_MOVE;
	if(strcmp(token,"get_moves") == 0)
		return SP_GET_MOVES;
	if(strcmp(token,"save") == 0)
		return SP_SAVE;
	if(strcmp(token,"undo") == 0)
		return SP_UNDO;
	if(strcmp(token,"reset") == 0)
		return SP_RESET;
	if(strcmp(token,"game_mode") == 0)
		return SP_GAME_MODE;
	if(strcmp(token,"difficulty") == 0)
		return SP_DIFFICULTY;
	if(strcmp(token,"user_color") == 0)
		return SP_USER_COLOR;
	if(strcmp(token,"load") == 0)
		return SP_LOAD;
	if(strcmp(token,"print_setting") == 0)
		return SP_PRINT_SETTING;
	if(strcmp(token,"default") == 0)
		return SP_DEFAULT;
	if(strcmp(token,"start") == 0)
		return SP_START;
	if(strcmp(token,"quit") == 0)
		return SP_QUIT;
	return SP_INVALID_LINE;
}

SPCommand readCommand(char buffer[SP_MAX_LINE_LENGTH], bool *memoryFailed)
{
    if(fgets( buffer, SP_MAX_LINE_LENGTH, stdin) == NULL){// read the user input using fgets
        //validate that there was no memory allocation error
        printf("Error: fgets has failed\n");
        *memoryFailed=true;
		return spParserParseLine(NULL);
    }
	return spParserParseLine(buffer);
}

pieceType readPieceType(char buffer[SP_MAX_LINE_LENGTH], bool *memoryFailed)
{
	if(fgets( buffer, SP_MAX_LINE_LENGTH, stdin) == NULL){// read the user input using fgets
		//validate that there was no memory allocation error
		printf("Error: fgets has failed\n");
		*memoryFailed=true;
		return ParsePieceFromString(NULL);
	}
	return ParsePieceFromString(buffer);
}

pieceType ParsePieceFromString(char* token){

	if(token) {
		if (strcmp(token, "pawn") == 0)
			return pawn;
		if (strcmp(token, "queen") == 0)
			return queen;
		if (strcmp(token, "rook") == 0)
			return rook;
		if (strcmp(token, "knight") == 0)
			return knight;
		if (strcmp(token, "bishop") == 0)
			return bishop;
	}
	return none;
}

