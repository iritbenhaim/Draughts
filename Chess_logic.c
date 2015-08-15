#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <limits.h>
#include "Chess.h"
#include "Chess_logic.h"
#include "Game_flow.h"
#include "Minimax.h"

/*returns 1 if the move is legal for the player of color "color"*/
int is_legal_move(game_move move, char color)
{
	linked_list all_moves = generate_moves(board, color);
	if (should_terminate)
	{
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
	game_move* chosen_move = NULL;/* = malloc(sizeof(game_move));
	if (chosen_move == NULL)
	{
		should_terminate = 1;
		perror_message("malloc");
		return 1;
	}*/
	int s = run_minimax(board, color, &chosen_move);
	if (s == INT_MIN)
		return 1;
	do_move(board, *chosen_move);
	print_message("Computer: move ");
	print_single_move(*chosen_move);
	print_board(board);
	end_game = get_winner(board, flip_color(user_color));
	if (should_terminate)
	{
		free(chosen_move);
		return -1;
	}
	if (end_game != 0)
	{
		free(chosen_move);
		print_message(user_color == WHITE ? "Black player wins!\n" : "White player wins!\n");
		return 1;
	}
	free(chosen_move);
	return 0;
}

/*returns 0 if the board is empty or if there are discs of only one color
or if there are more than 20 discs of the same color
else returns 1*/
int is_board_init_legal()
{
	int black_king = 0; /*will be 1 when a black king was found*/
	int white_king = 0;
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			if (board[i][j].type = WHITE_K)
			{
				if (board[i][j].color = WHITE)
					++white_king;
				else
					++black_king;
			}
		}
	}
	return (white_king && black_king);
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
			if (cur_player_color != board[i][j].color)
				continue;
			type = board[i][j].type;
			/*if (type == KING)
			{
				generate_king_moves(board[i][j], cur_player_color, &best_moves, &num_eats);
				if (should_terminate)
					break;
			}
			else if (type == MAN)
			{
				generate_man_moves(board[i][j], cur_player_color, &best_moves, &num_eats);
				if (should_terminate)
					break;
			}
			else
				continue;*/
		}
	}
	return best_moves;
}

/*appends to the best_moves list all the legal man moves form the current tile.
if a better move was found, best moves will be freed and replaced.
tile - the tile in which the man is at*/
void generate_man_moves(board_tile tile, char color, linked_list* best_moves, int* num_eats)
{/*
	int direction = color == WHITE ? 1 : -1; //black goes downwards.
	game_move* cur_move = malloc(sizeof(game_move));
	if (cur_move == NULL)
	{
		should_terminate = 1;
		perror_message("malloc");
		return;
	}
	cur_move->start = tile;

	game_move* cur_move_copy = copy_move(cur_move);
	if (should_terminate)
	{
		free(cur_move);
		return;
	}
	generate_eater_moves(tile, color, best_moves, num_eats, cur_move_copy);
	if (should_terminate)
	{
		free(cur_move);
		return;
	}
	if (0 == *num_eats) /*no eats yet * /
	{
		for (int i = 1; i > -2; i -= 2) /*when i=1, move right. when i=0 move left* /
		{
			if (out_of_boarders((tile.char_indexer) + i, tile.int_indexer + direction))
				continue;
			board_tile* next = &board[(tile.char_indexer) + i][tile.int_indexer + direction];
			if (next->type == EMPTY)
			{
				/*add the move to the best moves list* /
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
				/*malloc the next move* /
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
	free(cur_move);*/
}

