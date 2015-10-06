#ifndef CHESS_
#define CHESS_

#define DEBUG 0
#define DEBUG_GUI 1
#define COMP_ONLY 0

#include <stdio.h>

#define BLACK	'B'
#define WHITE	'w'

#define PAWN 'm'
#define BISHOP 'b'
#define KNIGHT 'n'
#define ROOK 'r'
#define QUEEN 'q'
#define KING 'k'


/*start locations of kings and rooks*/
#define CASTLE_KING_COL		4
#define CASTLE_LEFT_ROOK	0
#define CASTLE_RIGHT_ROOK	7
#define W_CASTLE_ROW		0
#define B_CASTLE_ROW		7

#define EMPTY ' '

#define BOARD_SIZE 8

/*prints*/
#define ENTER_SETTINGS "Enter game settings:\n" 
#define WRONG_GAME_MODE "Wrong game mode\n"
#define TWO_PLAYERS_GAME_MODE "Running game in 2 players mode\n"
#define PLAYER_VS_AI_GAME_MODE "Running game in player vs. AI mode\n"
#define WRONG_MINIMAX_DEPTH "Wrong value for minimax depth. The value should be between 1 to 4\n"
#define WRONG_FILE_NAME "Wrong file name\n"
#define WRONG_POSITION "Invalid position on the board\n"
#define NO_PIECE "Setting this piece creates an invalid board\n"  
#define WROND_BOARD_INITIALIZATION "Wrong board initialization\n"
#define NOT_YOUR_PIECE "The specified position does not contain your piece\n"

#define ILLEGAL_COMMAND "Illegal command, please try again\n"
#define ILLEGAL_MOVE "Illegal move\n"
 
#define WRONG_ROOK_POSITION "Wrong position for a rook\n" 
#define ILLEGAL_CALTLING_MOVE "Illegal castling move\n"  

#define TIE "The game ends in a tie\n"

#define perror_message(func_name) (fprintf(stderr, "Error: standard function %s has failed\n", func_name))
#define print_message(message) (printf("%s", message));


#endif
