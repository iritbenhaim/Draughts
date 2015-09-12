#include <SDL.h>

int test();
void DrawScreen(SDL_Surface* screen, int h);
void setpixel(SDL_Surface *screen, int x, int y, Uint8 r, Uint8 g, Uint8 b);
typedef struct Button Button;
int main_window();
int is_in_rect(int x, int y, SDL_Rect rect);