#define _CRT_SECURE_NO_WARNINGS
#define DEBUG 0

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Draughts.h"

/*represents a tile on the board*/
struct board_tile
{
	int char_indexer; /*col_num - char*/
	int int_indexer; /*row_num - int*/
	char type;
};

/*linked list node*/
struct node
{
	void* data;
	node* next;
};

/*linked list*/
struct linked_list
{
	node* first;
	node* last;
	int len;
};

/*represent a movement with the game piece on "start" tile*/
struct game_move
{
	board_tile start;
	linked_list jumps;
};


int minmax_depth = 1; /*the debth of minmax algorithm*/
char user_color = WHITE; /*color of the user player*/
board_tile board[BOARD_SIZE][BOARD_SIZE]; /*game board*/
int should_terminate = 0;
int is_user_turn;

int main()
{
	unsigned int input_size = 1024;
	char* input = malloc(input_size);
	if (input == NULL)
	{
		should_terminate = 1;
		perror_message("malloc");
		return -1;
	}
	init_board(board);
	if (DEBUG)
		print_board(board);


	print_message(WELCOME_TO_DRAUGHTS);
	print_message(ENTER_SETTINGS);
	while (1)//!DEBUG)
	{ /*game settings*/
		if (read_user_input_line(input, &input_size) == -1)
		{
			return -1; /*no resources were allocated yet*/
		}
		if (settings(input))
			break;
		if (should_terminate)
		{
			free(input);
			return;
		}
	}
	is_user_turn = user_color == WHITE;
	while (1)
	{/*game play*/
		while (is_user_turn)
		{
			print_message(ENTER_YOUR_MOVE);
			if (read_user_input_line(input, &input_size) == -1)
			{
				return -1; /*no resources were allocated yet*/
			}
			int is_comp_turn = user_move(input);
			if (should_terminate)
			{
				free(input);
				return;
			}
			if (is_comp_turn == -1)
			{
				free(input);
				return;
			}
			is_user_turn = !is_comp_turn;
		}
		/*computer turn*/
		if (1==do_computer_move(flip_color(user_color)))
			break;
		is_user_turn = 1;
		if (DEBUG)
			print_board(board);

	}
	free(input);
	if (DEBUG)
	{
		getchar();
	}
	return 0;
}

/*gets user input, parses it, and run the command in it
returns 1 if user turn ended. -1 if game ended. else returns 0*/
int user_move(char* input)
{

	if (0 == cmp_input_command(input, "quit"))
	{
		should_terminate = 1;
		return 1;
	}
	if (0 == cmp_input_command(input, "get_moves"))
	{
		print_moves(board, user_color);
		return 0;
	}
	if (0 == cmp_input_command(input, "move "))
	{
		int i, j;
		int end_game;
		game_move move;
		char* input_copy = strchr(input, '>') + 1;
		if (0 == get_board_position(input, &i, &j))
			return 0;
		input = input_copy;
		if (out_of_boarders(i, j))
		{
			print_message(WRONG_POSITION);
			return 0;
		}
		
		move.start = board[i][j];
		move.jumps = new_list();
		if (should_terminate)
			return -1;
		while (input[0] == ' ')
			++input;
		input += strlen("to "); 
		while (input[0] == '<')
		{
			input_copy = strchr(input, '>') + 1;
			if (0 == get_board_position(input, &i, &j))
				return 0;
			input = input_copy;
			if (out_of_boarders(i, j))
			{
				print_message(WRONG_POSITION);
				free_list(move.jumps);
				return 0;
			}
			list_add(&move.jumps, &board[i][j]);
		}
		if (get_tile_color(move.start) != user_color)
		{
			print_message(NO_DICS);
			free_list(move.jumps);
			return 0;
		}
		int legal = is_legal_move(move, user_color);
		if (should_terminate)
		{
			free_list(move.jumps);
			return -1;
		}
		if (legal)
		{
			do_move(board, move);
			print_board(board);
		}
		else
		{
			print_message(ILLEGAL_MOVE);
			free_list(move.jumps);
			return 0;
		}

		free_list(move.jumps);
		end_game = get_winner(board, flip_color(user_color));
		if (should_terminate)
			return -1;
		if (end_game != 0)
		{
			char* winner = user_color == WHITE ? "White player wins!\n" : "Black player wins!\n";
			print_message(winner);
			return -1;
		}
		return 1;
	}
	print_message(ILLEGAL_COMMAND);
	return 0;
}

