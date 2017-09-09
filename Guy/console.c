#include "console.h"
//todo - validate memory all over the program
//todo - when quit - free all memory


// main function
int main(int argc, char **argv){
	if(argc == 1){console();}
		//else if(argc== 2 && strcmp(argv[1],"-g")==0){gui(board);}//todo
	else if(argc==2 && strcmp(argv[1],"-c")==0){console();}
	else{exit(0);}
	return 0;
}

int console() {
	int diffLevel = 2; // variable to save the current difficult level enterd from the user
	int mode = 1;
	int userColor = 1;
	SPCHESSGame *game = NULL; //current game object
	char buffer[SP_MAX_LINE_LENGTH]; // help buffer used to read command from user with max length of 1024
	bool gameOver = false, memoryFailed = false, gameInitialized = false;// initialize boolean variabels that will help us handle specail states we are in
	SPCommand com; // current command object
	com.cmd = SP_Init; // init command to start the game

	//loop of steps until quit command is requested. each iteration will read a command from a user, and then handle it according to command type;

	while (com.cmd != SP_QUIT) {
		settingsLoop(game, buffer, &diffLevel, &mode, &userColor, &memoryFailed, &com);
		if (memoryFailed)
			break;

		if (com.cmd != SP_QUIT) {
			game = CreateGame(gameInitialized, diffLevel, game, &memoryFailed, mode);
			gameOver = false;// game was initialized so if game was over - now its not
			gameInitialized = true;
			if(game == NULL || memoryFailed) {
				memoryFailed = true;
				break;
			}

		}

		GameLoop(diffLevel, mode, userColor, game, buffer, &gameOver, &memoryFailed, &com);
		if (memoryFailed ||	gameOver) {
			// if any of the functions changed the memoryFailed flag - there was a problem and we need to exit before reading from user.
			break;
		}

		if (com.cmd == SP_RESET) {
			//handling reset in settings part because we exit the game part when user enters reset
			printf("Restarting...\n");
			com.cmd = SP_Init;
		}

	}
	printf("Exiting...\n"); //todo only writen in game part - for real?
	if (gameInitialized)
		spCHESSGameDestroy(game); //free all memory - we only hold one object of game that we need to free in a spefic time
	return 0;
}


