#include "Game_flow.h"
#include "Chess_logic.h"
#include "Minimax.h"
#include <stdlib.h>
#include <string.h>

board_tile board[BOARD_SIZE][BOARD_SIZE]; /*game board*/
int should_terminate = 0;
char user_color = WHITE; /*color of the user player*/
int is_user_turn;
int player_vs_player = 1; /*1 - player vs player mode. 2 - player vs comp. 0 (for debug only) - comp vs comp*/

int main()
{
	int input_size = 1024;
	char* input = malloc(input_size);
	if (input == NULL)
	{
		should_terminate = 1;
		perror_message("malloc");
		return -1;
	}
	init_board(board);
	print_board(board);
	while (1)
	{ /*game settings*/
		print_message(ENTER_SETTINGS);
		if (read_user_input_line(input, &input_size) == -1)
		{
			return -1; /*no resources were allocated yet*/
		}
		if (settings(input))
			break;
		if (should_terminate)
		{
			free(input);
			return -1;
		}
	}
	is_user_turn = user_color == WHITE;
	while (1)
	{/*game play*/
		if (!COMP_ONLY)
		{
			while (is_user_turn)
			{
				/*if (is_user_turn)
					print_message(ENTER_YOUR_MOVE);*/
				if (read_user_input_line(input, &input_size) == -1)
				{
					return -1; /*no resources were allocated yet*/
				}
				int is_comp_turn = user_move(input);
				if (should_terminate)
				{
					free(input);
					return -1;
				}
				if (is_comp_turn == -1)
				{
					if (DEBUG)
					{
						getchar();
					}
					free(input);
					return -1;
				}
				is_user_turn = !is_comp_turn;
			}
		}
		/*computer turn*/

		if (COMP_ONLY)
		{
			if (1 == do_computer_move((user_color)))
			{
				break;

				if (DEBUG)
				{
					getchar();
				}
			}
		}
		if (1 == do_computer_move(flip_color(user_color)))
		{
			break;

			if (DEBUG)
			{
				getchar();
			}
		}
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
		if (i >= *input_size - 1)
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
		if (move.start.color != user_color)
		{
			print_message(NO_PIECE);
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

/*return 0 if input ==  command*/
int cmp_input_command(char* input, char* cmd)
{
	if (strlen(input) >= strlen(cmd))
	{
		return memcmp(input, cmd, strlen(cmd));
	}
	return -1;
}



/*runs the game settings phase of the game on a given command.
returns 1 if game start command was sent. else returns 0*/
int settings(char* input)
{
	int temp, i, j;
	char* input_copy;
	if (0 == cmp_input_command(input, "game_mode "))
	{
		input += strlen("game_mode ");
		if ("1\n" == input)
		{
			print_message(TWO_PLAYERS_GAME_MODE);
			player_vs_player = 1;
		}
		else if ("2\n" == input)
		{
			print_message(PLAYER_VS_AI_GAME_MODE);
			player_vs_player = 2;
		}
		else if (DEBUG && "0\n" == input)
			player_vs_player = 0;
		else
			print_message(WRONG_GAME_MODE);
	}
	if (0 == cmp_input_command(input, "difficulty "))
	{
		if ()
		temp = atoi(input + strlen("minimax_depth "));
		if (temp < 1 || temp > 6)
		{
			print_message(WRONG_MINIMAX_DEPTH)
		}
		else
		{
			minimax_depth = temp;
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
				board[i][j].type2 = EMPTY;
				board[i][j].color = EMPTY;
			}
		}
		return 0;
	}
	if (0 == cmp_input_command(input, "rm"))
	{
		if (0 == get_board_position(input, &i, &j))
			return 0;
		board[i][j].type2 = EMPTY;
		board[i][j].color = EMPTY;
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
		board[i][j].type2 = type;
		board[i][j].color = color;
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
