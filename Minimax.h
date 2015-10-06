#include "Chess.h"
#include "Chess_logic.h"

extern int minimax_depth;		

int run_minimax(board_tile board[BOARD_SIZE][BOARD_SIZE], linked_list* possible, int depth, char color);
int minimax_algo(board_tile board[BOARD_SIZE][BOARD_SIZE], linked_list* possible, int depth, int max, int a, int b, char color);
int is_leaf(board_tile board[BOARD_SIZE][BOARD_SIZE], int depth, char color);
void copy_board(board_tile origin[BOARD_SIZE][BOARD_SIZE], board_tile copy[BOARD_SIZE][BOARD_SIZE]);
int calc_best_depth(board_tile board[BOARD_SIZE][BOARD_SIZE], char color);
int calc_piece_moves(board_tile tile);
int flip_max(int max);
int determine_v(int v, int tmp_v, int max);
int change_a(int a, int v, int max);
int change_b(int b, int v, int max);
int prune(int v, int max, int a, int b);