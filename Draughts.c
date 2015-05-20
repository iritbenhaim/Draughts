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

struct game_move
{
	board_tile start;
	int jumps_len;
	node* jumps;
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

game_move generate_moves(board_tile** board)
{
	int i, j;
	node* best_moves = NULL;
	node* temp_moves = NULL;
	game_move chosen_move;
	int temp_num_eats, num_eats = 0;
	char type, color;
	for (i = 0; i < BOARD_SIZE; ++i)
	{
		for (j = 0; j < BOARD_SIZE; j++)
		{
			type = get_tile_type(board[i][j]);
			color = get_tile_color(board[i][j]);
			if (type == KING)
			{
				temp_moves = generate_king_moves(board[i][j], color);
			}
			else if (type == MAN)
			{
				temp_moves = generate_man_moves(board[i][j], color);
			}
			else
				continue;
			temp_num_eats = ((game_move*)temp_moves->data)->jumps_len;
			if (temp_num_eats < num_eats)
			{ /*new move doesn't have enough eats*/
				free_moves(temp_moves);
			}
			else if (temp_num_eats > num_eats)
			{ /*old move doesn't have enough eats replace it*/
				free_moves(best_moves);
				num_eats = temp_num_eats;
				best_moves = temp_moves;
			}
			else
			{ /*join lists*/
				(get_last(best_moves))->next = temp_moves;
			}
		}
	}
	chosen_move = *((game_move*)best_moves->data);
	free_moves(best_moves);
	return chosen_move;
}

node* generate_man_moves(board_tile tile, char color)
{
	return NULL;
}

node* generate_king_moves(board_tile tile, char color)
{
	return NULL;
}
node* get_last(node* list)
{
	while (list->next != NULL)
	{
		list = list->next;
	}
	return list;
}

void free_moves(node* list)
{
	if (list == NULL)
		return;
	node* prev = list;
	while (list->next != NULL)
	{
		list = list->next;
		free_list(((game_move*)prev->data)->jumps);
		free(prev);
		prev = list;
	}
	free_list(((game_move*)prev->data)->jumps->next);
	free(prev);
}

void free_list(node* list)
{
	if (list == NULL)
		return;
	node* prev = list;
	while (list->next != NULL)
	{
		list = list->next;
		free(prev);
		prev = list;
	}
	free(prev);
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

