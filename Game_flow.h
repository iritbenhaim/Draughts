
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
	int moved; /*boolean variable marking that the original piece in this tile moved*/
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
extern int player_vs_player; /*1 - player vs player mode. 2 - player vs comp. 0 (for debug only) - comp vs comp*/
extern char next_player;
extern int gui; /*0 for command line. 1 for gui*/
extern int check; /*is 1 if game is in check*/
extern int tie; /*is 1 if game in tie*/
extern int mate; /*is 1 if game in mate*/

/*memory*/
void free_moves(linked_list list);

/*print and read functions*/
void print_board(board_tile board[BOARD_SIZE][BOARD_SIZE]);
int print_moves(board_tile board[BOARD_SIZE][BOARD_SIZE], board_tile tile, char color);
void print_single_move(game_move move);
void print_line();
void print_tile(board_tile tile);
void print_best_moves(board_tile board[BOARD_SIZE][BOARD_SIZE], char color, int depth);
int read_user_input_line(char* input, int* input_size);
int cmp_input_command(char* input, char* cmd);

/*game flow*/
int main_cmd();
int get_move(char *input, game_move* move, char player_color);
int check_game_end(char player_color);
char *get_xml_game();
int load_config(char *file_data);
int save_config(char * config_file_name);
int user_move(char* input, char player_color);
int settings(char* input);
int do_computer_move(char color);

/*help functions*/
void concat(char *orig, size_t *orig_size, char *addition);
char flip_color(char color);
int count_piece(char color, char type);
char get_color(char c);
int tile_cmp(board_tile a, board_tile b);
int out_of_boarders(int first_indexer, int second_indexer);
char get_tool_type(char color, char type);
game_move choose_rand_move(linked_list moves);

#endif GAME_FLOW_