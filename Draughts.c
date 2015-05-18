#define _CRT_SECURE_NO_WARNINGS

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
struct game_move
{
	game_move* next;
	board_tile data;
};


struct scoring_board
{
	char** board;
	char player;
};


int minmax_depth = 1; /*the debth of minmax algorithm*/
char* user_color = "white"; /*color of the user player*/
board_tile board[BOARD_SIZE][BOARD_SIZE]; /*game board*/


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
	print_board(board);


	print_message(WELCOME_TO_DRAUGHTS);
	print_message(ENTER_SETTINGS);
	while (1)
	{ /*game settings*/
		if (read_user_input_line(input, &input_size) == -1)
		{
			return -1; /*no resources were allocated yet*/
		}
		settings(input);
		
		
	}
	init_board(board);
	print_board(board);
	print_message(WRONG_MINIMAX_DEPTH);
	free(input);
	getchar();
	return 0;
}


void settings(char* input)
{
	int temp, i, j;
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
		return;
	}
	if (0 == cmp_input_command(input, "user_color "))
	{
		if (0 == cmp_input_command(input + strlen("user_color "), "white"))
			user_color = "white";
		else if (0 == cmp_input_command(input + strlen("user_color "), "black"))
			user_color = "black";
		return;
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
		return;
	}
	if (0 == cmp_input_command(input, "rm"))
	{
		i = strchr(input, '<') - input;
		j = strchr(input, ',') - input;
		if (j - i != 2 || input[i + 1] < 'a' || input[i + 1] > 'j')
		{
			print_message(WRONG_POSITION);
			return;
		}
		*strchr(input, '>') = '\0';
		temp = atoi(input + j + 1);
		if (temp < 1 || temp > 10)
		{
			print_message(WRONG_POSITION);
			return;
		}
		board[temp - 1][input[i + 1] - 'a'].type = EMPTY;/*check me!!!*/
		print_board(board);
		return;

	}
	if (0 == cmp_input_command(input, "set"))
	{

		return;
	}
	if (0 == cmp_input_command(input, "print"))
	{
		return;

	}
	if (0 == cmp_input_command(input, "quit"))
	{

		return;
	}
	if (0 == cmp_input_command(input, "start"))
	{

		return;
	}
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

