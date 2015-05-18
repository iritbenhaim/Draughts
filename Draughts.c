#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Draughts.h"

struct board_tile
{
	int row;
	int col;
	char type;
};
struct game_move
{
	game_move* next;
	board_tile data;
};


int main()
{
	unsigned int input_size = 1024;
	int m_minmax_debth = 1;
	char* input = malloc(input_size);
	if (input == NULL)
	{
		perror_message("main");
		return -1;
	}


	board_tile board[BOARD_SIZE][BOARD_SIZE];
	print_message(WELCOME_TO_DRAUGHTS);
	print_message(ENTER_SETTINGS);
	while (1)
	{ /*game settings*/
		if (read_user_input_line(input, &input_size) == -1)
		{
			return -1; /*no resources were allocated yet*/
		}
		if (cmp_input_command(input, "minimax_depth"))
		{

		}
		if (cmp_input_command(input, "user_color"))
		{

		}
		if (cmp_input_command(input, "clear"))
		{

		}
		if (cmp_input_command(input, "rm"))
		{

		}
		if (cmp_input_command(input, "set"))
		{

		}
		if (cmp_input_command(input, "print"))
		{

		}
		if (cmp_input_command(input, "quit"))
		{

		}
		if (cmp_input_command(input, "start"))
		{

		}
		
	}
	init_board(board);
	print_board(board);
	print_message(WRONG_MINIMAX_DEPTH);
	getchar();
	return 0;
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
	while (c != '\n') /*read string from user. the string length can be varied*/
	{
		c = getchar();
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
		input[i++] = c;
	}
	input[i] = '\0';
	return 1;
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

