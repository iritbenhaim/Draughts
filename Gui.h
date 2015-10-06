#include <SDL.h>
#include "Chess.h"

/*save slots*/
#define NUM_SAVE_SLOTS 7
#define SAVE_FILES_PATH "save_files\\"

/*images*/
#define FANCY		"images\\fancy.bmp"
#define GAME_PROG	"images\\game_prog.bmp"
#define QUIT		"images\\quit.bmp"
#define LOAD_GAME	"images\\load_game.bmp"
#define SAVE_GAME	"images\\save_game.bmp"
#define MAIN_MENU	"images\\main_menu.bmp"
#define SHOW_MOVES	"images\\show_moves.bmp"
#define BEST_MOVE	"images\\best_move.bmp"
#define NEW_GAME	"images\\new_game.bmp"
#define EMPTY_IMG	"images\\empty.bmp"
#define USED_IMG	"images\\used.bmp"
#define CANCEL_IMG	"images\\cancel2.bmp"
#define SYMBOLS		"images\\Chess_symbols.bmp"
#define P_VS_C		"images\\vs_comp.bmp"
#define P_VS_P		"images\\vs_player.bmp"
#define USER_BLACK	"images\\user_black.bmp"
#define USER_WHITE	"images\\user_white.bmp"
#define NEXT_BLACK	"images\\next_black.bmp"
#define NEXT_WHITE	"images\\next_white.bmp"
#define FINISH		"images\\finish.bmp"
#define DIFFICULTY	"images\\diff_0.bmp"
#define DIFF_GEN_OFFSET	strlen("images\\diff_")
#define CHECK		"images\\check.bmp"
#define MATE		"images\\mate.bmp"
#define END_TIE		"images\\tie.bmp"
#define ILLEGAL		"images\\illegal_init.bmp"



/*main window*/
#define MAIN_WIN_W 300
#define MAIN_WIN_H 390
#define MAIN_IMG_W 260
#define MAIN_IMG_H 54
#define MAIN_WIND_BUTTONS 3

/*game window*/
#define GAME_WIN_W 800
#define GAME_WIN_H 600
#define SQUERE_S 59
#define GAME_IMG_W 275
#define GAME_IMG_H 54
#define GAME_WIND_BUTTONS 6

/*load window*/
#define LOAD_BTN_W 140
#define LOAD_BTN_H 80
#define BTNS_PER_LINE 3

#define WIDTH 640
#define HEIGHT 480
#define BPP 4
#define DEPTH 32

/*AI settings window*/
#define AI_WIND_BUTTONS 4

/*player selection window*/
#define PLAY_IMG_W 275
#define PLAY_IMG_H 48
#define PLAY_WIND_BUTTONS 4



/*game windows functions*/
int  game_window();
int main_window();
int player_selection_window();
int load_save_game_wind(int is_save);
int ai_settings_window();

/*drawing helper funcs*/
void draw_current_board(SDL_Surface *w);
void draw_image(SDL_Rect img_rect, SDL_Rect img_place, char* img_path, SDL_Surface* w, int should_fill);
void paint_rect_edges(SDL_Rect rect, SDL_Surface *w, int color);
void paint_boarder_pieces(int is_promotion, char color, int is_top, SDL_Surface *w);
void paint_all_buttons(SDL_Surface *w, char ** images, int img_count, SDL_Rect top_button, SDL_Rect imgrect);


void init_turn_gui(char **pEnd_game_image, SDL_Surface *w, int is_first_turn);
int get_num_button_pressed(int x, int y, SDL_Rect top_button, int button_count);
void get_tool_rect(struct board_tile *tool, SDL_Rect *out_rect);
int is_in_rect(int x, int y, SDL_Rect rect);
int is_save_slot_free(int slot_num);
int save_load_game_from_slot(int slot_num, int is_save);
int handle_board_press(SDL_Event e, SDL_Surface *w);
char do_pawn_promotion(SDL_Surface *w);
void get_board_rect(int colnum, int rownum, SDL_Rect *out_rect);
int get_tile_col(int x);
int get_tile_row(int y);
char get_boarder_pieces_choice(int x, int y, int is_promotion, int is_top);
void handle_board_setting_press(SDL_Event e, SDL_Surface *w, char piece, char color);
