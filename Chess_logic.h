
#ifndef GAME_LOGIC_
#define GAME_LOGIC_

#include <stdio.h>
#include "Linked_list.h"
#include "Game_flow.h"

/*generate moves*/
linked_list generate_moves(board_tile board[BOARD_SIZE][BOARD_SIZE], char cur_player_color);
void generate_piece_moves(board_tile tile, linked_list* moves);
linked_list generate_castling_moves(board_tile board[BOARD_SIZE][BOARD_SIZE], char color);
void generate_pawn_moves(board_tile tile, linked_list* moves);
void generate_king_moves(board_tile tile, linked_list* moves);
void generate_knight_moves(board_tile tile, linked_list* moves);
void generate_queen_moves(board_tile tile, linked_list* moves);
void generate_bishop_moves(board_tile tile, linked_list* moves);
void generate_rook_moves(board_tile tile, linked_list* moves);
void get_direct_rook_moves(board_tile tile, linked_list* moves, int col, int neg);
void get_direct_bishop_moves(board_tile tile, linked_list* moves, int lft, int up);
int generate_direct_castling_move(linked_list* moves, board_tile board[BOARD_SIZE][BOARD_SIZE], board_tile king, board_tile rook);
void generate_promotion_moves(linked_list* moves, game_move* move);

/*general logic*/
linked_list get_best_moves(board_tile board[BOARD_SIZE][BOARD_SIZE], char color, int depth);
int get_move_score(board_tile board[BOARD_SIZE][BOARD_SIZE], game_move move, int depth);
void filter_moves_with_check(board_tile board[BOARD_SIZE][BOARD_SIZE], linked_list* moves, char color);
char get_winner(board_tile board[BOARD_SIZE][BOARD_SIZE], char color);
int score(board_tile board[BOARD_SIZE][BOARD_SIZE], char color);
int is_board_init_legal();
int is_tile_in_check(board_tile board[BOARD_SIZE][BOARD_SIZE], board_tile tile, char color);
int player_in_check(board_tile board[BOARD_SIZE][BOARD_SIZE], char color);
int player_in_mate(board_tile board[BOARD_SIZE][BOARD_SIZE], char color);

/*do moves*/
void do_move(board_tile m_board[][BOARD_SIZE], game_move move);
void do_castling_move(board_tile m_board[][BOARD_SIZE], board_tile rook, board_tile king);

/*help functions*/
game_move* copy_move(game_move* cur_move);
int up_direction(int start_c, int end_c, int is_white);
int get_board_position(char* input, int* i, int* j);
int find_move(linked_list possible_moves, game_move* move);
int game_move_list_cmp(linked_list list1, linked_list list2);
int is_legal_move(game_move move, char color);
int list_cmp(linked_list list1, linked_list list2);
void init_board(board_tile board[BOARD_SIZE][BOARD_SIZE]);
int promotion(board_tile tile);
int move_cmp(game_move a, game_move b); 
board_tile find_king(board_tile board[BOARD_SIZE][BOARD_SIZE], char color);

#endif GAME_LOGIC_