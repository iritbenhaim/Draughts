#ifndef DRAUGHTS_
#define DRAUGHTS_

#include <stdio.h>


#define WHITE_M 'm'
#define WHITE_K 'k'
#define BLACK_M 'M'
#define BLACK_K 'K'
#define EMPTY ' '
#define BLACK 'B'
#define WHITE 'W'
#define KING 'k'
#define MAN 'm'

#define BOARD_SIZE 10

typedef char** board_t;
#define WELCOME_TO_DRAUGHTS "Welcome to Draughts!\n"
#define ENTER_SETTINGS "Enter game settings:\n" 
#define WRONG_MINIMAX_DEPTH "Wrong value for minimax depth. The value should be between 1 to 6\n"
#define WRONG_POSITION "Invalid position on the board\n"
#define NO_DICS "The specified position does not contain your piece\n"
#define ILLEGAL_COMMAND "Illegal command, please try again\n"
#define ILLEGAL_MOVE "Illegal move\n"
#define WROND_BOARD_INITIALIZATION "Wrong board initialization\n"
 
#define ENTER_YOUR_MOVE "Enter your move:\n" 
 
#define perror_message(func_name) (printf("Error: standard function %s has failed", func_name))
#define print_message(message) (printf("%s", message));

typedef struct board_tile board_tile;
typedef struct game_move game_move;
typedef struct scoring_board scoring_board;
typedef struct node node;
typedef struct linked_list linked_list;

void generate_man_moves(board_tile tile, char color, linked_list* best_moves, int* num_eats);
void generate_king_moves(board_tile tile, char color, linked_list* best_moves, int* num_eats);
linked_list new_list();
void free_moves(linked_list list);
void free_list(linked_list list);
void list_add(linked_list list, void* data);
char get_tile_color(board_tile b);
char get_tile_type(board_tile b);
game_move generate_moves(board_tile** board, char cur_player_color);
int is_board_init_legal();
char get_tool_type(char* color, char type);
int get_board_position(char* input, int* i, int* j);
void print_board(board_tile board[BOARD_SIZE][BOARD_SIZE]);
void init_board(board_tile board[BOARD_SIZE][BOARD_SIZE]);
int read_user_input_line(char* input, int* input_size);
int cmp_input_command(char* input, char* cmd);
int minimax(scoring_board board, int depth, int maximize);
int settings(char* input);
void do_part_move(board_tile** board, board_tile start, board_tile end);
void do_move(board_tile** board, game_move move);

#endif  
