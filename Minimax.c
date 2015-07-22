#define DEBUG 0

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "Chess.h"
#include "Chess_logic.h"

extern int should_terminate;
int minimax_depth = 1;		/*levels considered in minimax tree*/

/*runs minimax algorithm and gets the best move possible
returns the best move score*/
int minimax(board_tile board[BOARD_SIZE][BOARD_SIZE], int depth, int maximize, game_move** best, char color, int top)
{
	int tmp_val;
	int best_val;
	int end_game = 0;
	linked_list possible;
	possible = generate_moves(board, maximize ? color : flip_color(color)); /*all possible moves for current player*/

	if (DEBUG && top)
	{
		node* f = possible.first;
		for (int i = 0; i < possible.len; i++)
		{
			print_single_move(*(game_move*)f->data);
			f = f->next;
		}
	}
	if (should_terminate)
	{
		free_moves(possible);
		return -1;
	}
	node* crnt = possible.first;

	tmp_val = score(board, color); /*current board score*/
	end_game = get_winner(board, color);
	if (should_terminate)
	{
		free_moves(possible);
		return -1;
	}
	if (depth == 0 || end_game != 0) /*reached minimax depth or end of game*/
	{
		free_moves(possible);
		return tmp_val;
	}
	if (maximize)
	{
		best_val = INT_MIN;
		for (int i = 0; i < possible.len; i++, crnt = crnt->next)
		{
			/*copy board*/
			board_tile board_copy[BOARD_SIZE][BOARD_SIZE];
			for (int i = 0; i <BOARD_SIZE; i++)
			{
				for (int j = 0; j < BOARD_SIZE; j++)
				{
					board_copy[i][j] = board[i][j];
				}

			}
			do_move(board_copy, (*(game_move*)crnt->data));
			tmp_val = minimax(board_copy, depth - 1, 0, best, color, 0);
			if (tmp_val > best_val)
			{
				best_val = tmp_val;
				if (top)
					*best = (game_move*)crnt->data; /*only change move if this is depth 0*/
			}
		}
	}
	else
	{
		best_val = INT_MAX;
		for (int i = 0; i < possible.len; i++, crnt = crnt->next)
		{
			/*copy board*/
			board_tile board_copy[BOARD_SIZE][BOARD_SIZE];
			for (int i = 0; i <BOARD_SIZE; i++)
			{
				for (int j = 0; j < BOARD_SIZE; j++)
				{
					board_copy[i][j] = board[i][j];
				}

			}
			do_move(board_copy, (*(game_move*)crnt->data));
			tmp_val = minimax(board_copy, depth - 1, 1, best, color, 0);
			if (tmp_val < best_val)
			{
				best_val = tmp_val;
				if (top)
					*best = (game_move*)crnt->data; /*only change move if this is depth 0*/
			}
		}
	}
	if (top)
		*best = copy_move(*best);
	free_moves(possible);
	return best_val;
}


int new_minimax(board_tile board[BOARD_SIZE][BOARD_SIZE], int depth, int max, int a, int b, game_move** best, char color, int top)
{
	int v = max ? INT_MIN : INT_MAX;
	linked_list possible;
	possible = generate_moves(board, max ? color : flip_color(color)); /*all possible moves for current player*/
	if (should_terminate)
	{
		free_moves(possible);
		return -1;
	}

}

int is_leaf(board_tile board[BOARD_SIZE][BOARD_SIZE], int depth, char color)
{
	if (depth == 0)
		return 1;
	if (get_winner(board, color) != 0)
		return 1;
	return 0;
}

