
#ifndef GAME_LOGIC_
#define GAME_LOGIC_

#include <stdio.h>
#include "Linked_list.h"
#include "Game_flow.h"

int contains_jump(game_move* cur_move, board_tile second, board_tile first);
int is_legal_move(game_move move, char color);
void print_line();
void generate_pawn_moves(board_tile tile, linked_list* moves);
void generate_king_moves(board_tile tile, linked_list* moves);
void generate_knight_moves(board_tile tile, linked_list* moves);
void generate_queen_moves(board_tile tile, linked_list* moves);
void generate_bishop_moves(board_tile tile, linked_list* moves);
void generate_rook_moves(board_tile tile, linked_list* moves);
void get_direct_rook_moves(board_tile tile, linked_list* moves, int col, int neg);
void get_direct_bishop_moves(board_tile tile, linked_list* moves, int lft, int up);
int list_cmp(linked_list list1, linked_list list2);
int find_move(linked_list possible_moves, game_move move);
linked_list generate_moves(board_tile board[BOARD_SIZE][BOARD_SIZE], char cur_player_color);
int is_board_init_legal();
int get_board_position(char* input, int* i, int* j);
void init_board(board_tile board[BOARD_SIZE][BOARD_SIZE]);
int score(board_tile board[BOARD_SIZE][BOARD_SIZE], char color);
void do_part_move(board_tile m_board[][BOARD_SIZE], board_tile start, board_tile end);
int do_computer_move(char color);
void do_move(board_tile m_board[][BOARD_SIZE], game_move move);
int up_direction(int start_c, int end_c, int is_white);
char get_winner(board_tile board[BOARD_SIZE][BOARD_SIZE], char color);
void print_line();

#endif GAME_LOGIC_