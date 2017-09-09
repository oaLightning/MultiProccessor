CC = gcc
OBJS = console.o SPMainAux.o SPXML.o SPCHESSParser.o SPMiniMax.o SPCHESSGame.o SPArrayList.o SPGetMoves.o
 
EXEC = prog
CC_COMP_FLAG = -std=c99 -Wall -Wextra \
-Werror -pedantic-errors
SDL_COMP_FLAG = -I/usr/local/lib/sdl_2.0.5/include/SDL2 -D_REENTRANT
SDL_LIB = -L/usr/local/lib/sdl_2.0.5/lib -Wl,-rpath,/usr/local/lib/sdl_2.0.5/lib -Wl,--enable-new-dtags -lSDL2 -lSDL2main

$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(SDL_LIB) -o $@

SPArrayList.o: SPArrayList.c SPArrayList.h
	$(CC) $(CC_COMP_FLAG) $(SDL_COMP_FLAG) -c SPArrayList.c

SPCHESSGame.o: SPCHESSGame.c SPCHESSGame.h SPGetMoves.h SPArrayList.h
	$(CC) $(CC_COMP_FLAG) $(SDL_COMP_FLAG) -c SPCHESSGame.c

SPGetMoves.o: SPGetMoves.c SPGetMoves.h
	$(CC) $(CC_COMP_FLAG) $(SDL_COMP_FLAG) -c SPGetMoves.c

SPMiniMax.o: SPMiniMax.c SPMiniMax.h SPCHESSGame.h SPGetMoves.h SPArrayList.h
	$(CC) $(CC_COMP_FLAG) $(SDL_COMP_FLAG) -c SPMiniMax.c

SPCHESSParser.o: SPCHESSParser.c SPCHESSParser.h SPCHESSGame.h SPGetMoves.h SPArrayList.h
	$(CC) $(CC_COMP_FLAG) $(SDL_COMP_FLAG) -c SPCHESSParser.c

SPXML.o: SPXML.c SPXML.h SPCHESSGame.h SPGetMoves.h SPArrayList.h
	$(CC) $(CC_COMP_FLAG) $(SDL_COMP_FLAG) -c SPXML.c

SPMainAux.o: SPMainAux.c SPMainAux.h SPXML.h SPCHESSParser.h SPMiniMax.h SPCHESSGame.h SPGetMoves.h SPArrayList.h
	$(CC) $(CC_COMP_FLAG) $(SDL_COMP_FLAG) -c SPMainAux.c

console.o: console.c console.h SPMainAux.h SPXML.h SPCHESSParser.h SPMiniMax.h SPCHESSGame.h SPGetMoves.h SPArrayList.h
	$(CC) $(CC_COMP_FLAG) $(SDL_COMP_FLAG) -c console.c

clean:
	rm -f *.o $(EXEC) $(UNIT_TESTS)