/*returns 1 if the move is legal for the player of color "color"*/
int is_legal_move(game_move move, char color)
{
	linked_list all_moves = generate_moves(board, color);
	if (should_terminate)
	{
		free_list(move.jumps);
		return 0;
	}
	int is_success = find_move(all_moves, move);

	free_moves(all_moves);
	return is_success;
}

/*return 1 if game ended*/
int do_computer_move(char color)
{
	int end_game;
	game_move* chosen_move = malloc(sizeof(game_move));
	if (chosen_move == NULL)
	{
		should_terminate = 1;
		perror_message("malloc");
		return 1;
	}
	int s = minimax(board, minmax_depth, 1, chosen_move, color, 1);
	if (s == INT_MIN)
		return 1;
	do_move(board, *chosen_move);
	print_message("Computer: move ");
	print_single_move(*chosen_move);
	print_board(board);
	end_game = get_winner(board, flip_color(user_color));
	if (should_terminate)
		return -1;
	if (end_game != 0)
	{
		print_message(user_color == WHITE ? "Black player wins!\n" : "White player wins!\n");
		return 1;
	}
	free_list(chosen_move->jumps);
	free(chosen_move);
	return 0;
}

/*prints to the user all his legal moves*/
void print_moves(board_tile board[BOARD_SIZE][BOARD_SIZE], char color)
{
	linked_list moves = generate_moves(board, color);
	node* crnt_move = moves.first;
	for (int i = 0; i < moves.len; i++)
	{
		print_single_move((*(game_move*)crnt_move->data));
		crnt_move = crnt_move->next;
	}
	free_moves(moves);
}

/*prints a single move in format "<x,y> to <i,j>[<k,l>…]\n" */
void print_single_move(game_move move)
{
	node crnt_node = *move.jumps.first;
	board_tile tile;
	print_tile(move.start);
	printf(" to ");
	for (int i = 0; i < move.jumps.len; i++)
	{
		tile = *(board_tile*)crnt_node.data;
		crnt_node = *crnt_node.next;
		print_tile(tile);
	}
	printf("\n");
}

/*prints a single board tile*/
void print_tile(board_tile tile)
{
	char index = 'a' + tile.char_indexer;
	printf("<%c,%d>", index, tile.int_indexer + 1);
}

/*runs the game settings phase of the game on a given command.
returns 1 if game start command was sent. else returns 0*/
int settings(char* input)
{
	int temp, i, j;
	char* input_copy;
	if (0 == cmp_input_command(input, "minimax_depth "))
	{
		temp = atoi(input + strlen("minimax_depth "));
		if (temp < 1 || temp > 6)
		{
			print_message(WRONG_MINIMAX_DEPTH)
		}
		else
		{
			minmax_depth = temp;
		}
		return 0;
	}
	if (0 == cmp_input_command(input, "user_color "))
	{
		input += strlen("user_color ");
		while (input[0] == ' ')
			++input;
		if (0 == cmp_input_command(input, "white"))
			user_color = WHITE;
		else if (0 == cmp_input_command(input, "black"))
			user_color = BLACK;
		return 0;
	}
	if (0 == cmp_input_command(input, "clear"))
	{
		for (i = 0; i < BOARD_SIZE; ++i)
		{
			for (j = 0; j < BOARD_SIZE; ++j)
			{
				board[i][j].type = EMPTY;
			}
		}
		return 0;
	}
	if (0 == cmp_input_command(input, "rm"))
	{
		if (0 == get_board_position(input, &i, &j))
			return 0;
		board[i][j].type = EMPTY;
		return 0;

	}
	if (0 == cmp_input_command(input, "set"))
	{
		char color;
		char type;
		input_copy = strchr(input, '>') + 2;
		if (0 == get_board_position(input, &i, &j))
			return 0;
		while (input_copy[0] == ' ')
			++input_copy;
		if (0 == cmp_input_command(input_copy, "white"))
			color = WHITE;
		else if (0 == cmp_input_command(input_copy, "black"))
			color = BLACK;
		else
			return 0;
		input_copy += 5;
		while (input_copy[0] == ' ')
			++input_copy;
		type = input_copy[0];
		board[i][j].type = get_tool_type(color, type);
		return 0; 
	}
	if (0 == cmp_input_command(input, "print"))
	{
		print_board(board);
		return 0;

	}
	if (0 == cmp_input_command(input, "quit"))
	{
		should_terminate = 1;
		return 0;
	}
	if (0 == cmp_input_command(input, "start"))
	{
		if (!is_board_init_legal())
		{
			print_message(WROND_BOARD_INITIALIZATION);
			return 0;
		}
		return 1;
	}
	print_message(ILLEGAL_COMMAND);
	return 0;
}

