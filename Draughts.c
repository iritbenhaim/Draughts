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

struct scoring_board
{
	char** board;
	char player;
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
	while (1)
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
	while (1)
	{/*game play*/

	}
	print_message(WRONG_MINIMAX_DEPTH);
	free(input);
	if (DEBUG)
		getchar();
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
/*
int minimax(scoring_board board, int depth, int maximize, game_move* best)
{
	int tmp_val;
	int best_val;
	game_move possible;
	possible = generate_moves(board);
	if (should_terminate)
	{//DO STUFF!}
	tmp_val = score(board);
	if (depth == 0 || tmp_val == 100 || tmp_val == -100)
		return tmp_val;
	if(maximize)
	{
		best_val = INT_MIN;
		while (possible != NULL)
		{
			tmp_val = minimax(do_move(board, possible), depth - 1, 0);
			if (tmp_val > best_val)
			{
				best_val = tmp_val;
				*best = possible;
			}
		}
		return best_val;
	}
	else
	{
		best_val = INT_MAX;
		while (possible != NULL)
		{
			tmp_val = minimax(do_move(board, possible), depth - 1, 1);
			if (tmp_val < best_val)
			{
				best_val = tmp_val;
				*best = possible;
			}
		}
		return best_val;
	}
}
*/

/*cur_player_color - the color of the current player*/
game_move generate_moves(board_tile** board, char cur_player_color)
{
	int i, j;
	linked_list best_moves;
	game_move chosen_move;
	int num_eats = 0;
	char type;

	chosen_move.jumps.first = NULL;
	best_moves = new_list();
	if (should_terminate)
		return chosen_move;
	for (i = 0; i < BOARD_SIZE; ++i)
	{
		for (j = 0; j < BOARD_SIZE; j++)
		{
			if (cur_player_color != get_tile_color(board[i][j]));
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
	chosen_move = *((game_move*)best_moves.first);
	free_moves(best_moves);
	return chosen_move;
}

/*tile - the tile in which the man is at*/
void generate_man_moves(board_tile tile, char color, linked_list* best_moves, int* num_eats)
{
	int direction = color == 'w' ? 1 : -1; //black goes downwards.
	int i;
	int max_eats = 0;
	board_tile* next_tile = malloc(sizeof(board_tile));
	if (next_tile == NULL)
	{
		should_terminate = 1;
		return;
	}
	game_move* cur_move = malloc(sizeof(game_move));
	if (NULL == cur_move)
	{
		should_terminate = 1;
		free(next_tile);
		return;
	}
	cur_move->jumps = new_list();
	if (should_terminate)
	{
		free(next_tile);
		free(cur_move);
		return;
	}
	for (i = 1; i > -2; i-=2) /*when i=1, move right. when i=0 move left*/
	{
		char c = get_tile_color(board[(tile.first_indexer) + i][tile.second_indexer + direction]);
		if (0 == max_eats && c == EMPTY)/*check me!!!!!!!!!*/
		{
			list_add(cur_move->jumps, &board[tile.first_indexer + i][tile.second_indexer + direction]);
		}
		else if ((c == BLACK && color == WHITE) || (c == WHITE && color == BLACK))
		{

		}

	}
}

void generate_king_moves(board_tile tile, char color, linked_list* best_moves, int* num_eats)
{

}

/*
performs a whole move with all steps
removes all opponent pawns eaten
*/
void do_move(board_tile** board, game_move move)
{
	board_tile current = move.start;
	node* next_move = move.jumps.first;
	board_tile next = *((board_tile*)next_move->data);

	for (int i = 0; i < move.jumps.len; i++)
	{
		do_part_move(board, current, next);
		current = next;
		next_move = next_move->next;
		next = *((board_tile*)next_move->data);
	}
}

/*
performs a single step within a move
removes opponent pawn(if exists) and moves current pawn
*/
void do_part_move(board_tile** board, board_tile start, board_tile end)
{
	int start_c, start_r, end_c, end_r;
	char pawn = board[start.first_indexer][start.second_indexer].type;
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
			board[start_r][start_c].type = EMPTY;
	board[end_r][end_c].type = pawn;
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
int score(scoring_board board)
{
	int black = 0;
	int white = 0;
	char tile;
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; i < 10; j++)
		{
			tile = board.board[i][j];
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
	if (board.player == BLACK)
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
	l.last = l.first;
	l.first->next = NULL;
	l.first->data = NULL;
	l.len = 0;
	return l;
}

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

void list_add(linked_list list, void* data)
{
	list.last->next = malloc(sizeof(node));
	if (NULL == list.last->next)
	{
		should_terminate = 1;
		return;
	}
	list.last = list.last->next;
	(list.last)->data = data;
	(list.last)->next = NULL;
	list.len++;
}