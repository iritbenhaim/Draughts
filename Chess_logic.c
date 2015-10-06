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
	if (move.start.color != color)
		return 0;
	linked_list all_moves = new_list();
	generate_piece_moves(board, move.start, &all_moves);
	filter_moves_with_check(board, &all_moves, color);
	/*linked_list all_moves = generate_moves(board, color);*/
	if (should_terminate)
	{
		return 0;
	}
	int is_success = find_move(all_moves, &move);

	free_moves(all_moves);
	return is_success;
}

/*returns 0 if one of the sides has no king or has more pieces of any king than the starting amount
else returns 1*/
int is_board_init_legal()
{
	int count_black_pieces[6] = { 0, 0, 0, 0, 0, 0 };
	int count_white_pieces[6] = { 0, 0, 0, 0, 0, 0 };
	int *current_count;
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			/*check witch board to inc*/
			if (board[i][j].color == WHITE)
				current_count = count_white_pieces;
			else if (board[i][j].color == BLACK)
				current_count = count_black_pieces;
			else
				continue;
			/*check witch piece to inc*/
			switch (board[i][j].type)
			{
			case KING:
				current_count[0]++;
				break;
			case QUEEN:
				current_count[1]++;
				break;
			case ROOK:
				current_count[2]++;
				break;
			case BISHOP:
				current_count[3]++;
				break;
			case KNIGHT:
				current_count[4]++;
				break;
			case PAWN:
				current_count[5]++;
				break;
			}
		}
	}
	int is_legal = 1;
	current_count = count_black_pieces;
	for (int i = 0; i < 2; ++i)
	{
		if (current_count[0] != 1 /*exacly one king*/
			|| current_count[1] > 1 /*at most one queen*/
			|| current_count[2] > 2 /*at most 2 rooks*/
			|| current_count[3] > 2 /*at most 2 bishops*/
			|| current_count[4] > 2 /*at most 2 knights*/
			|| current_count[5] > 8) /*at most 8 pawns*/
		{
			is_legal = 0;
			break;
		}
		current_count = count_white_pieces;
	}
	return is_legal;
}

/*if a player has won return its color, if no one won yet return 0
  a player wins if mate was reached*/
char get_winner(board_tile board[BOARD_SIZE][BOARD_SIZE])
{
	if (player_in_mate(board, WHITE))
		return BLACK; /*current player in mate, other player wins*/
	if (player_in_mate(board, BLACK))
		return WHITE; /*other player in mate, current player wins*/
	return 0; /*no one won yet*/
}

/*gets a string containing <x,y> - a board position.
fills i and j - the indieces pointed by x, y
returns 0 if the position is outside the range. else 1*/
int get_board_position(char* input, int* i, int* j)
{
	int temp;
	int start = strchr(input, '<') - input;
	int end = strchr(input, ',') - input;
	if (end - start != 2 || input[start + 1] < 'a' || input[start + 1] > 'a' + BOARD_SIZE)
	{
		print_message(WRONG_POSITION);
		return 0;
	}
	*strchr(input, '>') = '\0';
	temp = atoi(input + end + 1);
	if (temp < 1 || temp > BOARD_SIZE)
	{
		print_message(WRONG_POSITION);
		return 0;
	}
	*j = temp - 1;
	*i = input[start + 1] - 'a';
	return 1;

}


/*
performs a single move
removes opponent tool eaten
*/
void do_move(board_tile m_board[][BOARD_SIZE], game_move move)
{
	char type = move.start.type;
	char color = move.start.color;
	m_board[move.end.char_indexer][move.end.int_indexer].color = color;
	m_board[move.end.char_indexer][move.end.int_indexer].type = type;
	m_board[move.start.char_indexer][move.start.int_indexer].type = EMPTY;
	m_board[move.start.char_indexer][move.start.int_indexer].color = EMPTY;
	m_board[move.start.char_indexer][move.start.int_indexer].moved = 1;
	if (move.start.type == KING && move.start.char_indexer == 4 && (move.end.char_indexer == 2 || move.end.char_indexer == 6))
	{/*casteling*/
		int row = move.start.int_indexer;
		int start_col = move.end.char_indexer == 2 ? 0 : 7; /*check if left or right rook*/
		int end_row = move.end.char_indexer == 2 ? 3 : 5;
		m_board[end_row][row].color = color;
		m_board[end_row][row].type = ROOK;
		m_board[start_col][row].type = EMPTY;
		m_board[start_col][row].color = EMPTY;
	}
	if (promotion(move.start))
		m_board[move.end.char_indexer][move.end.int_indexer].type = move.promote;
}