/*appends to the best_moves list all the legal king moves form the current tile.
if a better move was found, best moves will be freed and replaced.
tile - the tile in which the king is at*/
void generate_king_moves_old(board_tile tile, char color, linked_list* best_moves, int* num_eats)
{/*
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

	for (int ud_direction = 1; ud_direction > -2; ud_direction -= 2) /*when ud_direction=1, move up. when ud_direction=-1 move down* /
	{
		for (int lr_direction = 1; lr_direction > -2; lr_direction -= 2) /*when lr_direction=1, move right. when lr_direction=-1 move left* /
		{
			for (int i = 1; !out_of_boarders(tile.char_indexer + lr_direction*i, tile.int_indexer + ud_direction*i); ++i)/*check me!!!* /
			{
				if (out_of_boarders((tile.char_indexer) + lr_direction*i, tile.int_indexer + ud_direction*i))
					continue;
				char tile_color = (board[tile.char_indexer + lr_direction*i][tile.int_indexer + ud_direction*i].color);

				if (EMPTY == tile_color)
				{
					if (*num_eats != 0)
						continue;
					board_tile* next = &board[(tile.char_indexer) + lr_direction*i][tile.int_indexer + ud_direction*i];
					/*add the move to the best moves list* /
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
					/*malloc the next move* /
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
				{/*try eating the oponnent. then stop this direction* /
					if (out_of_boarders(tile.char_indexer + lr_direction*(i + 1), tile.int_indexer + ud_direction*(i + 1)))
						break;
					board_tile* next = &board[(tile.char_indexer) + lr_direction*(i + 1)][tile.int_indexer + ud_direction*(i + 1)];
					if (EMPTY != (*next).type)
						break;
					/*add cur eat to move* /
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
				else /*the tile contains a man of your own color. stop this direction.* /
					break;
			}
		}
	}
	free_list(cur_move->jumps);
	free(cur_move);*/
}

/*appends to the best_moves list all the legal eater moves (man or ling eating moves) from the current tile.
if a better move was found, best moves will be freed and replaced.
tile - the tile in which the piece is at*/

void generate_eater_moves(board_tile tile, char color, linked_list* best_moves, int* num_eats, game_move* cur_move)
{/*
	int old_eats = *num_eats;
	/*if (!(((cur_move->start).type == MAN) && tile.int_indexer == (color == BLACK ? 0 : 9)))* /
	{
		for (int ud_direction = 1; ud_direction > -2; ud_direction -= 2) /*when ud_direction=1, move up. when ud_direction=-1 move down* /
		{
			for (int lr_direction = 1; lr_direction > -2; lr_direction -= 2) /*when lr_direction=1, move right. when lr_direction=-1 move left* /
			{
				int dest_lr = (tile.char_indexer) + lr_direction * 2;
				int dest_ud = tile.int_indexer + ud_direction * 2;
				if (out_of_boarders(dest_lr, dest_ud))
					continue;
				board_tile* next = &board[dest_lr][dest_ud];
				if (contains_jump(cur_move, *next, tile) || (*next).color != EMPTY)
					continue; //already ate this tile on this move..
				board_tile* mid = &board[(tile.char_indexer) + lr_direction][tile.int_indexer + ud_direction];
				char c = (*mid).color;
				if ((c == BLACK && color == WHITE) || (c == WHITE && color == BLACK))/*the next tile belongs to the oponnents* /
				{
					game_move* cur_move_copy = copy_move(cur_move);
					if (should_terminate)
					{
						free(cur_move);
						return;
					}

					/*add cur eat to move* /
					list_add(&cur_move_copy->jumps, next);
					if (should_terminate)
					{
						free(cur_move);
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
	}
	if (old_eats == *num_eats && cur_move->jumps.len != 0)
	{
		if (cur_move->jumps.len == *num_eats)
		{
			/*add the current move to the best moves list* /
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
			/*add the current move to the best moves list* /
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
	else
	{
		free_list(cur_move->jumps);
		free(cur_move);
	}*/
}

