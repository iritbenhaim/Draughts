# all dependency files needed
EXES=Chess_logic Game_flow Gui Linked_list Minimax
SOURCES=Chess_logic.c Game_flow.c Gui.c Linked_list.c Minimax.c
HEADERS=Chess_logic.h Game_flow.h Gui.h Linked_list.h Minimax.h Chess.h
OBJECTS=$(SOURCES:.c=.o)

# flags
CFLAGS= -std=c99 -pedantic-errors -Wall -g `sdl-config --cflags`
OFLAGS= -lm -std=c99 -pedantic-errors -g `sdl-config --libs`

# executables to make
all: Game_flow Chess_logic

# files to clean - exes and objects
clean:
	-rm Chess_logic Game_flow Gui Linked_list Minimax
	-rm Chess_logic.o Game_flow.o Gui.o Linked_list.o Minimax.o

######### make #########
# make executables
Chess_logic: $(OBJECTS)
	gcc -o Chess_logic $(OBJECTS) $(OFLAGS)

Game_flow: $(OBJECTS)
	gcc -o Game_flow $(OBJECTS) $(OFLAGS)

Gui: $(OBJECTS)
	gcc -o Gui $(OBJECTS) $(OFLAGS)

Linked_list: $(OBJECTS)
	gcc -o Linked_list $(OBJECTS) $(OFLAGS)

Minimax: $(OBJECTS)
	gcc -o Minimax $(OBJECTS) $(OFLAGS)

# make objects
Chess_logic.o: $(SOURCES) $(HEADERS)
	gcc $(CFLAGS) -c Chess_logic.c 

Game_flow.o: $(SOURCES) $(HEADERS)
	gcc $(CFLAGS) -c Game_flow.c

Gui.o: $(SOURCES) $(HEADERS)
	gcc $(CFLAGS) -c Gui.c

Linked_list.o: $(SOURCES) $(HEADERS)
	gcc $(CFLAGS) -c Linked_list.c

Minimax.o: $(SOURCES) $(HEADERS)
	gcc $(CFLAGS) -c Minimax.c