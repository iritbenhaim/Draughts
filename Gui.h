#include <SDL.h>

#define NUM_SAVE_SLOTS 7

/*images*/
#define GAME_PROG	"images\\game_prog.bmp"
#define QUIT		"images\\quit.bmp"
#define LOAD_GAME	"images\\load_game.bmp"
#define NEW_GAME	"images\\new_game.bmp"
#define EMPTY_IMG	"images\\empty.bmp"
#define USED_IMG	"images\\used.bmp"

/*main window*/
#define MAIN_WIN_W 300
#define MAIN_WIN_H 390
#define MAIN_IMG_W 260
#define MAIN_IMG_H 54


/*load window*/
#define LOAD_BTN_W 140
#define LOAD_BTN_H 80
#define BTNS_PER_LINE 3

#define WIDTH 640
#define HEIGHT 480
#define BPP 4
#define DEPTH 32


int test();
void DrawScreen(SDL_Surface* screen, int h);
void setpixel(SDL_Surface *screen, int x, int y, Uint8 r, Uint8 g, Uint8 b);
typedef struct Button Button;
int main_window();
int is_in_rect(int x, int y, SDL_Rect rect);
int load_game_wind();