
#ifndef GAME_FLOW_
#define GAME_FLOW_

#include "Chess.h"
#include "Linked_list.h"


typedef struct board_tile board_tile;
typedef struct game_move game_move;

/*represents a tile on the board*/
struct board_tile
{
	int char_indexer; /*col_num - first*/
	int int_indexer; /*row_num - second*/
	char type; /*type of the piece in the tile. if no piece, EMPTY */
	char color; /*color of the piece in the tile. if no piece, EMPTY*/
};

/*represent a movement with the game piece on "start" tile*/
struct game_move
{
	board_tile start;
	board_tile end;
	char promote;
	int score;
};

extern board_tile board[BOARD_SIZE][BOARD_SIZE]; /*game board*/
extern int should_terminate;
extern char user_color; /*color of the user player*/
extern int is_user_turn;
extern int player_vs_player; /*1 - player vs player mode. 2 - player vs comp. 0 (for debug only) - comp vs comp*/
extern char next_player;
extern int gui; /*0 for command line. 1 for gui*/

/*memory issues*/
void free_moves(linked_list list);

/*print and read functions*/
void print_board(board_tile board[BOARD_SIZE][BOARD_SIZE]);
void print_moves(board_tile board[BOARD_SIZE][BOARD_SIZE], char color);
void print_single_move(game_move move);
void print_line();
void print_tile(board_tile tile);
int read_user_input_line(char* input, int* input_size);
int cmp_input_command(char* input, char* cmd);

/*game flow*/
int check_game_end(char player_color);
char *get_xml_game();
void load_config(char *file_data);
int save_config(char * config_file_name);
int user_move(char* input, char player_color);
int settings(char* input);
int do_computer_move(char color);


/*help functions*/
void concat(char *orig, size_t *orig_size, char *addition);
char flip_color(char color);
int count_piece(color, type);
char get_color(char c);
int tile_cmp(board_tile a, board_tile b);
int move_cmp(game_move a, game_move b);
int out_of_boarders(int first_indexer, int second_indexer);
char get_tool_type(char color, char type);

#endif GAME_FLOW_