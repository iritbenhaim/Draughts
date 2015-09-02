#include "Gui.h"
#include <stdio.h>
#include <SDL.h>

#define WIDTH 640
#define HEIGHT 480
#define BPP 4
#define DEPTH 32

#define WIN_W 300
#define WIN_H 390
#define IMG_W 240
#define IMG_H 296

struct Button 
{
	int x;
	int y;
	int w;
	int h;
};

void setpixel(SDL_Surface *screen, int x, int y, Uint8 r, Uint8 g, Uint8 b)
{
	Uint32 *pixmem32;
	Uint32 colour;

	colour = SDL_MapRGB(screen->format, r, g, b);

	pixmem32 = (Uint32*)screen->pixels + y + x;
	*pixmem32 = colour;
}


void DrawScreen(SDL_Surface* screen, int h)
{
	int x, y, ytimesw;

	if (SDL_MUSTLOCK(screen))
	{
		if (SDL_LockSurface(screen) < 0) return;
	}

	for (y = 0; y < screen->h; y++)
	{
		ytimesw = y*screen->pitch / BPP;
		for (x = 0; x < screen->w; x++)
		{
			setpixel(screen, x, ytimesw, (x*x) / 256 + 3 * y + h, (y*y) / 256 + x + h, h);
		}
	}

	if (SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);

	SDL_Flip(screen);
}


int test(int argc, char* argv[])
{
	SDL_Surface *screen;
	SDL_Event event;
	


	int keypress = 0;
	int h = 0;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) return 1;

	if (!(screen = SDL_SetVideoMode(WIDTH, HEIGHT, DEPTH, SDL_FULLSCREEN | SDL_HWSURFACE)))
	{
		SDL_Quit();
		return 1;
	}

	while (!keypress)
	{
		DrawScreen(screen, h++);
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				keypress = 1;
				break;
			case SDL_KEYDOWN:
				keypress = 1;
				break;
			}
		}
	}

	SDL_Quit();

	return 0;
}





int main_window()
{
	SDL_Event e;
	SDL_Rect rect = { 10, 10, 50, 50 };
	SDL_Rect new_game = { 40, 120, 220, 60 };
	SDL_Rect load_game = { 40, 210, 220, 60 };
	SDL_Rect quit_game = { 40, 300, 220, 60 };
	SDL_Rect imgrect = { 0, 0, IMG_W, IMG_H };
	SDL_Surface *img = SDL_LoadBMP("test.bmp");
	SDL_Surface *w = SDL_SetVideoMode(WIN_W, WIN_H, 0, SDL_HWSURFACE | SDL_DOUBLEBUF);
	int quit = 0;

	/* Initialize SDL and make sure it quits*/
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("ERROR: unable to init SDL: %s\n", SDL_GetError());
		return 1;
	}
	atexit(SDL_Quit);

	/* Create window surface*/

	if (w == NULL) {
		printf("ERROR: failed to set video mode: %s\n", SDL_GetError());
		return 1;
	}

	/* Define the rects we need*/


	/* Load test spritesheet image*/

	if (img == NULL) {
		printf("ERROR: failed to load image: %s\n", SDL_GetError());
		return 1;
	}

	/* Set colorkey to BLUE*/
	if (SDL_SetColorKey(img, SDL_SRCCOLORKEY, SDL_MapRGB(img->format, 0, 0, 255)) != 0) {
		printf("ERROR: failed to set color key: %s\n", SDL_GetError());
		SDL_FreeSurface(img);
		return 1;
	}


	while (!quit) {
		/* Clear window to BLACK*/
		if (SDL_FillRect(w, 0, 0) != 0) {
			printf("ERROR: failed to draw rect: %s\n", SDL_GetError());
			break;
		}

		/* Green rectangle button*/
		if (SDL_FillRect(w, &new_game, SDL_MapRGB(w->format, 0, 255, 0)) != 0) {
			printf("ERROR: failed to draw rect: %s\n", SDL_GetError());
			break;
		}

		/* Green rectangle button*/
		if (SDL_FillRect(w, &load_game, SDL_MapRGB(w->format, 255, 0, 0)) != 0) {
			printf("ERROR: failed to draw rect: %s\n", SDL_GetError());
			break;
		}

		/* Green rectangle button*/
		if (SDL_FillRect(w, &quit_game, SDL_MapRGB(w->format, 0, 0, 255)) != 0) {
			printf("ERROR: failed to draw rect: %s\n", SDL_GetError());
			break;
		}

		/* Advance to next sprite*/
		imgrect.x += imgrect.w;
		if (imgrect.x >= img->w) {
			imgrect.x = 0;
			imgrect.y += imgrect.h;
			if (imgrect.y >= img->h) imgrect.y = 0;
		}

		/* We finished drawing*/
		if (SDL_Flip(w) != 0) {
			printf("ERROR: failed to flip buffer: %s\n", SDL_GetError());
			break;
		}

		/* Poll for keyboard & mouse events*/

		while (SDL_PollEvent(&e) != 0) {
			switch (e.type) {
			case (SDL_QUIT) :
				quit = 1;
				break;
			case (SDL_KEYUP) :
				if (e.key.keysym.sym == SDLK_ESCAPE) quit = 1;
				break;
			case (SDL_MOUSEBUTTONUP) :
				if ((e.button.x > rect.x) && (e.button.x < rect.x + rect.w) && (e.button.y > rect.y) && (e.button.y < rect.y + rect.h))
					quit = 1;
				break;
			default:
				break;
			}
		}

		/* Wait a little before redrawing*/
		SDL_Delay(1000);
	}

	SDL_FreeSurface(img);
	return 0;
}



