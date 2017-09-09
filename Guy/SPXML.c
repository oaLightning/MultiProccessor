#include "SPXML.h"

//todo is copied so change names


//todo add to xml all fields in game

//create the xml file with all the parameters
void create_xml(FILE *fb, SPCHESSGame* src, int mode, int userColor){
	fprintf(fb, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
	fprintf(fb, "<game>\n");
	fprintf(fb, "\t<next_turn>%d</next_turn>\n", src->currentPlayer-1);
	fprintf(fb, "\t<game_mode>%d</game_mode>\n", mode);
	if(mode ==1) { // only write in one player game
		fprintf(fb, "\t<difficulty>%d</difficulty>\n", src->diff);
		fprintf(fb, "\t<user_color>%d</user_color>\n", userColor - 1);
	}
	fprintf(fb, "\t<board>\n");
	for (int i = SP_CHESS_BOARD_SIZE; i >0; i--){
		fprintf(fb, "\t\t<row_%d>%s</row_%d>\n",i, src->gameBoard[i],i);//was row
	}
	fprintf(fb, "\t</board>\n");
	fprintf(fb, "</game>\n");

}

//read the xml file and update all the parameters
//returns user color
int read_xml(FILE *fb, SPCHESSGame* src){
	char *s = read_next_line(fb);
	int currentPlayer=0, mode=0, diff=0, userColor=0;
	int row = 0;
	if (s == NULL){
		fclose(fb);
		exit(0);
	}
	while (strstr(s, "/board") == NULL){
		if (strstr(s, "next_turn") != NULL){
			char *index = strstr(s, ">");
			currentPlayer = readInt(index);
		}
		if (strstr(s, "game_mode") != NULL){
			char *index = strstr(s, ">");
			mode = readInt(index);
			src = spCHESSGameCreate(mode == 1, HISTORY_SIZE);
			if(src==NULL){
				fclose(fb);
				exit(0);
			}
		}
		if (strstr(s, "difficulty") != NULL){
			char *index = strstr(s, ">");
			diff = readInt(index);
		}
		if (strstr(s, "user_color") != NULL){
			char *index = strstr(s, ">");
			userColor = readInt(index);
		}
		if (strstr(s, "row") != NULL){
			char *index = strstr(s, ">");
			init_xml_board(index + 1, src, row);
			row++;
		}
		free(s);
		s = read_next_line(fb);
		if (s == NULL){
			fclose(fb);
			exit(0);
		}
	}
	free(s);
	src->currentPlayer = currentPlayer;
	src->diff = diff;
	return userColor;

}

int readInt(char *token){
	int var;
	sscanf (token, "%d", &var);
	return var;
}

//return the next line from the file fb
char *read_next_line(FILE *fb){
	char *s = (char*)malloc(10 * sizeof(char));
	char c;
	if (s == NULL){
		printf("Error: standard function %s has failed","malloc");
		return NULL;}
	int count = 10;
	int index = 0;
	while ((c= (char) fgetc(fb)) != '\n'){
		if (index == count-2){
			count=count + 10;
			s = (char*)realloc(s, count * sizeof(char));
			if (s == NULL){
				printf("Error: standard function %s has failed","realloc");
				return NULL;}
		}
		s[index] = c;
		index++;
	}
	s[index] = '\0';
	return s;
}

//update board variable
void init_xml_board(char *s,SPCHESSGame* src,int row){
	for (int i = 0; i < SP_CHESS_BOARD_SIZE; i++){
		src->gameBoard[i][row] = s[i];
	}
}



