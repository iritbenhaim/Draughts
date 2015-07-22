
#ifndef GAME_FLOW_
#define GAME_FLOW_

#include "Chess.h"
#include "Linked_list.h"


typedef struct board_tile board_tile;
typedef struct game_move game_move;

/*represents a tile on the board*/
struct board_tile
{
	int char_indexer; /*col_num - char*/
	int int_indexer; /*row_num - int*/
	char type2; /*type of the piece in the tile. if no piece, EMPTY */
	char color; /*color of the piece in the tile. if no piece, EMPTY*/
};

/*represent a movement with the game piece on "start" tile*/
struct game_move
{
	board_tile start;
	linked_list jumps;
};

extern board_tile board[BOARD_SIZE][BOARD_SIZE]; /*game board*/
extern int should_terminate;
extern char user_color; /*color of the user player*/
extern int is_user_turn;

int read_user_input_line(char* input, int* input_size);
int cmp_input_command(char* input, char* cmd);
int user_move(char* input);
int settings(char* input);

#endif GAME_FLOW_