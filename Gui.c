#include "Gui.h"
#include <stdio.h>
#include <SDL.h>

#define WIDTH 640
#define HEIGHT 480
#define BPP 4
#define DEPTH 32

#define WIN_W 300
#define WIN_H 390
#define IMG_W 260
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


int test()
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
	SDL_Rect new_game = { 20, 120, 260, 54 };
	SDL_Rect load_game = { 20, 210, 260, 54 };
	SDL_Rect quit_game = { 20, 300, 260, 54 };
	SDL_Rect game_prog = { 10, 35, 280, 54 };
	SDL_Rect game_prog_rect = { 0, 0, 280, 54 };
	SDL_Rect imgrect = { 0, 0, IMG_W, IMG_H };

	SDL_Surface *new_img = SDL_LoadBMP("new_game.bmp");
	if (new_img == NULL) {
		printf("ERROR: failed to load image: %s\n", SDL_GetError());
		return 1;
	}
	SDL_Surface *load_img = SDL_LoadBMP("load_game.bmp");
	if (load_img == NULL) {
		printf("ERROR: failed to load image: %s\n", SDL_GetError());
		return 1;
	}
	SDL_Surface *quit_img = SDL_LoadBMP("quit.bmp");
	if (quit_img == NULL) {
		printf("ERROR: failed to load image: %s\n", SDL_GetError());
		return 1;
	}
	SDL_Surface *program_img = SDL_LoadBMP("game_prog.bmp");
	if (program_img == NULL) {
		printf("ERROR: failed to load image: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Surface *w = SDL_SetVideoMode(WIN_W, WIN_H, 0, SDL_HWSURFACE | SDL_DOUBLEBUF);
	if (w == NULL) {
		printf("ERROR: failed to set video mode: %s\n", SDL_GetError());
		return 1;
	}

	int quit = 0;

	/* Initialize SDL and make sure it quits*/
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("ERROR: unable to init SDL: %s\n", SDL_GetError());
		return 1;
	}
	atexit(SDL_Quit);



	/* Clear window to BLACK*/
	if (SDL_FillRect(w, 0, 0) != 0) {
		printf("ERROR: failed to draw rect: %s\n", SDL_GetError()); //todo - quit
	}

	/* White rectangle buttons */
	if (SDL_FillRect(w, &new_game, SDL_MapRGB(w->format, 255, 255, 255)) != 0) {
		printf("ERROR: failed to draw rect: %s\n", SDL_GetError());//todo - quit
	}
	if (SDL_FillRect(w, &load_game, SDL_MapRGB(w->format, 255, 255, 255)) != 0) {
		printf("ERROR: failed to draw rect: %s\n", SDL_GetError());//todo - quit
	}
	if (SDL_FillRect(w, &quit_game, SDL_MapRGB(w->format, 255, 255, 255)) != 0) {
		printf("ERROR: failed to draw rect: %s\n", SDL_GetError());//todo - quit
	}


	/* Draw image sprites*/
	if (SDL_BlitSurface(load_img, &imgrect, w, &load_game) != 0) {
		SDL_FreeSurface(load_img);
		printf("ERROR: failed to blit image: %s\n", SDL_GetError());//todo - quit
	}
	if (SDL_BlitSurface(new_img, &imgrect, w, &new_game) != 0) {
		SDL_FreeSurface(new_img);
		printf("ERROR: failed to blit image: %s\n", SDL_GetError());//todo - quit
	}
	if (SDL_BlitSurface(quit_img, &imgrect, w, &quit_game) != 0) {
		SDL_FreeSurface(quit_img);
		printf("ERROR: failed to blit image: %s\n", SDL_GetError());//todo - quit
	}
	if (SDL_BlitSurface(program_img, &game_prog_rect, w, &game_prog) != 0) {
		SDL_FreeSurface(quit_img);
		printf("ERROR: failed to blit image: %s\n", SDL_GetError());//todo - quit
	}

	while (!quit) {



		

		/* Advance to next sprite* /
		imgrect.x += imgrect.w;
		if (imgrect.x >= img->w) {
			imgrect.x = 0;
			imgrect.y += imgrect.h;
			if (imgrect.y >= img->h) imgrect.y = 0;
		}

		/* We finished drawing */
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
				if (is_in_rect(e.button.x, e.button.y, quit_game))
					quit = 1;
				if (is_in_rect(e.button.x, e.button.y, new_game))
					test();
				if (is_in_rect(e.button.x, e.button.y, load_game))
					test();
				break;
			default:
				break;
			}
		}

		/* Wait a little before redrawing*/
		SDL_Delay(1000);
	}

	/*SDL_FreeSurface(img);*/
	return 0;
}


/*return 1 if given x,y coordinates are in the rect. else return 0*/
int is_in_rect(int x, int y, SDL_Rect rect)
{
	if ((x > rect.x) && (x < rect.x + rect.w) && (y > rect.y) && (y < rect.y + rect.h))
		return 1;
	return 0;
}
