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

#define perror_message(func_name) (fprintf(stderr, "Error: standard function %s has failed\n", func_name))
#define print_message(message) (printf("%s", message));

typedef struct board_tile board_tile;
typedef struct game_move game_move;
typedef struct node node;
typedef struct linked_list linked_list;

int contains_jump(game_move* cur_move, board_tile second, board_tile first);
int same_tile(board_tile first, board_tile second);
int is_legal_move(game_move move, char color);
game_move* copy_move(game_move* cur_move);
int out_of_boarders(int first_indexer, int second_indexer);
void generate_man_moves(board_tile tile, char color, linked_list* best_moves, int* num_eats);
void generate_king_moves(board_tile tile, char color, linked_list* best_moves, int* num_eats);
void generate_eater_moves(board_tile tile, char color, linked_list* best_moves, int* num_eats, game_move* cur_move);
linked_list new_list();
void free_moves(linked_list list);
void free_list(linked_list list);
void list_add(linked_list* list, void* data);
int game_move_list_cmp(linked_list list1, linked_list list2);
int find_move(linked_list possible_moves, game_move move);
char get_tile_color(board_tile b);
char get_tile_type(board_tile b);
linked_list generate_moves(board_tile board[BOARD_SIZE][BOARD_SIZE], char cur_player_color);
int is_board_init_legal();
char get_tool_type(char color, char type);
int get_board_position(char* input, int* i, int* j);
void print_board(board_tile board[BOARD_SIZE][BOARD_SIZE]);
void init_board(board_tile board[BOARD_SIZE][BOARD_SIZE]);
int read_user_input_line(char* input, int* input_size);
int cmp_input_command(char* input, char* cmd);
int minimax(board_tile board[BOARD_SIZE][BOARD_SIZE], int depth, int maximize, game_move* best, char color, int top);
int score(board_tile board[BOARD_SIZE][BOARD_SIZE], char color);
char flip_color(char color);
int settings(char* input);
int user_move(char* input);
void do_part_move(board_tile m_board[][BOARD_SIZE], board_tile start, board_tile end);
int do_computer_move(char color);
void do_move(board_tile m_board[][BOARD_SIZE], game_move move);
int up_direction(int start_c, int end_c, int is_white);
void print_moves(board_tile board[BOARD_SIZE][BOARD_SIZE], char color);
void print_single_move(game_move move);
void print_tile(board_tile tile);
int is_changed_to_king(char pawn, board_tile loc);
char get_winner(board_tile board[BOARD_SIZE][BOARD_SIZE], char color);

#endif  
