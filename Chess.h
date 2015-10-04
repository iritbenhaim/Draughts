
#ifndef CHESS_
#define CHESS_


#define DEBUG 1
#define DEBUG_GUI 1
#define COMP_ONLY 0


#include <stdio.h>

#define WHITE	'w'
#define WHITE_P 'm'
#define WHITE_B 'b'
#define WHITE_N 'n'
#define WHITE_R 'r'
#define WHITE_Q 'q'
#define WHITE_K 'k'

#define BLACK	'B'

#define W_K_X 4
#define W_K_Y 0
#define W_L_R_X 0
#define W_L_R_Y 0
#define W_R_R_X 9
#define W_R_R_Y 0
#define B_K_X 4
#define B_K_Y 9
#define B_L_R_X 0
#define B_L_R_Y 9
#define B_R_R_X 9
#define B_R_R_Y 9



#define EMPTY ' '

#define BOARD_SIZE 8

/* board initialization*/
#define ENTER_SETTINGS "Enter game settings:\n" 
#define WRONG_GAME_MODE "Wrong game mode\n"
#define TWO_PLAYERS_GAME_MODE "Running game in 2 players mode\n"
#define PLAYER_VS_AI_GAME_MODE "Running game in player vs. AI mode\n"
#define WRONG_MINIMAX_DEPTH "Wrong value for minimax depth. The value should be between 1 to 4\n"
#define WRONG_FILE_NAME "Wrong file name\n"
#define WRONG_POSITION "Invalid position on the board\n"
#define NO_PIECE "Setting this piece creates an invalid board\n"  
#define WROND_BOARD_INITIALIZATION "Wrong board initialization\n"

#define ILLEGAL_COMMAND "Illegal command, please try again\n"
#define ILLEGAL_MOVE "Illegal move\n"
 
#define WRONG_ROOK_POSITION "Wrong position for a rook\n" 
#define ILLEGAL_CALTLING_MOVE "Illegal castling move\n"  

#define TIE "The game ends in a tie\n"

#define perror_message(func_name) (fprintf(stderr, "Error: standard function %s has failed\n", func_name))
#define print_message(message) (printf("%s", message));


#endif CHESS_