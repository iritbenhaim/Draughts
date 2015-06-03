#define _CRT_SECURE_NO_WARNINGS
#define DEBUG 1

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Draughts.h"

struct board_tile
{
	int first_indexer; /*row num?*/
	int second_indexer; /*col_num?*/
	char type;
};

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

struct game_move
{
	board_tile start;
	linked_list jumps;
};


int minmax_depth = 1; /*the debth of minmax algorithm*/
char* user_color = "white"; /*color of the user player*/
board_tile board[BOARD_SIZE][BOARD_SIZE]; /*game board*/
int should_terminate = 0;

int main()
{
	unsigned int input_size = 1024;
	char* input = malloc(input_size);
	if (input == NULL)
	{
		perror_message("main");
		return -1;
	}
	init_board(board);
	if (DEBUG)
		print_board(board);


	print_message(WELCOME_TO_DRAUGHTS);
	print_message(ENTER_SETTINGS);
	while (1 && !DEBUG)
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
	int is_user_turn = user_color == "white";
	while (1)
	{/*game play*/
		while (is_user_turn)
		{
			print_message(ENTER_YOUR_MOVE);
			if (read_user_input_line(input, &input_size) == -1)
			{
				return -1; /*no resources were allocated yet*/
			}
			if (user_move(input))
				break;
			if (should_terminate)
			{
				free(input);
				return;
			}
			is_user_turn = 0;
		}
		/*computer turn*/
		{
			if (do_computer_move(flip_color(user_color)))
				break;
			is_user_turn = 1;
			if (DEBUG)
				print_board(board);
		}

	}
	free(input);
	if (DEBUG)
	{
		getchar();
		print_message(WRONG_MINIMAX_DEPTH);
	}
	return 0;
}

int user_move(char* input)
{
	return 0;
}

/*return 1 if game ended*/
int do_computer_move(char color)
{
	game_move* chosen_move = malloc(sizeof(game_move));
	if (chosen_move == NULL)
	{
		should_terminate = 1;
		return 1;
	}
	int s = minimax(board, minmax_depth, 1, chosen_move, color);
	if (s == INT_MIN)
		return 1;
	do_move(board, *chosen_move);
	free_list(chosen_move->jumps);
	free(chosen_move);
	return 0;
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
			user_color = "white";
		else if (0 == cmp_input_command(input, "black"))
			user_color = "black";
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
		get_board_position(input, &i, &j);
		board[i][j].type = EMPTY;
		return 0;

	}
	if (0 == cmp_input_command(input, "set"))
	{
		char* color;
		char type;
		input_copy = strchr(input, '>') + 2;
		get_board_position(input, &i, &j);
		while (input_copy[0] == ' ')
			++input_copy;
		if (0 == cmp_input_command(input_copy, "white"))
			color = "white";
		else if (0 == cmp_input_command(input_copy, "black"))
			color = "black";
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


/*returns the char used on board to represent the given type and color of tool*/
char get_tool_type(char* color, char type)
{
	if (color[0] == 'w')
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
			perror_message("main");
			return -1;
		}
		c = ch;
		if (i >= *input_size-1)
		{
			*input_size *= 2;
			input = realloc(input, *input_size);
			if (input == NULL)
			{
				perror_message("main");
				return -1;
			}
		}
		if (c == '\n')
			c = '\0';
		input[i++] = c;
	}
	return 1;
}

int minimax(board_tile board[BOARD_SIZE][BOARD_SIZE], int depth, int maximize, game_move* best, char color)
{
	int tmp_val;
	int best_val;
	linked_list possible;
	possible = generate_moves(board, color);
	if (should_terminate)
	{/*DO STUFF!*/}
	node* crnt = possible.first;
	
	tmp_val = score(board, maximize? color: flip_color(color));
	if (depth == 0 || tmp_val == 100 || tmp_val == -100)
		return tmp_val;
	if(maximize)
	{
		best_val = INT_MIN;
		for (int i = 0; i < possible.len; i++, crnt = crnt->next)
		{
			do_move(board, (*(game_move*)crnt->data));
			tmp_val = minimax(board, depth - 1, 0, best, color);
			if (tmp_val > best_val)
			{
				best_val = tmp_val;
				*best = *(game_move*)crnt->data;
			}
		}
		return best_val;
	}
	else
	{
		best_val = INT_MAX;
		for (int i = 0; i < possible.len; i++, crnt = crnt->next)
		{
			do_move(board, (*(game_move*)crnt->data));
			tmp_val = minimax(board, depth - 1, 1, best, color);
			if (tmp_val < best_val)
			{
				best_val = tmp_val;
				*best = (*(game_move*)crnt->data);
			}
		}
		return best_val;
	}
}

char flip_color(color)
{
	return BLACK == color ? WHITE: BLACK;
}

/*cur_player_color - the color of the current player*/
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

