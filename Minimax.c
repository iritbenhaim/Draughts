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

int minimax_depth = 1;		/*levels considered in minimax tree. -1: means difficulty best – best argument allows 
							the computer to execute minimax algorithm in different depths, depending on the state of
							the game. Using this option, the number of evaluated boards should not exceed 10^-6 */

/*wrapper function for the minimax algorithm which determines all default values and runs minimax*/
int run_minimax(board_tile board[BOARD_SIZE][BOARD_SIZE], linked_list* possible, int depth, char color, game_move** best)
{
	int a, b, top, max, minimax_depth;
	a = INT_MIN;
	b = INT_MAX;
	top = 1;
	max = 1;
	if (depth == -1)
		minimax_depth = calc_best_depth(board, color); /*calculate best possible depth*/
	else
		minimax_depth = depth;
	return minimax_algo(board, possible, minimax_depth, max, a, b, best, color, top);
}

/*running minimax algorithm with alpha-beta pruning*/
int minimax_algo(board_tile board[BOARD_SIZE][BOARD_SIZE], linked_list* possible, 
	int depth, int max, int a, int b, game_move** best, char color, int top)
{
	int v = max ? INT_MIN : INT_MAX;
	int tmp_v;

	if (is_leaf(board, depth, color))	/*reached max depth or a winning/losig board*/
	{
		*possible = new_list();
		if (should_terminate)
			return -1;
		return score(board, color);
	}

	*possible = generate_moves(board, max ? color : flip_color(color), 1, 1); /*all possible moves for current player*/
	if (should_terminate)
		return -1;
	node* crnt = possible->first;

	for (int i = 0; i < possible->len; i++, crnt = crnt->next)
	{
		board_tile copy[BOARD_SIZE][BOARD_SIZE];
		copy_board(board, copy);
		do_move(copy, *(game_move*)(crnt->data));
		linked_list next_level;
		tmp_v = minimax_algo(copy, &next_level, depth - 1, flip_max(max), a, b, best, color, 0);
		free_moves(next_level);
		if (determine_v(v, tmp_v, max))		/*change node value and best move if needed*/
		{
			v = tmp_v;
			if (top)
			{
				*best = copy_move((game_move*)(crnt->data));
				if (should_terminate)
				{
					return -1;
				}
			}
		}
		((game_move*)(crnt->data))->score = v;
		a = change_a(a, v, max);	/*update alpha - only for max node*/
		b = change_b(b, v, max);	/*update beta - only for min node*/
		if (prune(v, max, a, b))	/*prune sub-tree if possible*/
			break;
	}
	return v;
}


/*checks if current node in minimax tree is a leaf - 
if we reached minimax depth or some player won or tied*/
int is_leaf(board_tile board[BOARD_SIZE][BOARD_SIZE], int depth, char color)
{
	if (depth == 0)
		return 1;
	if (get_winner(board, color) != 0)
		return 1;
	if (player_in_tie(board, color))
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

/*returns the best depth while boards don't exceed 1000000*/
int calc_best_depth(board_tile board[BOARD_SIZE][BOARD_SIZE], char color)
{
	int my_moves = 0;
	int other_moves = 0;
	int sum_moves;
	int depth = 0;
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			if (board[i][j].color == color)
				my_moves += calc_piece_moves(board[i][j]); /*current player piece*/
			if (board[i][j].color == flip_color(color))
				other_moves += calc_piece_moves(board[i][j]); /*other player piece*/

		}
	}
	sum_moves = my_moves;
	while (sum_moves < 1000000) /*check no. of boards don't exceed 1000000*/
	{
		if (depth % 2 == 0)
			sum_moves *= other_moves;
		else
			sum_moves *= my_moves;
		depth += 1;
	}
	return depth;
}

/*return max no. of moves for piece*/
int calc_piece_moves(board_tile tile)
{
	switch (tile.type)
	{
	case KING:
		return 8;
	case QUEEN:
		return 13 + 14;
	case ROOK:
		return 14;
	case BISHOP:
		return 13;
	case KNIGHT:
		return 8;
	case PAWN:
		return 3;
	default:
		return 0;
	}
}
