
#ifndef SPXML_H
#define SPXML_H
#include "SPCHESSGame.h"
#include <string.h>
#include <stdio.h>
#define HISTORY_SIZE 6


int readInt(char *token);
void create_xml(FILE *fb, SPCHESSGame* src, int mode, int userColor);
int read_xml(FILE *fb, SPCHESSGame* src);
char *read_next_line(FILE *fb);
void init_xml_board(char *s,SPCHESSGame* src,int row);
#endif //SPXML_H