/*returns 0 if the board is empty or if there are discs of only one color
or if there are more than 20 discs of the same color
else returns 1*/
int is_board_init_legal()
{
	int count_black = 0;
	int count_white = 0;
	int i, j;
	char color;
	for (i = 0; i < BOARD_SIZE; i++)
	{
		for (j = 0; j < BOARD_SIZE; j++)
		{
			color = get_tile_color(board[i][j]);
			if (color == BLACK)
				++count_black;
			else if (color == WHITE)
				++count_white;
		}
	}
	if (!count_black || !count_white || count_black > 20 || count_white > 20)
		return 0;
	return 1;
}

/*if a player has won return its color
  if no one won yet return 0*/
char get_winner(board_tile board[BOARD_SIZE][BOARD_SIZE], char color)
{
	linked_list possible_moves;
	int board_score = score(board, color);
	if (board_score == 100)
		return color;
	if (board_score == -100)
		return flip_color(color);
	possible_moves = generate_moves(board, color);
	if (should_terminate)
		return -1;
	if (possible_moves.len == 0)
	{
		free_moves(possible_moves);
		return flip_color(color);
	}
	free_moves(possible_moves);
	possible_moves = generate_moves(board, flip_color(color));
	if (should_terminate)
		return -1;
	if (possible_moves.len == 0)
	{
		free_moves(possible_moves);
		return color;
	}
	free_moves(possible_moves);
	return 0;
}

/*returns the char used on board to represent the given type and color of tool*/
char get_tool_type(char color, char type)
{
	if (color == WHITE)
	{
		if (type == 'm')
		{
			return WHITE_M;
		}
		return WHITE_K;
	}
	if (type == 'm')
	{
		return BLACK_M;
	}
	return BLACK_K;
}

/*gets a string containing <x,y> - a board position.
fills i and j - the indieces pointed by x, y
returns 0 if the position is outside the range. else 1*/
int get_board_position(char* input, int* i, int* j)
{
	int temp;
	int start = strchr(input, '<') - input;
	int end = strchr(input, ',') - input;
	if (end - start != 2 || input[start + 1] < 'a' || input[start + 1] > 'j')
	{
		print_message(WRONG_POSITION);
		return 0;
	}
	*strchr(input, '>') = '\0';
	temp = atoi(input + end + 1);
	if (temp < 1 || temp > 10)
	{
		print_message(WRONG_POSITION);
		return 0;
	}
	*j = temp - 1;
	*i = input[start + 1] - 'a';
	if ((*i + *j) % 2 != 0)
	{
		print_message(WRONG_POSITION);
		return 0;
	}
	return 1;

}

/*return 0 if input ==  command*/
int cmp_input_command(char* input, char* cmd)
{
	if (strlen(input) >= strlen(cmd))
	{
		return memcmp(input, cmd, strlen(cmd));
	}
	return -1;
}

/*reads a line of input from the user. and returns it in "input"*/
int read_user_input_line(char* input, int* input_size)
{
	int i = 0;
	char c = ' ';
	int ch;
	while (c != '\0') /*read string from user. the string length can be varied*/
	{
		ch = getchar();
		if (ch == EOF)
		{
			free(input);
			should_terminate = 1;
			perror_message("getchar");
			return -1;
		}
		c = ch;
		if (i >= *input_size-1)
		{
			*input_size *= 2;
			input = realloc(input, *input_size);
			if (input == NULL)
			{
				should_terminate = 1;
				perror_message("realloc");
				return -1;
			}
		}
		if (c == '\n')
			c = '\0';
		input[i++] = c;
	}
	return 1;
}