/*return 1 if tile needs promotion, 0 otherwise*/
int promotion(board_tile tile)
{
	if (tile.type != PAWN)
		return 0;
	if (tile.int_indexer == BOARD_SIZE - 2 && tile.color == WHITE)
		return 1;
	if (tile.int_indexer == 1 && tile.color == BLACK)
		return 1;
	return 0;
}

/*performs a castling move*/
void do_castling_move(board_tile m_board[][BOARD_SIZE], board_tile rook, board_tile king)
{
	char k_end = king.char_indexer;
	char r_end;
	k_end += rook.char_indexer > king.char_indexer ? 2 : -2;
	r_end = k_end + (k_end > king.char_indexer ? -1 : 1);
	/*move king*/
	m_board[k_end][king.int_indexer].color = king.color;
	m_board[k_end][king.int_indexer].type = king.type;
	m_board[king.char_indexer][king.int_indexer].type = EMPTY;
	/*move rook*/
	m_board[r_end][rook.int_indexer].color = rook.color;
	m_board[r_end][rook.int_indexer].type = rook.type;
	m_board[rook.char_indexer][rook.int_indexer].type = EMPTY;
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
  win -> 1000
  lose -> -1000
  */
int score(board_tile board[BOARD_SIZE][BOARD_SIZE], char color)
{
	int black = 0;
	int white = 0;
	int score;
	char tile;
	char col;
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			tile = board[i][j].type;
			col = board[i][j].color;
			if (tile == EMPTY)
				continue;
			if (tile == KING)
				score = 400;
			if (tile == QUEEN)
				score = 9;
			if ((tile == KNIGHT) || (tile == BISHOP))
				score = 3;
			if (tile == ROOK)
				score = 5;
			if (tile == PAWN)
				score = 1;
			if (col == WHITE)
				white += score;
			else
				black += score;
		}
	}
	if (color == BLACK)
	{
		if (white == 0)
			return 1000;
		if (black == 0)
			return -1000;
		return black - white;
	}
	else
	{
		if (white == 0)
			return -1000;
		if (black == 0)
			return 1000;
		return white - black;
	}
}

/*initializes the board as a normal chess game start*/
void init_board(board_tile board[BOARD_SIZE][BOARD_SIZE]){
	int i,j;
	for (i = 0; i < BOARD_SIZE; i++){
		for (j = 0; j < BOARD_SIZE; j++){
			if (1 == j || 6 == j) {
				board[i][j].type = PAWN;
				board[i][j].color = j == 1 ? WHITE : BLACK;
			}
			else if (j == 0)
				board[i][j].color = WHITE;
			else if (j == BOARD_SIZE-1)
				board[i][j].color = BLACK;
			else
			{
				board[i][j].type = EMPTY;
				board[i][j].color = EMPTY;
			}
			board[i][j].char_indexer = i;
			board[i][j].int_indexer = j;
			board[i][j].moved = 0;
		}
	}
	board[0][0].type = ROOK;
	board[1][0].type = KNIGHT;
	board[2][0].type = BISHOP;
	board[3][0].type = QUEEN;
	board[4][0].type = KING;
	board[5][0].type = BISHOP;
	board[6][0].type = KNIGHT;
	board[7][0].type = ROOK;
	board[0][BOARD_SIZE - 1].type = ROOK;
	board[1][BOARD_SIZE - 1].type = KNIGHT;
	board[2][BOARD_SIZE - 1].type = BISHOP;
	board[3][BOARD_SIZE - 1].type = QUEEN;
	board[4][BOARD_SIZE - 1].type = KING;
	board[5][BOARD_SIZE - 1].type = BISHOP;
	board[6][BOARD_SIZE - 1].type = KNIGHT;
	board[7][BOARD_SIZE - 1].type = ROOK;

}


