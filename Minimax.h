#include "Chess.h"
#include "Chess_logic.h"

extern int minimax_depth;		

int run_minimax(board_tile board[BOARD_SIZE][BOARD_SIZE], char color, game_move** best);
int minimax_algo(board_tile board[BOARD_SIZE][BOARD_SIZE], int depth, int max, int a, int b, game_move** best, char color, int top);
int is_leaf(board_tile board[BOARD_SIZE][BOARD_SIZE], int depth, char color);
void copy_board(board_tile origin[BOARD_SIZE][BOARD_SIZE], board_tile copy[BOARD_SIZE][BOARD_SIZE]);
int flip_max(int max);
int determine_v(int v, int tmp_v, int max);
int change_a(int a, int v, int max);
int change_b(int b, int v, int max);
int prune(int v, int max, int a, int b);