/*runs minimax algorithm and gets the best move possible 
returns the best move score*/
int minimax(board_tile board[BOARD_SIZE][BOARD_SIZE], int depth, int maximize, game_move* best, char color, int top)
{
	int tmp_val;
	int best_val;
	int end_game = 0;
	linked_list possible;
	possible = generate_moves(board, color); /*all possible moves for current player*/

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
	{/*DO STUFF!*/}
	node* crnt = possible.first;
	
	tmp_val = score(board, color); /*current board score*/
	end_game = get_winner(board, color);
	if (should_terminate)
		return -1;
	if (depth == 0 || end_game != 0) /*reached minimax depth or end of game*/
		return tmp_val;
	if(maximize)
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
			tmp_val = minimax(board_copy, depth - 1, 0, best, flip_color(color), 0);
			if (tmp_val > best_val)
			{
				best_val = tmp_val;
				*best = top ? *(game_move*)crnt->data : *best; /*only change move if this is depth 0*/
			}
		}
		return best_val;
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
			tmp_val = minimax(board_copy, depth - 1, 1, best, flip_color(color), 0);
			if (tmp_val < best_val)
			{
				best_val = tmp_val;
				*best = top ? *(game_move*)crnt->data : *best; /*only change move if this is depth 0*/
			}
		}
		return best_val;
	}
}

/*returns the opposite color*/
char flip_color(char color)
{
	return BLACK == color ? WHITE: BLACK;
}

/*gets a linked list containing all possible moves for a player
cur_player_color - the color of the current player*/
linked_list generate_moves(board_tile board[BOARD_SIZE][BOARD_SIZE], char cur_player_color)
{
	int i, j;
	linked_list best_moves;
	int num_eats = 0;
	char type;

	best_moves = new_list();
	if (should_terminate)
		return best_moves;
	for (i = 0; i < BOARD_SIZE; ++i)
	{
		for (j = 0; j < BOARD_SIZE; j++)
		{
			if (cur_player_color != get_tile_color(board[i][j]))
				continue;
			type = get_tile_type(board[i][j]);
			if (type == KING)
			{
				generate_king_moves(board[i][j], cur_player_color, &best_moves, &num_eats);
			}
			else if (type == MAN)
			{
				generate_man_moves(board[i][j], cur_player_color, &best_moves, &num_eats);
				if (should_terminate)
					break;
			}
			else
				continue;
		}
	}
	return best_moves;
}

/*appends to the best_moves list all the legal man moves form the current tile.
if a better move was found, best moves will be freed and replaced.
tile - the tile in which the man is at*/
void generate_man_moves(board_tile tile, char color, linked_list* best_moves, int* num_eats)
{
	int direction = color == WHITE ? 1 : -1; //black goes downwards.
	game_move* cur_move = malloc(sizeof(game_move));
	if (cur_move == NULL)
	{
		should_terminate = 1;
		perror_message("malloc");
		return;
	}
	cur_move->jumps = new_list();
	cur_move->start = tile;
	if (should_terminate)
	{
		free(cur_move);
		return;
	}

	game_move* cur_move_copy = copy_move(cur_move);
	if (should_terminate)
	{
		free_list(cur_move->jumps);
		free(cur_move);
		return;
	}
	generate_eater_moves(tile, color, best_moves, num_eats, cur_move_copy);
	if (should_terminate)
	{
		free_list(cur_move->jumps);
		free(cur_move);
		return;
	}
	if (0 == *num_eats) /*no eats yet*/
	{
		for (int i = 1; i > -2; i -= 2) /*when i=1, move right. when i=0 move left*/
		{
			if (out_of_boarders((tile.char_indexer) + i, tile.int_indexer + direction))
				continue;
			board_tile* next = &board[(tile.char_indexer) + i][tile.int_indexer + direction];
			char c = get_tile_color(*next);
			if (next->type == EMPTY)
			{
				/*add the move to the best moves list*/
				list_add(&cur_move->jumps, next);
				if (should_terminate)
				{
					free_list(cur_move->jumps);
					free(cur_move);
					return;
				}
				list_add(best_moves, cur_move);
				if (should_terminate)
				{
					free_list(cur_move->jumps);
					free(cur_move);
					return;
				}
				/*malloc the next move*/
				cur_move = malloc(sizeof(game_move));
				if (cur_move == NULL)
				{
					should_terminate = 1;
					return;
				}
				cur_move->jumps = new_list();
				cur_move->start = tile;
				if (should_terminate)
				{
					free(cur_move);
					return;
				}
				continue;
			}
		}
	}
	free_list(cur_move->jumps);
	free(cur_move);
}

