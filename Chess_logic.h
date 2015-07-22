
#ifndef GAME_LOGIC_
#define GAME_LOGIC_

#include <stdio.h>
#include "Linked_list.h"
#include "Game_flow.h"

int contains_jump(game_move* cur_move, board_tile second, board_tile first);
int same_tile(board_tile first, board_tile second);
int is_legal_move(game_move move, char color);
game_move* copy_move(game_move* cur_move);
int out_of_boarders(int first_indexer, int second_indexer);
void generate_man_moves(board_tile tile, char color, linked_list* best_moves, int* num_eats);
void generate_king_moves(board_tile tile, char color, linked_list* best_moves, int* num_eats);
void generate_eater_moves(board_tile tile, char color, linked_list* best_moves, int* num_eats, game_move* cur_move);
void free_moves(linked_list list);
int game_move_list_cmp(linked_list list1, linked_list list2);
int find_move(linked_list possible_moves, game_move move);
linked_list generate_moves(board_tile board[BOARD_SIZE][BOARD_SIZE], char cur_player_color);
int is_board_init_legal();
char get_tool_type(char color, char type);
int get_board_position(char* input, int* i, int* j);
void print_board(board_tile board[BOARD_SIZE][BOARD_SIZE]);
void init_board(board_tile board[BOARD_SIZE][BOARD_SIZE]);
int score(board_tile board[BOARD_SIZE][BOARD_SIZE], char color);
char flip_color(char color);
void do_part_move(board_tile m_board[][BOARD_SIZE], board_tile start, board_tile end);
int do_computer_move(char color);
void do_move(board_tile m_board[][BOARD_SIZE], game_move move);
int up_direction(int start_c, int end_c, int is_white);
void print_moves(board_tile board[BOARD_SIZE][BOARD_SIZE], char color);
void print_single_move(game_move move);
void print_tile(board_tile tile);
int is_changed_to_king(char pawn, board_tile loc);
char get_winner(board_tile board[BOARD_SIZE][BOARD_SIZE], char color);

#endif GAME_LOGIC_