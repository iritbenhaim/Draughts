#define DEBUG 0

#include "Chess.h"
#include "Chess_logic.h"
#include "Game_flow.h"
#include "Minimax.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "Chess.h"
#include "Chess_logic.h"
#include <stdlib.h>

int minimax_depth = 1;		/*levels considered in minimax tree. -1: means we difficulty best – best argument allows 
							the computer to execute minimax algorithm in different depths, depending on the state of
							the game. Using this option, the number of evaluated boards should not exceed 10^-6 */

/*wrapper function for the minimax algorithm which determines all default values and runs minimax*/
int run_minimax(board_tile board[BOARD_SIZE][BOARD_SIZE], char color, game_move** best)
{
	int a, b, depth, top, max;
	a = INT_MIN;
	b = INT_MAX;
	depth = minimax_depth;
	top = 1;
	max = 1;
	return minimax_algo(board, depth, max, a, b, best, color, top);
}

/*running minimax algorithm with alpha-beta pruning*/
int minimax_algo(board_tile board[BOARD_SIZE][BOARD_SIZE], int depth, int max, int a, int b, game_move** best, char color, int top)
{
	int v = max ? INT_MIN : INT_MAX;
	int tmp_v;
	linked_list possible;

	if (is_leaf(board, depth, color))	/*reached max depth or a winning/losig board*/
	{
		return score(board, color);
	}

	possible = generate_moves(board, max ? color : flip_color(color)); /*all possible moves for current player*/
	if (should_terminate)
	{
		free_moves(possible);
		return -1;
	}
	node* crnt = possible.first;

	for (int i = 0; i < possible.len; i++, crnt = crnt->next)
	{
		board_tile copy[BOARD_SIZE][BOARD_SIZE];
		copy_board(board, copy);
		do_move(copy, *(game_move*)(crnt->data));
		tmp_v = minimax_algo(copy, depth - 1, flip_max(max), a, b, best, color, 0);
		if (determine_v(v, tmp_v, max))		/*change node value and best move if needed*/
		{
			v = tmp_v;
			if (top)
				*best = (game_move*)(crnt->data);
		}
		a = change_a(a, v, max);	/*update alpha - only for max node*/
		b = change_b(b, v, max);	/*update beta - only for min node*/
		if (prune(v, max, a, b))	/*prune sub-tree if possible*/
			break;
	}
	/*if (top)
		*best = copy_move(*best);*/
	free_moves(possible);
	return v;

}


/*checks if current node in minimax tree is a leaf - 
if we reached minimax depth or some player won*/
int is_leaf(board_tile board[BOARD_SIZE][BOARD_SIZE], int depth, char color)
{
	if (depth == 0)
		return 1;
	if (get_winner(board, color) != 0)
		return 1;
	return 0;
}

/*creates a copy of origin board*/
void copy_board(board_tile origin[BOARD_SIZE][BOARD_SIZE], board_tile copy[BOARD_SIZE][BOARD_SIZE])
{
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			copy[i][j] = origin[i][j];
		}
	}
}

int flip_max(int max)
{
	return max ? 0 : 1;
}

/*determines if v should change according to new information*/
int determine_v(int v, int tmp_v, int max)
{
	if (max)
		return tmp_v > v ? 1 : 0;
	else
		return tmp_v < v ? 1 : 0;
}

/*returns updated vlaue of a*/
int change_a(int a, int v, int max)
{
	if (max)
		return v < a ? a : v;
	else
		return a;
}

/*returns updated value of b*/
int change_b(int b, int v, int max)
{
	if (max)
		return b;
	else
		return v < b ? v : b;
}

/*determines if tree can be pruned at this point*/
int prune(int v, int max, int a, int b)
{
	if (max)
		return b > v ? 0 : 1;
	else
		return a < v ? 0 : 1;
}

