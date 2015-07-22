#include "Game_flow.h"
#include "Chess_logic.h"
#include "Minimax.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

board_tile board[BOARD_SIZE][BOARD_SIZE]; /*game board*/
int should_terminate = 0;
char user_color = WHITE; /*color of the user player*/
int is_user_turn;
int player_vs_player = 1; /*1 - player vs player mode. 2 - player vs comp. 0 (for debug only) - comp vs comp*/
char next_player = WHITE;

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
		if (player_vs_player = 1)
		{
			print_message(ILLEGAL_COMMAND);
			return 0;
		}
		input += strlen("difficulty ");
		if (0 == cmp_input_command(input, "best"))
		{
			minimax_depth = -1;
			return 0;
		}
		
		if (0 == cmp_input_command(input, "depth "))
		{
			temp = atoi(input + strlen("depth "));
			if (temp < 1 || temp > 4)
			{
				print_message(WRONG_MINIMAX_DEPTH)
			}
			else
			{
				minimax_depth = temp;
			}
			return 0;
		}
		print_message(ILLEGAL_COMMAND);
		return 0;
	}
	if (0 == cmp_input_command(input, "user_color "))
	{
		if (player_vs_player = 1)
		{
			print_message(ILLEGAL_COMMAND);
			return 0;
		}
		input += strlen("user_color ");
		while (input[0] == ' ')
			++input;
		if (0 == cmp_input_command(input, "white"))
			user_color = WHITE;
		else if (0 == cmp_input_command(input, "black"))
			user_color = BLACK;
		return 0;
	}
	if (0 == cmp_input_command(input, "load "))
	{
		input += strlen("load ");
		FILE *setting_file = fopen(input, "rt");
		if (NULL == setting_file)
		{
			print_message(WRONG_FILE_NAME)
			return 0;
		}
		/*TODO - */
		fclose(setting_file);
		print_board(board);
	}	
	if (0 == cmp_input_command(input, "clear"))
	{
		for (i = 0; i < BOARD_SIZE; ++i)
		{
			for (j = 0; j < BOARD_SIZE; ++j)
			{
				board[i][j].type = EMPTY;
				board[i][j].color = EMPTY;
			}
		}
		return 0;
	}
	if (0 == cmp_input_command(input, "next_player "))
	{
		input += strlen("next_player ");
		if (0 == cmp_input_command(input, "white"))
			next_player = WHITE;
		else if (0 == cmp_input_command(input, "black"))
			next_player = BLACK;
	}
	if (0 == cmp_input_command(input, "rm "))
	{
		if (0 == get_board_position(input, &i, &j))
			return 0;
		board[i][j].type = EMPTY;
		board[i][j].color = EMPTY;
		return 0;

	}
	if (0 == cmp_input_command(input, "set "))
	{
		char color, type;
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
		if (0 == cmp_input_command(input_copy, "king"))
			type = WHITE_K;
		if (0 == cmp_input_command(input_copy, "queen"))
			type = WHITE_Q;
		if (0 == cmp_input_command(input_copy, "rook"))
			type = WHITE_R;
		if (0 == cmp_input_command(input_copy, "knight"))
			type = WHITE_N;
		if (0 == cmp_input_command(input_copy, "bishop"))
			type = WHITE_B;
		if (0 == cmp_input_command(input_copy, "pawn"))
			type = WHITE_P;

		int piece_num = count_piece(color, type);
		if (( (type == WHITE_K || type == WHITE_Q) && piece_num >= 1) ||
			(type == WHITE_P && piece_num >= 8) ||
			( (type == WHITE_R || type == WHITE_N || type == WHITE_B) && piece_num >= 2))
		{
			print_message(NO_PIECE);
			return 0;
		}
		board[i][j].color = color;
		board[i][j].type = type;
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
		/*TODO - when the game starts, if no legal moves, we should move to game end*/
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

/*prints a single move in format "<x,y> to <i,j>[<k,l>�]\n" */
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

/*checks if two tiles are the same*/
int same_tile(board_tile first, board_tile second)
{
	return (first.char_indexer == second.char_indexer && first.int_indexer == second.int_indexer);
}

/*checks if given indices out of counds of board*/
int out_of_boarders(int char_indexer, int int_indexer)
{
	return (char_indexer < 0 || char_indexer >= BOARD_SIZE || int_indexer < 0 || int_indexer >= BOARD_SIZE);
}

/*copies a game move to new memory*/
game_move* copy_move(game_move* cur_move)
{
	if (cur_move == NULL)
		return NULL;
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

	/*returns the char used on board to represent the given type and color of tool*/
	char get_tool_type(char color, char type)
{
	if (color == EMPTY)
		return EMPTY;
	if (color == WHITE)
	{
		return type;
	}
	return toupper(type);
}



	void print_board(board_tile board[BOARD_SIZE][BOARD_SIZE])
	{
		int i, j;
		print_line();
		for (j = BOARD_SIZE - 1; j >= 0; j--)
		{
			printf((j < 9 ? " %d" : "%d"), j + 1);
			for (i = 0; i < BOARD_SIZE; i++){
				printf("| %c ", get_tool_type(board[i][j].color, board[i][j].type));

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

/*returns the opposite color*/
char flip_color(char color)
{
	return BLACK == color ? WHITE : BLACK;
}


/*count the number of pieces of given type and color are on the board*/
int count_piece(color, type)
{
	int counter = 0;
	for (int i = 0; i < BOARD_SIZE; ++i)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			if (color == board[i][j].color && type == board[i][j].type)
				++counter;
		}
	}
	return counter;
}