/*returns 1 if cur_move jumps eats the tile between cur and next*/
int contains_jump(game_move* cur_move, board_tile second, board_tile first)
{
	return 0;/*
	board_tile cur_tile = cur_move->start;
	node* cur_node = cur_move->jumps.first;
	/*if ((cur_move->start).type == KING)* /
	{
		board_tile next_tile = *((board_tile*)(cur_node->data));
		int mid_char = cur_tile.char_indexer > next_tile.char_indexer ? next_tile.char_indexer + 2 : next_tile.char_indexer - 2;
		int mid_int = cur_tile.int_indexer > next_tile.int_indexer ? next_tile.int_indexer + 2 : next_tile.int_indexer - 2;
		board_tile start_eat = board[mid_char][mid_int];
		if ((same_tile(next_tile, second) && same_tile(start_eat, first)) ||
			(same_tile(next_tile, first) && same_tile(start_eat, second)))
		{
			return 1;
		}
	}
	while (cur_node != NULL && cur_node->data != NULL)
	{
		board_tile next_tile = *((board_tile*)(cur_node->data));
		if ((same_tile(next_tile, second) && same_tile(cur_tile, first)) ||
			(same_tile(next_tile, first) && same_tile(cur_tile, second)))
			return 1;
		cur_node = cur_node->next;
		cur_tile = next_tile;
	}
	return 0;*/
}

/*
performs a whole move with all steps
removes all opponent pawns eaten
*/

void do_move(board_tile m_board[][BOARD_SIZE], game_move move)
{/*
	board_tile current = move.start;
	node* next_move = move.jumps.first;
	board_tile next;
	for (int i = 0; i < move.jumps.len; i++)
	{
		next = *((board_tile*)next_move->data);
		do_part_move(m_board, current, next); /*do each jump separately* /
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
	//char pawn = m_board[start.char_indexer][start.int_indexer].color;
	//int start_c = start.char_indexer; 
	//int start_r = start.int_indexer;
	//int end_c = end.char_indexer;
	//int end_r = end.int_indexer;
	//int col_d, row_d;
	//col_d = start_c < end_c ? 1 : -1; /*check columns direction*/
	//row_d = start_r < end_r ? 1 : -1; /*check rows direction*/
	//for (; start_c != end_c; start_c += col_d, start_r += row_d)
	//{
	//	m_board[start_c][start_r].type = EMPTY; 
	//}
	//if (is_changed_to_king(pawn, end))
	//	m_board[start_c][start_r].type = pawn == WHITE_P ? WHITE_K : BLACK_K;
	//else
	//	m_board[start_c][start_r].type = pawn;
}