/*appends to the best_moves list all the legal king moves form the current tile.
if a better move was found, best moves will be freed and replaced.
tile - the tile in which the king is at*/
void generate_king_moves(board_tile tile, char color, linked_list* best_moves, int* num_eats)
{
	game_move* cur_move = malloc(sizeof(game_move));
	if (cur_move == NULL)
	{
		should_terminate = 1;
		perror_message("malloc");
		return;
	}
	cur_move->jumps = new_list();
	if (should_terminate)
	{
		free(cur_move);
		return;
	}
	cur_move->start = tile;

	for (int ud_direction = 1; ud_direction > -2; ud_direction -= 2) /*when ud_direction=1, move up. when ud_direction=-1 move down*/
	{
		for (int lr_direction = 1; lr_direction > -2; lr_direction -= 2) /*when lr_direction=1, move right. when lr_direction=-1 move left*/
		{
			for (int i = 1; !out_of_boarders(tile.char_indexer + lr_direction*i, tile.int_indexer + ud_direction*i); ++i)/*check me!!!*/
			{
				char tile_color = (get_tile_color(board[tile.char_indexer + lr_direction*i][tile.int_indexer + ud_direction*i]));

				if (EMPTY == tile_color)
				{
					if (*num_eats != 0)
						continue;
					board_tile* next = &board[(tile.char_indexer) + lr_direction*i][tile.int_indexer + ud_direction*i];
					/*add the move to the best moves list*/
					list_add(&cur_move->jumps, next);
					if (should_terminate)
					{
						free_list(cur_move->jumps);
						free(cur_move);
						return;
					}
					list_add(best_moves, cur_move);
					if (should_terminate)
					{
						free_list(cur_move->jumps);
						free(cur_move);
						return;
					}
					/*malloc the next move*/
					cur_move = malloc(sizeof(game_move));
					if (cur_move == NULL)
					{
						should_terminate = 1;
						perror_message("malloc");
						return;
					}
					cur_move->jumps = new_list();
					cur_move->start = tile;
					if (should_terminate)
					{
						free(cur_move);
						return;
					}
				}
				else if (tile_color == flip_color(color))
				{/*try eating the oponnent. then stop this direction*/
					if (out_of_boarders(tile.char_indexer + lr_direction*(i + 1), tile.int_indexer + ud_direction*(i + 1)))
						break;
					board_tile* next = &board[(tile.char_indexer) + lr_direction*(i + 1)][tile.int_indexer + ud_direction*(i + 1)];
					if (EMPTY != (*next).type)
						break;
					/*add cur eat to move*/
					list_add(&cur_move->jumps, next);
					if (should_terminate)
					{
						free_list(cur_move->jumps);
						free(cur_move);
						return;
					}
					game_move* cur_move_copy = copy_move(cur_move);
					if (should_terminate)
					{
						free_list(cur_move->jumps);
						free(cur_move);
						return;
					}
					generate_eater_moves(*next, color, best_moves, num_eats, cur_move_copy);
					if (should_terminate)
					{
						free_list(cur_move->jumps);
						free(cur_move);
						return;
					}
					break;
				}
				else /*the tile contains a man of your own color. stop this direction.*/
					break;
			}
		}
	}
	free_list(cur_move->jumps);
	free(cur_move);
}

