#include <SDL.h>
#include "Chess.h"

#define NUM_SAVE_SLOTS 7

#define SAVE_FILES_PATH "save_files/"

/*images*/
#define GAME_PROG	"images\\game_prog.bmp"
#define QUIT		"images\\quit.bmp"
#define LOAD_GAME	"images\\load_game.bmp"
#define NEW_GAME	"images\\new_game.bmp"
#define EMPTY_IMG	"images\\empty.bmp"
#define USED_IMG	"images\\used.bmp"
#define CANCEL_IMG	"images\\cancel.bmp"
#define SYMBOLS		"images\\Chess_symbols.bmp"

/*main window*/
#define MAIN_WIN_W 300
#define MAIN_WIN_H 390
#define MAIN_IMG_W 260
#define MAIN_IMG_H 54

/*game window*/
#define GAME_WIN_W 800
#define GAME_WIN_H 600
#define SQUERE_S 61

/*load window*/
#define LOAD_BTN_W 140
#define LOAD_BTN_H 80
#define BTNS_PER_LINE 3

#define WIDTH 640
#define HEIGHT 480
#define BPP 4
#define DEPTH 32


SDL_Rect *get_tool_rect(struct board_tile tool);
int  game_window();
int main_window();
int is_in_rect(int x, int y, SDL_Rect rect);
int load_game_wind(); 
int is_save_slot_free(int slot_num);
void load_game_from_slot(int slot_num);
void draw_current_board(SDL_Surface *symbols_img, SDL_Surface *w);