/*checks if current move changed man to king*/
int is_changed_to_king(char pawn, board_tile loc)
{
	if (pawn == BLACK_P)
		return loc.int_indexer == 0 ? 1 : 0;
	if (pawn == WHITE_P)
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


/*returns a score for the current board
  win -> 100
  lose -> -100
  */
int score(board_tile board[BOARD_SIZE][BOARD_SIZE], char color)
{
	int black = 0;
	int white = 0;
	/*char tile;*/
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			if (board[i][j].color == EMPTY)
				continue;
			/*if (tile == BLACK_K)
				black += 3;
			if (tile == BLACK_P)
				black++;
			if (tile == WHITE_K)
				white += 3;
			if (tile == WHITE_P)
				white++;*/
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


void init_board(board_tile board[BOARD_SIZE][BOARD_SIZE]){
	int i,j;
	for (i = 0; i < BOARD_SIZE; i++){
		for (j = 0; j < BOARD_SIZE; j++){
			if (1 == j || 6 == j) {
				board[i][j].type = WHITE_P;
				board[i][j].color = j == 1 ? WHITE : BLACK;
			}
			else if (j == 0)
				board[i][j].color = WHITE;
			else if (j == BOARD_SIZE-1)
				board[i][j].color = BLACK;
			else{
				board[i][j].type = EMPTY;
				board[i][j].color = EMPTY;
			}
		}
	}
	board[0][0].type = WHITE_R;
	board[1][0].type = WHITE_N;
	board[2][0].type = WHITE_B;
	board[3][0].type = WHITE_Q;
	board[4][0].type = WHITE_K;
	board[5][0].type = WHITE_B;
	board[6][0].type = WHITE_N;
	board[7][0].type = WHITE_R;
	board[0][BOARD_SIZE - 1].type = WHITE_R;
	board[1][BOARD_SIZE - 1].type = WHITE_N;
	board[2][BOARD_SIZE - 1].type = WHITE_B;
	board[3][BOARD_SIZE - 1].type = WHITE_Q;
	board[4][BOARD_SIZE - 1].type = WHITE_K;
	board[5][BOARD_SIZE - 1].type = WHITE_B;
	board[6][BOARD_SIZE - 1].type = WHITE_N;
	board[7][BOARD_SIZE - 1].type = WHITE_R;

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
		if (b1.char_indexer != b2.char_indexer || b1.int_indexer != b2.int_indexer)
			return 0;
		cur1 = cur1->next;
		cur2 = cur2->next;
	} while (cur1->next != NULL);
	return 1;


}

/*searches for the move in possible moves list. return 1 if found. else 0*/
int find_move(linked_list possible_moves, game_move move)
{/*
	node* cur = possible_moves.first;
	for (int i = 0; i < possible_moves.len; i++)
	{
		game_move cur_move = *((game_move*)cur->data);
		if (game_move_list_cmp(cur_move.jumps, move.jumps))
			return 1;
		cur = cur->next;
	}
	return 0;*/
}





/*return legal moves for a king*/
void generate_king_moves(board_tile tile, linked_list* moves)
{
	char color = tile.color;
	game_move* cur_move = malloc(sizeof(game_move));
	if (cur_move == NULL)
	{
		should_terminate = 1;
		perror_message("malloc");
		return;
	}
	cur_move->start = tile;

	for (int r = -1; r <= 1; r++)
	{
		for (int c = -1; c <= 1; c++)
		{
			if (out_of_boarders(tile.char_indexer + c, tile.int_indexer + r))
				continue;
			if (board[r][c].color == color)
				continue;
			if (r == 0 && c == 0)
				continue;
			board_tile next = board[(tile.char_indexer) + c][tile.int_indexer + r];
			cur_move->end = next;
			list_add(moves, cur_move);
			if (should_terminate)
			{
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
			cur_move->start = tile;
		}
	}
	free(cur_move);
}

/*returns legal moves for a knight out of 8 possible*/
void generate_knight_moves(board_tile tile, linked_list* moves)
{
	char color = tile.color;
	int indxs[4] = { -2, -1, 1, 2 };
	game_move* cur_move = malloc(sizeof(game_move));
	if (cur_move == NULL)
	{
		should_terminate = 1;
		perror_message("malloc");
		return;
	}
	cur_move->start = tile;

	for (int r = 0; r < 4; r++)
	{
		for (int c = 0; c < 4; c++)
		{
			if (out_of_boarders(tile.char_indexer + c, tile.int_indexer + r))
				continue;
			if (abs(indxs[r]) == abs(indxs[c]))
				continue;
			if (board[tile.char_indexer + c][tile.int_indexer + r].color == color)
				continue;
			board_tile next = board[(tile.char_indexer) + c][tile.int_indexer + r];
			cur_move->end = next;

			list_add(moves, cur_move);
			if (should_terminate)
			{
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
			cur_move->start = tile;
		}
	}
	free(cur_move);
}

/*returns a single move forward*/
void generate_pawn_moves(board_tile tile, linked_list* moves)
{
	/*TODO - pawn promotion*/
	char color = tile.color;
	int r;
	game_move* cur_move = malloc(sizeof(game_move));
	if (cur_move == NULL)
	{
		should_terminate = 1;
		perror_message("malloc");
		return;
	}
	cur_move->start = tile;
	if (color == WHITE)
		r = 1;
	else
		r = -1;
	for (int c = -1; c <= 1; c++)
	{
		if (out_of_boarders(tile.char_indexer + c, tile.int_indexer + r))
			continue;
		if (board[tile.char_indexer + c][tile.int_indexer + r].color == color)
			continue;
		if (board[tile.char_indexer + c][tile.int_indexer + r].color == flip_color(color) && c == 0)
			continue;
		if (board[tile.char_indexer + c][tile.int_indexer + r].color == EMPTY && abs(c) == 1)
			continue;
		board_tile next = board[tile.char_indexer + c][tile.int_indexer + r];
		cur_move->end = next;

		list_add(moves, cur_move);
		if (should_terminate)
		{
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
		cur_move->start = tile;
	}
}

/*queen moves combines possible moves for rook and bishop*/
void generate_queen_moves(board_tile tile, linked_list* moves)
{
	generate_rook_moves(tile, moves);
	generate_bishop_moves(tile, moves);
}

/*returns all legal bishop moves along the diagonals*/
void generate_bishop_moves(board_tile tile, linked_list* moves)
{
	get_direct_bishop_moves(tile, moves, 1, 1);
	get_direct_bishop_moves(tile, moves, 1, 0);
	get_direct_bishop_moves(tile, moves, 0, 1);
	get_direct_bishop_moves(tile, moves, 0, 0);
}

/*return all legal rook moves along row and column*/
void generate_rook_moves(board_tile tile, linked_list* moves)
{
	get_direct_rook_moves(tile, moves, 1, 1);
	get_direct_rook_moves(tile, moves, 0, 1);
	get_direct_rook_moves(tile, moves, 1, 0);
	get_direct_rook_moves(tile, moves, 0, 0);
}

void generate_castling_move(board_tile tile,)

/*this function adds rook moves for a specific direction of four directions:
  up, down, left, right - reflected in col and neg variables*/
void get_direct_rook_moves(board_tile tile, linked_list* moves, int col, int neg)
{
	if (should_terminate)
		return;
	char color = tile.color;
	int r = tile.int_indexer;
	int c = tile.char_indexer;
	game_move* cur_move = malloc(sizeof(game_move));
	if (cur_move == NULL)
	{
		should_terminate = 1;
		perror_message("malloc");
		return;
	}
	cur_move->start = tile;
	for (int i = 1; i < BOARD_SIZE; i++)
	{
		if (neg) /*advance indexes according to directionality*/
		{
			c = c - 1 ? col : c;
			r = r - 1 ? !col : r;
		}
		else
		{
			c = c + 1 ? col : c;
			r = r + 1 ? !col : r;
		}
		if (out_of_boarders(c, r))
			break;
		if (board[c][r].color == color)
			break;
		board_tile next = board[c][r];
		cur_move->end = next;
		list_add(moves, cur_move);
		if (should_terminate)
		{
			free(cur_move);
			return;
		}
		if (next.color == flip_color(color)) /*eat move - no more moves to this direction*/
			break;
		/*malloc the next move*/
		cur_move = malloc(sizeof(game_move));
		if (cur_move == NULL)
		{
			should_terminate = 1;
			perror_message("malloc");
			return;
		}
		cur_move->start = tile;
	}
	free(cur_move);
}

/*this function adds bishop moves for a specific firection of four directions
  reflected in lft and up variables*/
void get_direct_bishop_moves(board_tile tile, linked_list* moves, int lft, int up)
{
	if (should_terminate)
		return;
	char color = tile.color;
	int r = tile.int_indexer;
	int c = tile.char_indexer;
	game_move* cur_move = malloc(sizeof(game_move));
	if (cur_move == NULL)
	{
		should_terminate = 1;
		perror_message("malloc");
		return;
	}
	cur_move->start = tile;
	for (int i = 1; i < BOARD_SIZE; i++)
	{
		c = c + 1 ? !lft : c - 1; /*advance indexes according to directionality*/
		r = r + 1 ? up : r - 1;
		if (out_of_boarders(c, r))
			break;
		if (board[c][r].color == color)
			break;
		board_tile next = board[c][r];
		cur_move->end = next;
		list_add(moves, cur_move);
		if (should_terminate)
		{
			free(cur_move);
			return;
		}
		if (next.color == flip_color(color)) /*eat move - no more moves to this direction*/
			break;
		/*malloc the next move*/
		cur_move = malloc(sizeof(game_move));
		if (cur_move == NULL)
		{
			should_terminate = 1;
			perror_message("malloc");
			return;
		}
		cur_move->start = tile;
	}
	free(cur_move);
}