/*appends to the best_moves list all the legal eater moves (man or ling eating moves) from the current tile.
if a better move was found, best moves will be freed and replaced.
tile - the tile in which the piece is at*/
void generate_eater_moves(board_tile tile, char color, linked_list* best_moves, int* num_eats, game_move* cur_move)
{
	if ((get_tile_type(cur_move->start) == MAN) && tile.int_indexer == (color == BLACK ? 0 : 9))
	{
		free_list(cur_move->jumps);
		free(cur_move);
		return;
	}


	int used_cur_move = 0;
	int old_eats = *num_eats;
	for (int ud_direction = 1; ud_direction > -2; ud_direction -= 2) /*when ud_direction=1, move up. when ud_direction=-1 move down*/
	{
		for (int lr_direction = 1; lr_direction > -2; lr_direction -= 2) /*when lr_direction=1, move right. when lr_direction=-1 move left*/
		{
			int dest_lr = (tile.char_indexer) + lr_direction * 2;
			int dest_ud = tile.int_indexer + ud_direction * 2;
			if (out_of_boarders(dest_lr, dest_ud))
				continue;
			board_tile* next = &board[dest_lr][dest_ud];
			if (contains_jump(cur_move, *next, tile) || get_tile_color(*next) != EMPTY)
				continue; //already ate this tile on this move..
			board_tile* mid = &board[(tile.char_indexer) + lr_direction][tile.int_indexer + ud_direction];
			char c = get_tile_color(*mid);
			if ((c == BLACK && color == WHITE) || (c == WHITE && color == BLACK))/*the next tile belongs to the oponnents*/
			{
				game_move* cur_move_copy = copy_move(cur_move);
				if (should_terminate)
				{
					free_list(cur_move->jumps);
					free(cur_move);
					return;
				}

				/*add cur eat to move*/
				list_add(&cur_move_copy->jumps, next);
				if (should_terminate)
				{
					free_list(cur_move->jumps);
					free(cur_move);
					free_list(cur_move_copy->jumps);
					free(cur_move_copy);
					return;
				}
				generate_eater_moves(*next, color, best_moves, num_eats, cur_move_copy);
				if (should_terminate)
				{
					free_list(cur_move->jumps);
					free(cur_move);
					free_list(cur_move_copy->jumps);
					free(cur_move_copy);
					return;
				}
			}
		}
	}
	if (old_eats == *num_eats && cur_move->jumps.len != 0)
	{
		if (cur_move->jumps.len == *num_eats)
		{
			/*add the current move to the best moves list*/
			list_add(best_moves, cur_move);
			if (should_terminate)
			{
				free_list(cur_move->jumps);
				free(cur_move);
				return;
			}
		}
		else if (cur_move->jumps.len > *num_eats)
		{
			free_moves(*best_moves);
			*best_moves = new_list();
			if (should_terminate)
			{
				free_list(cur_move->jumps);
				free(cur_move);
				return;
			}
			/*add the current move to the best moves list*/
			list_add(best_moves, cur_move);
			if (should_terminate)
			{
				free_list(cur_move->jumps);
				free(cur_move);
				return;
			}
			*num_eats = cur_move->jumps.len;
		}
		else
		{
			free_list(cur_move->jumps);
			free(cur_move);
		}
	}
}

/*returns 1 if cur_move jumps eats the tile between cur and next*/
int contains_jump(game_move* cur_move, board_tile second, board_tile first)
{
	board_tile cur_tile = cur_move->start;
	node* cur_node = cur_move->jumps.first;
	while (cur_node != NULL && cur_node->data != NULL)
	{
		board_tile next_tile = *((board_tile*)(cur_node->data));
		if ((same_tile(next_tile, second) && same_tile(cur_tile, first)) ||
			(same_tile(next_tile, first) && same_tile(cur_tile, second)))
			return 1;
		cur_node = cur_node->next;
		cur_tile = next_tile;
	}
	return 0;
}

/*checks if two tiles are the same*/
int same_tile(board_tile first, board_tile second)
{
	return (first.char_indexer == second.char_indexer && first.int_indexer == second.int_indexer);
}