/*initializes all globals of game to start a new game*/
void init_game()
{
	init_board(board);
	minimax_depth = 1;
	player_vs_player = 1;
	user_color = WHITE;
	next_player = WHITE;
	check = 0;
	mate = 0;
	tie = 0;
}

/*initialize stuff that should be changed or checked on every move start.
returns 1 if game ended. otherwise returns 0*/
int init_turn(int is_first_turn)
{
	if (!is_first_turn)
		next_player = flip_color(next_player);
	return check_game_end(next_player);
}

/*returns 1 if a and b are the same move, else 0*/
int move_cmp(game_move a, game_move b)
{
	if (!tile_cmp(a.start, b.start))
		return 0;
	if (!tile_cmp(a.end, b.end))
		return 0;
	if (promotion(a.start))
		if (a.promote != b.promote)
			return 0;
	return 1;
}

/*searches for the move in possible moves list. return 1 if found. else 0
  the function also updates the score for the move if found*/
int find_move(linked_list possible_moves, game_move* move)
{
	node* crnt = possible_moves.first;
	for (int i = 0; i < possible_moves.len; i++)
	{
		game_move legit_move = *((game_move*)crnt->data);
		if (move_cmp(*move, legit_move))
		{
			move->score = legit_move.score;
			return 1;
		}
		crnt = crnt->next;
	}
	return 0;
}

/*this function finds the score for the move and returns it, 
  if its not a legal move it returns 0*/
int get_move_score(board_tile board[BOARD_SIZE][BOARD_SIZE], game_move move, int depth)
{
	linked_list moves;
	game_move* best;
	run_minimax(board, &moves, depth, move.start.color, &best); /*run minimax to find score for moves*/
	if (!find_move(moves, &move))
		return 0; /*move not legal*/
	return move.score; /*updated score*/
}

/*returns a list with all moves with highest score according to minimax algorithm*/
linked_list get_best_moves(board_tile board[BOARD_SIZE][BOARD_SIZE], char color, int depth)
{
	linked_list moves;
	game_move* best = NULL;
	node* crnt;
	int v = run_minimax(board, &moves, depth, color, &best); /*run minimax to find highest score*/
	crnt = moves.first;
	for (int i = 0; i < moves.len; i++, crnt = crnt->next)
	{
		if (((game_move*)(crnt->data))->score != v)	/*find all moves with v - highest score*/
			list_remove(&moves, crnt->data);
	}
	free(best);
	return moves;
}

/*returns a linked list containing all possible moves for a player
cur_player_color - the color of the current player
should_generate_king - if 0, will not genrate kings moves*/
linked_list generate_moves(board_tile board[BOARD_SIZE][BOARD_SIZE], char cur_player_color, int is_check, int should_generate_king)
{
	int i, j;
	linked_list moves;

	moves = new_list();
	if (should_terminate)
		return moves;
	for (i = 0; i < BOARD_SIZE; ++i)
	{
		for (j = 0; j < BOARD_SIZE; j++)
		{
			if (cur_player_color != board[i][j].color) /*other player piece*/
				continue;
			if (!should_generate_king && board[i][j].type == KING)
				continue;
			generate_piece_moves(board, board[i][j], &moves); /*get all moves for this piece*/
		}
	}
	if (is_check)
		filter_moves_with_check(board, &moves, cur_player_color);
	return moves;
}

/*checks the type of a single piece and generates moves accordingly*/
void generate_piece_moves(board_tile board[BOARD_SIZE][BOARD_SIZE], board_tile tile, linked_list* moves)
{
	char type = tile.type;
	switch (type)
	{
	case PAWN:
		generate_pawn_moves(board, tile, moves);
		break;
	case KING:
		generate_king_moves(board, tile, moves);
		break;
	case BISHOP:
		generate_bishop_moves(board, tile, moves);
		break;
	case KNIGHT:
		generate_knight_moves(board, tile, moves);
		break;
	case ROOK:
		generate_rook_moves(board, tile, moves);
		break;
	case QUEEN:
		generate_queen_moves(board, tile, moves);
		break;
	}
}

