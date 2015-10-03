#define _CRT_SECURE_NO_DEPRECATE
#include "Game_flow.h"
#include "Gui.h"
#include "Chess_logic.h"
#include "Minimax.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <SDL.h>
#include <stdio.h>

board_tile board[BOARD_SIZE][BOARD_SIZE]; /*game board*/
int should_terminate = 0;
char user_color = WHITE; /*color of the user player*/
int is_turn_end;
int player_vs_player = 1; /*1 - player vs player mode. 2 - player vs comp. 0 (for debug only) - comp vs comp*/
char next_player = WHITE;
int gui = 0; /*0 for command line. 1 for gui*/

int main(int argc, char* argv[])
{
	init_board(board);
	/*main_window();*/
	if (argc > 2)
	{
		print_message("to many command argumants. usage: Chess.exe <gui_type>");
		return -1;
	}
	else if (argc == 2 || (DEBUG && DEBUG_GUI))
	{
		if ((DEBUG && DEBUG_GUI) || strcmp(argv[1], "gui") == 0)
		{
			SDL_Init(SDL_INIT_VIDEO);
			gui = 1;
			main_window();
			SDL_Quit();
		}
		else if (strcmp(argv[1], "console") != 0)
		{
			print_message("arg1 bad format. usage: Chess.exe <gui_type>");
			return -1;
		}
	}
	int input_size = 1024;
	char* input = malloc(input_size);
	if (input == NULL)
	{
		should_terminate = 1;
		perror_message("malloc");
		return -1;
	}
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
	
	
	while (check_game_end(WHITE) != -1)
	{/*game play*/
		if (player_vs_player == 1 || (player_vs_player == 0 && next_player == user_color))
		{/*user turn*/
			is_turn_end = 0;
			char* text = next_player == WHITE ? "white player - enter your move:\n" : "black player - enter your move:\n";
			printf(text);
			while (!is_turn_end)
			{
				if (read_user_input_line(input, &input_size) == -1)
				{
					return -1; /*no resources were allocated yet*/
				}
				int is_turn_end = user_move(input, user_color);
				if (should_terminate)
				{
					free(input);
					return -1;
				}
				if (is_turn_end == -1)
				{
					if (DEBUG)
					{
						getchar();
					}
					free(input);
					return -1;
				}
			}

		}
		else
		{/*computer turn*/
			if (1 == do_computer_move(next_player))
			{/*game ended*/
				if (DEBUG)
				{
					getchar();
				}
			}
			break;
		}
		next_player = flip_color(next_player);
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
int user_move(char* input, char player_color)
{
	if (0 == cmp_input_command(input, "move "))
	{
		int i, j;
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


		input = strchr(input, '<'); /*move to second board place in input*/
		input_copy = strchr(input, '>') + 1;
		if (0 == get_board_position(input, &i, &j))
			return 0;
		input = input_copy;
		if (out_of_boarders(i, j))
		{
			print_message(WRONG_POSITION);
			return 0;
		}
		move.end = board[i][j];

		if (move.start.color != player_color)
		{
			print_message(NO_PIECE);
			return 0;
		}
		while (input[0] == ' ')
			++input;
		/*TODO - pawn promotion*/

		int legal = is_legal_move(move, player_color);
		if (should_terminate)
		{
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
			return 0;
		}

		return check_game_end(player_color);
	}
	if (0 == cmp_input_command(input, "get_moves"))
	{
		print_moves(board, player_color);
		return 0;
	}
	if (0 == cmp_input_command(input, "save "))
	{
		char *file_name = input + strlen("save ");

		return save_config(file_name);
	}
	if (0 == cmp_input_command(input, "quit"))
	{
		should_terminate = 1;
		return -1;
	}
	if (0 == cmp_input_command(input, "get best moves"))
	{
		//todo
	}
	if (0 == cmp_input_command(input, "get score"))
	{
		//todo
	}
	print_message(ILLEGAL_COMMAND);
	return 0;
}

/*saves current game to the file given in config_file_name*/
int save_config(char * config_file_name)
{
	char *xml_info = get_xml_game();
	if (should_terminate)
		return -1;

	FILE *xml_file = fopen(config_file_name, "wt");
	if (NULL == xml_file)
	{
		free(xml_info);
		print_message(WRONG_FILE_NAME);
		return 0;
	}
	int count = fwrite(xml_info, sizeof(char), strlen(xml_info), xml_file);
	if (count != strlen(xml_info))
	{
		free(xml_info);
		should_terminate = 1;
		perror_message("fwrite");
		return -1;
	}
	free(xml_info);
	fclose(xml_file);
	return 0;
}

/*makes a string containing all the xml data based in the xml format*/
char *get_xml_game()
{
	int xml_buf_size = 1024;
	char *xml_data = malloc(xml_buf_size);
	if (xml_data == NULL)
	{
		should_terminate = 1;
		perror_message("malloc");
		return NULL;
	}
	xml_data[0] = '\0'; /*null terminate*/
	concat(xml_data, &xml_buf_size, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<game>\n\t<next_turn>");
	if (should_terminate)
	{
		free(xml_data);
		return NULL;
	}

	char *next = next_player == WHITE ? "White" : "Black";
	concat(xml_data, &xml_buf_size, next);
	if (should_terminate)
	{
		free(xml_data);
		return NULL;
	}

	concat(xml_data, &xml_buf_size, "</next_turn>\n\t<game_mode>");
	if (should_terminate)
	{
		free(xml_data);
		return NULL;
	}

	char game_mode[10];
	_itoa(player_vs_player, game_mode, 10);
	concat(xml_data, &xml_buf_size, game_mode);
	if (should_terminate)
	{
		free(xml_data);
		return NULL;
	}

	concat(xml_data, &xml_buf_size, "</game_ mode>\n\t<difficulty>");
	if (should_terminate)
	{
		free(xml_data);
		return NULL;
	}
	if (2 == player_vs_player)
	{
		if (-1 == minimax_depth)
		{
			concat(xml_data, &xml_buf_size, "best");
		}
		else
		{
			char diff[10];
			_itoa(minimax_depth, diff, 10);
			concat(xml_data, &xml_buf_size, diff);
		}
		if (should_terminate)
		{
			free(xml_data);
			return NULL;
		}

		concat(xml_data, &xml_buf_size, "</difficulty>\n\t<user_color>");
		if (should_terminate)
		{
			free(xml_data);
			return NULL;
		}
		char *user = user_color == WHITE ? "White" : "Black";
		concat(xml_data, &xml_buf_size, user);
		if (should_terminate)
		{
			free(xml_data);
			return NULL;
		}
		concat(xml_data, &xml_buf_size, "</user_color>\n\t<board>\n");
		if (should_terminate)
		{
			free(xml_data);
			return NULL;
		}
		for (size_t i = BOARD_SIZE; i > 0; i--)
		{ /*append the board itself*/
			char row[128];
			row[0] = '\0';
			strcat(row, "\t\t<row_");
			char line_num[8];
			_itoa(i, line_num, 10);
			strcat(row, line_num);
			strcat(row, ">");
			size_t cur_len = strlen(row);
			row[cur_len + BOARD_SIZE] = '\0';
			for (size_t j = 0; j < BOARD_SIZE; j++)
			{
				char c = get_tool_type(board[j][i].color, board[j][i].type);
				if (c == EMPTY)
					c = '_';
				row[cur_len + j] = c;
			}
			strcat(row, "</row_");
			strcat(row, line_num);
			strcat(row, ">\n");
			concat(xml_data, &xml_buf_size, row);
			if (should_terminate)
			{
				free(xml_data);
				return NULL;
			}
		}
	}

	concat(xml_data, &xml_buf_size, "\t<\board>\n</game>");
	if (should_terminate)
	{
		free(xml_data);
		return NULL;
	}
	return xml_data;
}

/*concatinates to strings with reallocating if needed*/
void concat(char *orig, size_t *orig_size, char *addition)
{

	while (strlen(orig) + strlen(addition) >= *orig_size - 1)
	{
		*orig_size *= 2;
		orig = realloc(orig, *orig_size);
		if (orig == NULL)
		{
			should_terminate = 1;
			perror_message("realloc");
			return;
		}
	}
	strcat(orig, addition);
}

/*returns 1 if game ended. otherwise returns 0*/
int check_game_end(char player_color)
{
	int end_game = get_winner(board, flip_color(player_color));
	if (should_terminate)
		return 0;
	if (end_game != 0)
	{
		char* winner = end_game == WHITE ? "White player wins!\n" : "Black player wins!\n";
		print_message(winner);
		return 1;
	}
	return 0;
}


/*return 0 if input starts with*/
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
	if (0 == cmp_input_command(input, "game_mode ")) /*determines if two or single player game*/
	{
		input += strlen("game_mode ");
		if (strcmp(input, "1") == 0)
		{
			print_message(TWO_PLAYERS_GAME_MODE);
			player_vs_player = 1;
		}
		else if (strcmp(input, "2") == 0)
		{
			print_message(PLAYER_VS_AI_GAME_MODE);
			player_vs_player = 2;
		}
		else if (DEBUG && "0\n" == input)
			player_vs_player = 0;
		else
			print_message(WRONG_GAME_MODE);
	}
	if (0 == cmp_input_command(input, "difficulty ")) /*no. of steps for minimax*/
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
	if (0 == cmp_input_command(input, "user_color ")) /*user color in single player game*/
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
	if (0 == cmp_input_command(input, "load ")) /*load saved game settings*/
	{
		input += strlen("load ");
		load_config(input);
		if (should_terminate)
			return 0;
		print_board(board);
	}	
	if (0 == cmp_input_command(input, "clear")) /*clear the board*/
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
	if (0 == cmp_input_command(input, "next_player ")) /*first player color*/
	{
		input += strlen("next_player ");
		if (0 == cmp_input_command(input, "white"))
			next_player = WHITE;
		else if (0 == cmp_input_command(input, "black"))
			next_player = BLACK;
	}
	if (0 == cmp_input_command(input, "rm ")) /*remove piece from location*/
	{
		if (0 == get_board_position(input, &i, &j))
			return 0;
		board[i][j].type = EMPTY;
		board[i][j].color = EMPTY;
		return 0;

	}
	if (0 == cmp_input_command(input, "set ")) /*place piece on location*/
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
	if (0 == cmp_input_command(input, "print")) /*print game board*/
	{
		print_board(board);
		return 0;

	}
	if (0 == cmp_input_command(input, "quit")) /*exit the program*/
	{
		should_terminate = 1;
		return 0;
	}
	if (0 == cmp_input_command(input, "start")) /*start the game*/
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

/*loads all data in a configuration file*/
void load_config(char *file_name)
{
	FILE *setting_file = fopen(file_name, "rt");
	if (NULL == setting_file)
	{
		print_message(WRONG_FILE_NAME)
			return;
	}
	fseek(setting_file, 0, SEEK_END);
	int file_len = ftell(setting_file);
	char *file_data = malloc(file_len);
	if (file_data == NULL)
	{
		should_terminate = 1;
		perror_message("malloc");
		fclose(setting_file);
		return;
	}
	fread(file_data, 1, file_len, setting_file);
	fclose(setting_file);

	char *cur_config;
	cur_config = strstr(file_data, "<next_turn>") + strlen("<next_turn>");
	if (0 == cmp_input_command(cur_config, "White"))
		next_player = WHITE;
	else
		next_player = BLACK;
	cur_config = strstr(file_data, "<game_mode>") + strlen("<game_mode>");
	player_vs_player = cur_config[0] + '0';
	if (player_vs_player == 2)
	{
		cur_config = strstr(file_data, "<difficulty>") + strlen("<difficulty>");
		if (0 == cmp_input_command(cur_config, "best"))
			minimax_depth = -1;
		else
			minimax_depth = cur_config[0] + '0';
		cur_config = strstr(file_data, "<user_color>") + strlen("<user_color>");
		if (0 == cmp_input_command(cur_config, "White"))
			user_color = WHITE;
		else
			user_color = BLACK;
	}
	/*get board*/

	for (size_t i = BOARD_SIZE; i > 0; i--)
	{ /*parse row*/
		file_data = strstr(file_data, "<row_") + strlen("<row_C>");
		for (size_t j = 0; j < BOARD_SIZE; j++)
		{/*parse a char on board*/
			char c = file_data[j];
			board[j][i].color = get_color(c);
			if (c == '_')
				board[j][i].type = EMPTY;
			else
				board[j][i].type = tolower(c);
		}
	}

	free(file_data);

}

/*gets a tile color
input - char c: the char used on board to represent the given type and color of tool*/
char get_color(char c)
{
	if ((c == EMPTY) || (c == '_'))
		return EMPTY;
	if (isupper(c))
		return BLACK;
	return WHITE;
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

void print_best_moves(board_tile board[BOARD_SIZE][BOARD_SIZE], char color, int depth)
{
	linked_list moves;
	linked_list best_moves = new_list();
	game_move* best;
	node* crnt;
	int v = run_minimax(board, &moves, depth, color, &best);
	crnt = moves.first;
	for (int i = 0; i < moves.len; i++, crnt = crnt->next)
	{
		if (((game_move*)(crnt->data))->score == v)
			list_add(&best_moves, (game_move*)(crnt->data));
	}
	crnt = best_moves.first;
	for (int i = 0; i < moves.len; i++, crnt = crnt->next)
		print_single_move(*((game_move*)(crnt->data)));

	free_moves(moves);
	free_moves(best_moves);
	free(best);
}

/*prints a single move in format "<x,y> to <i,j> x\n" */
void print_single_move(game_move move)
{/*
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
	printf("\n");*/
}

void print_line(){
	int i;
	printf("  |");
	for (i = 1; i < BOARD_SIZE * 4; i++){
		printf("-");
	}
	printf("|\n");
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


/*gets a list of game moves, and frees it*/
void free_moves(linked_list list)
{
	node* cur = list.first;
	node* prev = list.first;
	while (cur->next != NULL)
	{
		cur = cur->next;
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