/*copies a game move to new memory*/
game_move* copy_move(game_move* cur_move)
{
	game_move* copy = malloc(sizeof(game_move));
	if (copy == NULL)
	{
		should_terminate = 1;
		perror_message("malloc");
		return NULL;
	}
	copy->start = cur_move->start;
	node* cur = cur_move->jumps.first;
	copy->jumps = new_list();
	if (should_terminate)
	{
		free(copy);
		return NULL;
	}
	for (int i = 0; i < cur_move->jumps.len; i++)
	{
		list_add(&copy->jumps, cur->data);
		if (should_terminate)
		{
			free_list(copy->jumps);
			free(copy);
			return NULL;
		}
		cur = cur->next;
	}
	return copy;
}

/*checks if given indices out of counds of board*/
int out_of_boarders(int char_indexer, int int_indexer)
{
	return (char_indexer < 0 || char_indexer > BOARD_SIZE || int_indexer < 0 || int_indexer > BOARD_SIZE);
}

/*
performs a whole move with all steps
removes all opponent pawns eaten
*/
void do_move(board_tile m_board[][BOARD_SIZE], game_move move)
{
	board_tile current = move.start;
	node* next_move = move.jumps.first;
	board_tile next;
	for (int i = 0; i < move.jumps.len; i++)
	{
		next = *((board_tile*)next_move->data);
		do_part_move(m_board, current, next); /*do each jump separately*/
		current = next;
		next_move = next_move->next;
	}
	/*if (DEBUG)
		print_board(m_board);*/
}

/*
performs a single step within a move
removes opponent pawn(if exists) and moves current pawn
*/
void do_part_move(board_tile m_board[][BOARD_SIZE], board_tile start, board_tile end)
{
	char pawn = m_board[start.char_indexer][start.int_indexer].type;
	int start_c = start.char_indexer; 
	int start_r = start.int_indexer;
	int end_c = end.char_indexer;
	int end_r = end.int_indexer;
	int col_d, row_d;
	col_d = start_c < end_c ? 1 : -1; /*check columns direction*/
	row_d = start_r < end_r ? 1 : -1; /*check rows direction*/
	for (; start_c != end_c; start_c += col_d, start_r += row_d)
	{
		m_board[start_c][start_r].type = EMPTY; 
	}
	if (is_changed_to_king(pawn, end))
		m_board[start_c][start_r].type = pawn == WHITE_M ? WHITE_K : BLACK_K;
	else
		m_board[start_c][start_r].type = pawn;
}

/*checks if current move changed man to king*/
int is_changed_to_king(char pawn, board_tile loc)
{
	if (pawn == BLACK_M)
		return loc.int_indexer == 0 ? 1 : 0;
	if (pawn == WHITE_M)
		return loc.int_indexer == 9 ? 1 : 0;
	return 0;
}

/*
determines if current player direction is up 
according to tile and color
*/
int up_direction(int start_c, int end_c, int is_white)
{
	if ((start_c < end_c))
	{
		if (is_white == 1)
			return 1;
		else
			return 0;
	}
	else
	{
		if (is_white == 1)
			return 0;
		else
			return 1;
	}
}

/*
returns a tile's color
*/
char get_tile_color(board_tile b)
{
	if (b.type == WHITE_K || b.type == WHITE_M)
		return WHITE;
	if (b.type == BLACK_K || b.type == BLACK_M)
		return BLACK;
	if (b.type == EMPTY)
		return EMPTY;
	return 0;
}

/*
returns a tile's type
*/
char get_tile_type(board_tile b)
{
	if (b.type == WHITE_K || b.type == BLACK_K)
		return KING;
	if (b.type == WHITE_M || b.type == BLACK_M)
		return MAN;
	if (b.type == EMPTY)
		return EMPTY;
	return 0;
}

/*returns a score for the current board
  win -> 100
  lose -> -100
  */