/*returns a list containing all possible rooks that can be used in a castling move for a player
*/
void generate_castling_moves(board_tile board[BOARD_SIZE][BOARD_SIZE], char color, linked_list* moves)
{
	if (color == WHITE)
	{
		if (board[CASTLE_KING_COL][W_CASTLE_ROW].moved)
			return;
		if (!board[CASTLE_LEFT_ROOK][W_CASTLE_ROW].moved)
			generate_direct_castling_move(moves, board, board[CASTLE_KING_COL][W_CASTLE_ROW], board[CASTLE_LEFT_ROOK][W_CASTLE_ROW]);
		if (!board[CASTLE_RIGHT_ROOK][W_CASTLE_ROW].moved)
			generate_direct_castling_move(moves, board, board[CASTLE_KING_COL][W_CASTLE_ROW], board[CASTLE_RIGHT_ROOK][W_CASTLE_ROW]);
	}
	if (color == BLACK)
	{
		if (board[CASTLE_KING_COL][B_CASTLE_ROW].moved)
			return;
		if (!board[CASTLE_LEFT_ROOK][B_CASTLE_ROW].moved)
			generate_direct_castling_move(moves, board, board[CASTLE_KING_COL][B_CASTLE_ROW], board[CASTLE_LEFT_ROOK][B_CASTLE_ROW]);
		if (!board[CASTLE_RIGHT_ROOK][B_CASTLE_ROW].moved)
			generate_direct_castling_move(moves, board, board[CASTLE_KING_COL][B_CASTLE_ROW], board[CASTLE_RIGHT_ROOK][B_CASTLE_ROW]);
	}
}

/*return legal moves for a king*/
void generate_king_moves(board_tile board[BOARD_SIZE][BOARD_SIZE], board_tile tile, linked_list* moves)
{
	char color = tile.color;
	generate_castling_moves(board, color, moves);

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
			if (board[tile.char_indexer + c][tile.int_indexer + r].type != EMPTY &&
				board[tile.char_indexer + c][tile.int_indexer + r].color == color)
				continue;
			if (r == 0 && c == 0) /*stay in place - not a move*/
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
void generate_knight_moves(board_tile board[BOARD_SIZE][BOARD_SIZE], board_tile tile, linked_list* moves)
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
			/*check if move is illegal, if so, discard and continue*/
			if (out_of_boarders(tile.char_indexer + indxs[c], tile.int_indexer + indxs[r]))
				continue;
			if (abs(indxs[r]) == abs(indxs[c]))
				continue;
			if (board[tile.char_indexer + indxs[c]][tile.int_indexer + indxs[r]].color == color)
				continue;
			board_tile next = board[(tile.char_indexer) + indxs[c]][tile.int_indexer + indxs[r]];
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

/*returns a single move forward or diagonally if capturing*/
void generate_pawn_moves(board_tile board[BOARD_SIZE][BOARD_SIZE], board_tile tile, linked_list* moves)
{
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
		/*check if move is illegal, if so, discard move and continue*/
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
		cur_move->promote = EMPTY;
		
		/*check if white pawn reached upper line or black pawn reached lower line*/
		if (promotion(cur_move->start))
			generate_promotion_moves(board, moves, cur_move);

		else
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
	free(cur_move);
}

/*adds all possible promotion moves when promotion is achieved*/
void generate_promotion_moves(board_tile board[BOARD_SIZE][BOARD_SIZE], linked_list* moves, game_move* move)
{
	char types[] = { BISHOP, KNIGHT, QUEEN, ROOK };
	for (int i = 0; i < 4; i++)
	{
		game_move* cur_move = copy_move(move);
		if (should_terminate)
			return;
		cur_move->promote = types[i];
		list_add(moves, cur_move);
		if (should_terminate)
		{
			free(cur_move);
			return;
		}
	}
}

/*queen moves combines possible moves for rook and bishop*/
void generate_queen_moves(board_tile board[BOARD_SIZE][BOARD_SIZE], board_tile tile, linked_list* moves)
{
	generate_rook_moves(board, tile, moves); /*possible queen moves as rook moves*/
	generate_bishop_moves(board, tile, moves); /* possible queen moves as bishop moves*/
}

/*returns all legal bishop moves along the diagonals*/
void generate_bishop_moves(board_tile board[BOARD_SIZE][BOARD_SIZE], board_tile tile, linked_list* moves)
{
	get_direct_bishop_moves(board, tile, moves, 1, 1);
	get_direct_bishop_moves(board, tile, moves, 1, 0);
	get_direct_bishop_moves(board, tile, moves, 0, 1);
	get_direct_bishop_moves(board, tile, moves, 0, 0);
}

/*return all legal rook moves along row and column*/
void generate_rook_moves(board_tile board[BOARD_SIZE][BOARD_SIZE], board_tile tile, linked_list* moves)
{
	get_direct_rook_moves(board, tile, moves, 1, 1);
	get_direct_rook_moves(board, tile, moves, 0, 1);
	get_direct_rook_moves(board, tile, moves, 1, 0);
	get_direct_rook_moves(board, tile, moves, 0, 0);
}

/*this function adds rook moves for a specific direction of four directions:
  up, down, left, right - reflected in col and neg variables*/
void get_direct_rook_moves(board_tile board[BOARD_SIZE][BOARD_SIZE], board_tile tile, linked_list* moves, int col, int neg)
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
			c = col ? c - 1 : c;
			r = !col ? r - 1 : r;
		}
		else
		{
			c = col ? c + 1 : c;
			r = !col ? r + 1 : r;
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
		/*malloc the next move*/
		cur_move = malloc(sizeof(game_move));
		if (cur_move == NULL)
		{
			should_terminate = 1;
			perror_message("malloc");
			return;
		}
		cur_move->start = tile;

		if (next.color == flip_color(color)) /*eat move - no more moves to this direction*/
			break;
	}
	free(cur_move);
}

