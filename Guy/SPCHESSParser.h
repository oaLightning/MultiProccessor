#ifndef SPCHESSPARSER_H_
#define SPCHESSPARSER_H_
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "SPCHESSGame.h"
//specify the maximum line length
#define SP_MAX_LINE_LENGTH 1024
#define STRTOK_DELIMITER " \t\r\n"

//a type used to represent a command
typedef enum {
	SP_MOVE,//move
	SP_GET_MOVES,//get moves
	SP_SAVE,//save
	SP_UNDO,//undo
	SP_RESET,//reset

	//Setting:
	SP_GAME_MODE,//game_mode
	SP_DIFFICULTY,//difficulty
	SP_USER_COLOR,//user_color
	SP_LOAD,//load
	SP_PRINT_SETTING,//print_setting
	SP_START,//start
	SP_DEFAULT,//default

	SP_QUIT,//quit
	SP_INVALID_LINE,
	SP_Init
} SP_COMMAND;

//a new type that is used to encapsulate a parsed line
typedef struct command_t {
	SP_COMMAND cmd;
	int arg;
	int move[4];
	char *path;//todo should free path - getting it from token, also should free com when reading a new one? no malloc

} SPCommand;

/**
 * Checks if a specified string represents a valid integer. It is recommended
 * to use this function prior to calling the standard library function atoi.
 *
 * @return
 * true if the string represents a valid integer, and false otherwise.
 */
bool spParserIsInt(const char* str);

/**
 * Parses a specified line. If the line is a command which has an integer
 * argument then the argument is parsed and is saved in the field arg and the
 * field validArg is set to true. In any other case then 'validArg' is set to
 * false and the value 'arg' is undefined
 *
 * @return
 * A parsed line such that:
 *   cmd - contains the command type, if the line is invalid then this field is
 *         set to INVALID_LINE
 *   validArg - is set to true if the command is add_disc and the integer argument
 *              is valid
 *   arg      - the integer argument in case validArg is set to true
 */
SPCommand spParserParseLine(const char* str);

/**
 * Checks if a specified string represents a valid command as described in the exrecise
 *
 * @return
 * SP_COMMAND value of the command from the user, or SP_INVALID_LINE if not described
 */
SP_COMMAND ParseCmdFromString(char* token);

/**
 * Reads a user input from std in and then Parses a specified line.
 * If the line is a command which has an integer
 * argument then the argument is parsed and is saved in the field arg and the
 * field validArg is set to true. In any other case then 'validArg' is set to
 * false and the value 'arg' is undefined
 *
 * @param buffer - an helper char array with max size of the input line
 *
 * @return
 * A parsed line such that:
 *   cmd - contains the command type, if the line is invalid then this field is
 *         set to INVALID_LINE
 *   validArg - is set to true if the command is add_disc and the integer argument
 *              is valid
 *   arg      - the integer argument in case validArg is set to true
 *
 * this function will also change the @param memoryFailed flag to true if any memory allocation problem were thrown
 */
SPCommand readCommand(char buffer[SP_MAX_LINE_LENGTH], bool *memoryFailed);


pieceType readPieceType(char buffer[SP_MAX_LINE_LENGTH], bool *memoryFailed);

pieceType ParsePieceFromString(char* token);

#endif