/*tile - the tile in which the man is at*/
void generate_man_moves(board_tile tile, char color, linked_list* best_moves, int* num_eats)
{
	int direction = color == 'w' ? 1 : -1; //black goes downwards.
	game_move* cur_move = malloc(sizeof(game_move));
	if (cur_move == NULL)
	{
		should_terminate = 1;
		return;
	}
	cur_move->jumps = new_list();
	if (should_terminate)
	{
		free(cur_move);
		return;
	}

	generate_eater_moves(tile, color, best_moves, num_eats, cur_move);
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
			if (out_of_boarders((tile.first_indexer) + i, tile.second_indexer + direction))
				continue;
			board_tile* next = &board[(tile.first_indexer) + i][tile.second_indexer + direction];
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

void generate_eater_moves(board_tile tile, char color, linked_list* best_moves, int* num_eats, game_move* cur_move)
{
	int used_cur_move = 0;
	int old_eats = *num_eats;
	for (int ud_direction = 1; ud_direction > -2; ud_direction -= 2) /*when ud_direction=1, move up. when ud_direction=-1 move down*/
	{
		for (int lr_direction = 1; lr_direction > -2; lr_direction -= 2) /*when lr_direction=1, move right. when lr_direction=-1 move left*/
		{
			if (out_of_boarders((tile.first_indexer) + lr_direction * 2, tile.second_indexer + ud_direction * 2))
				continue;
			board_tile* next = &board[(tile.first_indexer) + lr_direction][tile.second_indexer + ud_direction];
			char c = get_tile_color(*next);
			if ((c == BLACK && color == WHITE) || (c == WHITE && color == BLACK))/*the next tile belongs to the oponnents*/
			{
				game_move* cur_move_copy = copy_move(cur_move);
				if (should_terminate)
				{
					free_list(cur_move->jumps);
					free(cur_move);
					return;
				}

				board_tile* next = &board[(tile.first_indexer) + lr_direction * 2][tile.second_indexer + ud_direction * 2];
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
				generate_eater_moves(tile, color, best_moves, num_eats, cur_move_copy);
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
	if (*num_eats == old_eats && old_eats > 0)
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
			++*num_eats;
		}
	}
}

game_move* copy_move(game_move* cur_move)
{
	game_move* copy = malloc(sizeof(game_move));
	if (copy == NULL)
	{
		should_terminate = 1;
		return NULL;
	}
	copy->start = cur_move->start;
	copy->jumps.len = cur_move->jumps.len;
	node* cur = cur_move->jumps.first;
	while (cur != NULL)
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
int out_of_boarders(int first_indexer, int second_indexer)
{
	return (first_indexer < 0 || first_indexer > BOARD_SIZE || second_indexer < 0 || second_indexer > BOARD_SIZE);
}
void generate_king_moves(board_tile tile, char color, linked_list* best_moves, int* num_eats)
{
	game_move* cur_move = malloc(sizeof(game_move));
	if (cur_move == NULL)
	{
		should_terminate = 1;
		return;
	}
	cur_move->jumps = new_list();
	if (should_terminate)
	{
		free(cur_move);
		return;
	}

	
	for (int ud_direction = 1; ud_direction > -2; ud_direction -= 2) /*when ud_direction=1, move up. when ud_direction=-1 move down*/
	{
		for (int lr_direction = 1; lr_direction > -2; lr_direction -= 2) /*when lr_direction=1, move right. when lr_direction=-1 move left*/
		{
			for (int i = 1; !out_of_boarders(tile.first_indexer + ud_direction*i, tile.second_indexer + lr_direction*i); ++i)/*check me!!!*/
			{
				char tile_color = (get_tile_color(board[tile.first_indexer + ud_direction*i][tile.second_indexer + lr_direction*i]));
				
				if (EMPTY == tile_color)
				{
					if (*num_eats != 0)
						continue;
					board_tile* next = &board[(tile.first_indexer) + lr_direction*i][tile.second_indexer + ud_direction*i];
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
					if (should_terminate)
					{
						free(cur_move);
						return;
					}
				}
				else if (tile_color == flip_color(color))
				{/*try eating the oponnent. then stop this direction*/
					if (out_of_boarders(tile.first_indexer + ud_direction*(i + 1), tile.second_indexer + lr_direction*(i + 1)))
						break;
					board_tile* next = &board[(tile.first_indexer) + lr_direction*(i + 1)][tile.second_indexer + ud_direction*(i + 1)];
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
					generate_eater_moves(tile, color, best_moves, num_eats, cur_move);
					if (should_terminate)
					{
						free_list(cur_move->jumps);
						free(cur_move);
						return;
					}
					/*if one eat only, add it*/
						

					if (*num_eats <= 1)
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
							++*num_eats;
						}
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

/*
performs a whole move with all steps
removes all opponent pawns eaten
*/
void do_move(board_tile m_board[][BOARD_SIZE], game_move move)
{
	board_tile current = move.start;
	node* next_move = move.jumps.first;
	board_tile next = *((board_tile*)next_move->data);

	for (int i = 0; i < move.jumps.len; i++)
	{
		do_part_move(m_board, current, next);
		current = next;
		next_move = next_move->next;
		next = *((board_tile*)next_move->data);
	}
}

/*
performs a single step within a move
removes opponent pawn(if exists) and moves current pawn
*/
void do_part_move(board_tile m_board[][BOARD_SIZE], board_tile start, board_tile end)
{
	int start_c, start_r, end_c, end_r;
	char pawn = m_board[start.first_indexer][start.second_indexer].type;
	if (start.first_indexer < end.first_indexer)
	{
		start_r = start.first_indexer;
		end_r = end.first_indexer;
	}
	else
	{
		start_r = end.first_indexer;
		end_r = start.first_indexer;
	}
	if (start.second_indexer < end.second_indexer)
	{
		start_c = start.second_indexer;
		end_c = end.second_indexer;
	}
	else
	{
		start_c = end.second_indexer;
		end_c = start.second_indexer;
	}
	for (; start_r < end_r; start_r++)
		for (; start_c < end_c; start_c++)
			m_board[start_r][start_c].type = EMPTY;
	m_board[end_r][end_c].type = pawn;
}

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
			board[i][j].first_indexer = i;
			board[i][j].second_indexer = j;
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
	(*list).last->next = malloc(sizeof(node));
	if (NULL == (*list).last->next)
	{
		should_terminate = 1;
		return;
	}
	(*list).last = (*list).last->next;
	((*list).last)->data = data;
	((*list).last)->next = NULL;
	(*list).len++;
}