/*this function adds bishop moves for a specific firection of four directions
  reflected in lft and up variables*/
void get_direct_bishop_moves(board_tile board[BOARD_SIZE][BOARD_SIZE], board_tile tile, linked_list* moves, int lft, int up)
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
		c = !lft ? c + 1 : c - 1; /*advance indexes according to directionality*/
		r = up ? r + 1 : r - 1;
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
		/*malloc the next move*/
		cur_move = malloc(sizeof(game_move));
		if (cur_move == NULL)
		{
			should_terminate = 1;
			perror_message("malloc");
			return;
		}
		cur_move->start = tile;

		if (next.color == flip_color(color)) /*eat move - no more moves to this direction*/
			break;
	}
	free(cur_move);
}

/*returns 1 if king not in check and no legal moves, otherwise 0*/
int player_in_tie(board_tile board[BOARD_SIZE][BOARD_SIZE], char color)
{
	linked_list moves = generate_moves(board, color, 1, 1);
	if ((moves.len == 0) && !player_in_check(board, color))
		return 1;
	return 0;
}

/*return 1 if king in check, otherwise 0*/
int player_in_check(board_tile board[BOARD_SIZE][BOARD_SIZE], char color)
{
	board_tile king = find_king(board, color);
	return is_tile_in_check(board, king, color);
}

/*returns 1 if king in check and no moves are possible, otherwise returns 0*/
int player_in_mate(board_tile board[BOARD_SIZE][BOARD_SIZE], char color)
{
	linked_list moves = generate_moves(board, color, 1, 1);
	if (moves.len == 0 && player_in_check(board, color))
		return 1;
	return 0;
}

