#include "Chess.h"
#include "Chess_logic.h"

extern int minimax_depth;		


int minimax(board_tile board[BOARD_SIZE][BOARD_SIZE], int depth, int maximize, game_move** best, char color, int top);