int score(board_tile board[BOARD_SIZE][BOARD_SIZE], char color)
{
	int black = 0;
	int white = 0;
	char tile;
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			tile = board[i][j].type;
			if (tile == BLACK_K)
				black += 3;
			if (tile == BLACK_M)
				black++;
			if (tile == WHITE_K)
				white += 3;
			if (tile == WHITE_M)
				white++;
		}
	}
	if (color == BLACK)
	{
		if (white == 0)
			return 100;
		if (black == 0)
			return -100;
		return black - white;
	}
	else
	{
		if (white == 0)
			return -100;
		if (black == 0)
			return 100;
		return white - black;
	}
}

void print_line(){
	int i;
	printf("  |");
	for (i = 1; i < BOARD_SIZE*4; i++){
		printf("-");
	}
	printf("|\n");
}

void print_board(board_tile board[BOARD_SIZE][BOARD_SIZE])
{
	int i,j;
	print_line();
	for (j = BOARD_SIZE-1; j >= 0 ; j--)
	{
		printf((j < 9 ? " %d" : "%d"), j+1);
		for (i = 0; i < BOARD_SIZE; i++){
			printf("| %c ", board[i][j].type);
		}
		printf("|\n");
		print_line();
	}
	printf("   ");
	for (j = 0; j < BOARD_SIZE; j++){
		printf(" %c  ", (char)('a' + j));
	}
	printf("\n");
}

void init_board(board_tile board[BOARD_SIZE][BOARD_SIZE]){
	int i,j;
	for (i = 0; i < BOARD_SIZE; i++){
		for (j = 0; j < BOARD_SIZE; j++){
			board[i][j].char_indexer = i;
			board[i][j].int_indexer = j;
			if ((i + j) % 2 == 0){
				if (j <= 3){
					board[i][j].type = WHITE_M;
				}
				else if (j >= 6){
					board[i][j].type = BLACK_M;
				}
				else{
					board[i][j].type = EMPTY;
				}
			}
			else{
				board[i][j].type = EMPTY;
			}
		}
	}
}


linked_list new_list()
{
	linked_list l;
	l.first = malloc(sizeof(node));
	if (l.first == NULL)
	{
		should_terminate = 1;
		perror_message("malloc");
		return l;
	}
	l.first->next = NULL;
	l.first->data = NULL;
	l.last = l.first;
	l.len = 0;
	return l;
}

/*gets a list of game moves, and frees it*/
void free_moves(linked_list list)
{
	node* cur = list.first;
	node* prev = list.first;
	while (cur->next != NULL)
	{
		cur = cur->next;
		free_list(((game_move*)prev->data)->jumps);
		free(prev->data);
		free(prev);
		prev = cur;
	}
	free(prev);
}

/*does not free the data!*/
void free_list(linked_list list)
{
	node* cur = list.first;
	node* prev = list.first;
	while (cur->next != NULL)
	{
		cur = cur->next;
		free(prev);
		prev = cur;
	}
	free(prev);
}

void list_add(linked_list* list, void* data)
{
	(*list).last->data = data;
	(*list).last->next = malloc(sizeof(node));
	if (NULL == (*list).last->next)
	{
		should_terminate = 1;
		perror_message("malloc");
		return;
	}
	(*list).last = (*list).last->next;
	((*list).last)->next = NULL;
	((*list).last)->data = NULL;
	(*list).len++;
}

/*checks if the game move list are the same*/
int game_move_list_cmp(linked_list list1, linked_list list2)
{
	if (list1.len != list2.len)
		return 0;
	if (list1.len == 0)
		return 1;
	node* cur1 = list1.first;
	node* cur2 = list2.first;
	do
	{
		board_tile b1 = *(board_tile*)(cur1->data);
		board_tile b2 = *(board_tile*)(cur2->data);
		if (b1.char_indexer != b2.char_indexer && b1.int_indexer != b2.int_indexer)
			return 0;
		cur1 = cur1->next;
		cur2 = cur2->next;
	} while (cur1->next != NULL);
	return 1;


}

/*searches for the move in possible moves list. return 1 if found. else 0*/
int find_move(linked_list possible_moves, game_move move)
{
	node* cur = possible_moves.first;
	for (int i = 0; i < possible_moves.len; i++)
	{
		game_move cur_move = *((game_move*)cur->data);
		if (game_move_list_cmp(cur_move.jumps, move.jumps))
			return 1;
		cur = cur->next;
	}
	return 0;
}