/*removes from move list all moves that lead to check*/
void filter_moves_with_check(board_tile board[BOARD_SIZE][BOARD_SIZE], linked_list* moves, char color)
{
	board_tile copy[BOARD_SIZE][BOARD_SIZE];
	board_tile king;
	node* move = moves->first;
	game_move crnt;
	int len = moves->len;
	for (int i = 0; i < len; i++)
	{
		crnt = *(game_move*)(move->data);
		if (crnt.start.type == KING && crnt.start.char_indexer == 4 && (crnt.end.char_indexer == 2 || crnt.end.char_indexer == 6))
		{/*casteling move*/
			if (is_tile_in_check(board, crnt.start, color)) /*check board before move*/
			{
				node* tmp = move; /*we might need to remove this node*/
				move = move->next;
				list_remove(moves, tmp->data); /*remove move from possible moves*/
				continue;
			}
			/*check middle tile*/
			board_tile middle = board[crnt.start.char_indexer + (crnt.end.char_indexer - crnt.start.char_indexer) / 2][crnt.start.int_indexer];
			if (is_tile_in_check(board, middle, color)) /*check board before move*/
			{
				node* tmp = move; /*we might need to remove this node*/
				move = move->next;
				list_remove(moves, tmp->data); /*remove move from possible moves*/
				continue;
			}
		}
		copy_board(board, copy);
		do_move(copy, crnt); /*do move on copy board*/
		king = find_king(copy, color);
		if (king.type == EMPTY)
			return;
		node* tmp = move; /*we might need to remove this node*/
		move = move->next;
		if (is_tile_in_check(copy, king, king.color)) /*check board after move*/
			list_remove(moves, tmp->data); /*remove move from possible moves*/
		
	}
}

/*returns the tile of the king or an empty tile if there is no king*/
board_tile find_king(board_tile board[BOARD_SIZE][BOARD_SIZE], char color)
{
	board_tile out;
	out.type = EMPTY;
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			if ((board[i][j].color == color) && (board[i][j].type == KING))
				return board[i][j];
		}
	}
	return out;
}

/*determines if certain tile on board is in attack*/
int is_tile_in_check(board_tile board[BOARD_SIZE][BOARD_SIZE], board_tile tile, char color)
{
	linked_list enemy_moves;
	int is_check = 0;
	/*generate all possible moves for other player*/
	enemy_moves = generate_moves(board, flip_color(color), 0, 0);
	/*for each move check if end tile is tile in question*/
	node* move = enemy_moves.first;
	for (int i = 0; i < enemy_moves.len; i++)
	{
		board_tile end = (*(game_move*)move->data).end;
		if (end.char_indexer == tile.char_indexer && end.int_indexer == tile.int_indexer)
		{
			is_check = 1; /*current tile is in check*/
			break;
		}
		move = move->next;
	}
	free_moves(enemy_moves);
	return is_check;
}

/*this function checks if castling move is possible with given king and rook
if possible, it attaches the rook tile to list of moves
*/
int generate_direct_castling_move(linked_list* moves, board_tile board[BOARD_SIZE][BOARD_SIZE], board_tile king, board_tile rook)
{
	int k_start[] = { king.char_indexer, king.int_indexer };
	/*if (is_tile_in_check(board, king, king.color))
		return 1;*/ /*checked in filter moves*/
	board_tile k_end;
	if (king.type != KING || rook.type != ROOK)
		return 1;
	if (rook.char_indexer < king.char_indexer) /*left rook*/
	{
		k_end = board[king.char_indexer - 2][king.int_indexer]; /*end position for king*/
		if (board[king.char_indexer - 1][king.int_indexer].type != EMPTY ||
			board[king.char_indexer - 2][king.int_indexer].type != EMPTY ||
			board[king.char_indexer - 3][king.int_indexer].type != EMPTY)
			return 1;
	}
	else /*right rook*/
	{
		k_end = board[king.char_indexer + 2][king.int_indexer]; /*end position for king*/
		if (board[king.char_indexer + 1][king.int_indexer].type != EMPTY ||
			board[king.char_indexer + 2][king.int_indexer].type != EMPTY)
			return 1;
	}
	game_move *move = malloc(sizeof(game_move));
	if (move == NULL)
	{
		should_terminate = 1;
		perror_message("malloc");
		return 1;
	}
	move->start = king;
	move->end = k_end;
	list_add(moves, move); /*current rook and king can perform castling*/
	return 0;
}

/*creates and returns a copy of a game move*/
game_move* copy_move(game_move* cur_move)
{
	game_move* copy = malloc(sizeof(game_move));
	if (cur_move == NULL)
	{
		should_terminate = 1;
		perror_message("malloc");
		return copy;
	}
	copy->start = cur_move->start;
	copy->end = cur_move->end;
	copy->promote = cur_move->promote;
	return copy;

}