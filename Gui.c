#include "Gui.h"
#include "Game_flow.h"
#include <stdio.h>
#include <SDL.h>




int game_window()
{
	SDL_Event e;
	int quit = 0;
	SDL_Rect imgrect = { 0, 0, GAME_IMG_W, GAME_IMG_H };
	SDL_WM_SetCaption("Game Play", "Game Play");
	int buttons_x = (int)(SQUERE_S * (0.25 + BOARD_SIZE) + 5);
	SDL_Surface *w = SDL_SetVideoMode(GAME_WIN_W, GAME_WIN_H, 0, SDL_HWSURFACE | SDL_DOUBLEBUF);
	if (w == NULL) {
		should_terminate = 1;
		printf("ERROR: failed to set video mode: %s\n", SDL_GetError());
		return 1;
	}
	/* Clear window to BLACK*/
	if (SDL_FillRect(w, 0, 0) != 0) {
		should_terminate = 1;
		printf("ERROR: failed to draw rect: %s\n", SDL_GetError());
		SDL_FreeSurface(w);
		return 1;
	}

	/*draw initial board*/
	draw_current_board(w);
	if (should_terminate)
	{
		SDL_FreeSurface(w);
		return 1;
	}

	/*draw buttons*/
	SDL_Rect quit_game = { buttons_x, GAME_WIN_H - GAME_IMG_H - 10, GAME_IMG_W, GAME_IMG_H };
	draw_image(quit_game, imgrect, QUIT, w, 1);
	if (should_terminate)
	{
		SDL_FreeSurface(w);
		return 1;
	}

	SDL_Rect main_menu = { buttons_x, 30, GAME_IMG_W, GAME_IMG_H };
	draw_image(main_menu, imgrect, MAIN_MENU, w, 1);
	if (should_terminate)
	{
		SDL_FreeSurface(w);
		return 1;
	}

	SDL_Rect save_game = { buttons_x, (int)(30 + GAME_IMG_H * 1.5), GAME_IMG_W, GAME_IMG_H };
	draw_image(save_game, imgrect, SAVE_GAME, w, 1);
	if (should_terminate)
	{
		SDL_FreeSurface(w);
		return 1;
	}

	SDL_Rect show_moves = { buttons_x, 30 + GAME_IMG_H * 3, GAME_IMG_W, GAME_IMG_H };
	draw_image(show_moves, imgrect, SHOW_MOVES, w, 1);
	if (should_terminate)
	{
		SDL_FreeSurface(w);
		return 1;
	}

	SDL_Rect best_move = { buttons_x, (int)(30 + GAME_IMG_H * 4.5), GAME_IMG_W, GAME_IMG_H };
	draw_image(best_move, imgrect, BEST_MOVE, w, 1);
	if (should_terminate)
	{
		SDL_FreeSurface(w);
		return 1;
	}
	
	/* We finished drawing */
	if (SDL_Flip(w) != 0) {
		should_terminate = 1;
		printf("ERROR: failed to flip buffer: %s\n", SDL_GetError());
		SDL_FreeSurface(w);
		return 1;
	}

	while (!quit) {

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
				break;
			default:
				break;
			}
		}

		/* Wait a little before redrawing*/
		SDL_Delay(100);
	}

	return 0;
}
void draw_current_board(SDL_Surface *w)
{
	SDL_Surface *symbols_img = SDL_LoadBMP(SYMBOLS);
	if (symbols_img == NULL) {
		should_terminate = 1;
		printf("ERROR: failed to load image: %s\n", SDL_GetError());
		return;
	}
	/*paint board*/
	for (int i = 0; i < BOARD_SIZE; ++i)
	{
		for (int j = 0; j < BOARD_SIZE; ++j)
		{
			/*one tile*/
			int color = (i + j) % 2 == 0 ? SDL_MapRGB(w->format, 255, 255, 255) : SDL_MapRGB(w->format, 75, 75, 75);
			SDL_Rect rect = { (int)(SQUERE_S * (0.25 + i)), SQUERE_S*(1 + j), SQUERE_S, SQUERE_S };
			if (SDL_FillRect(w, &rect, color) != 0) {
				should_terminate = 1;
				printf("ERROR: failed to draw rect: %s\n", SDL_GetError());
				SDL_FreeSurface(symbols_img);
				return;
			}

		}
	}
	/*paint tools images*/
	Uint32 green = SDL_MapRGB(symbols_img->format, 0, 255, 0);
	SDL_SetColorKey(symbols_img, SDL_SRCCOLORKEY, green);
	for (int i = 0; i < BOARD_SIZE; ++i)
	{
		for (int j = 0; j < BOARD_SIZE; ++j)
		{
			SDL_Rect *tool_rect = get_tool_rect(board[i][j]);
			SDL_Rect rect = { (int)(SQUERE_S * (0.25 + i)), SQUERE_S * (1 + j), SQUERE_S, SQUERE_S };
			if (should_terminate)
			{
				should_terminate = 1;
				SDL_FreeSurface(symbols_img);
				return;
			}
			if (tool_rect != NULL)
			{
				/* Draw image sprites*/
				if (SDL_BlitSurface(symbols_img, tool_rect, w, &rect) != 0) {
					should_terminate = 1;
					printf("ERROR: failed to blit image: %s\n", SDL_GetError());
					SDL_FreeSurface(symbols_img);
					free(tool_rect);
					return;
				}
				free(tool_rect);
			}
		}
	}
	SDL_FreeSurface(symbols_img);
}
/*returns a rect in the SYMBOLS image matching the given tool
returns null if given tool is EMPTY*/
SDL_Rect *get_tool_rect(board_tile tool)
{
	int rect_y = 0;
	int rect_x = 10;
	switch (tool.color)
	{
	case EMPTY:
		return NULL;
	case BLACK:
		rect_y += SQUERE_S;
		break;
	}
	switch (tool.type)
	{
	case WHITE_Q:
		rect_x += SQUERE_S;
		break;
	case WHITE_R:
		rect_x += SQUERE_S * 2;
		break;
	case WHITE_B:
		rect_x += SQUERE_S * 3;
		break;
	case WHITE_N:
		rect_x += SQUERE_S * 4;
		break;
	case WHITE_P:
		rect_x += SQUERE_S * 5;
		break;
	}
	SDL_Rect *out = malloc(sizeof(SDL_Rect));
	if (NULL == out)
	{
		perror_message("malloc");
		should_terminate = 1;
	}
	out->h = SQUERE_S;
	out->w = SQUERE_S;
	out->x = rect_x;
	out->y = rect_y;
	return out;
}

/*draw the load window*/
int load_game_wind()
{
	SDL_Event e;
	int quit = 0;

	/*number of button lines - we save one spot for the cancel button*/
	int num_lines = 1 + ((NUM_SAVE_SLOTS) / BTNS_PER_LINE); 
	SDL_Rect imgrect = { 0, 0, LOAD_BTN_W, LOAD_BTN_H };

	SDL_WM_SetCaption("Load Game", "Load Game");
	/*surface for load window*/
	SDL_Surface *w = SDL_SetVideoMode(BTNS_PER_LINE * (LOAD_BTN_W + 20) + 20,
		num_lines * (LOAD_BTN_H + 20) + 50, 0, SDL_HWSURFACE | SDL_DOUBLEBUF);
	if (w == NULL) {
		should_terminate = 1;
		printf("ERROR: failed to set video mode: %s\n", SDL_GetError());
		return 1;
	}

	/* Clear window to BLACK*/
	if (SDL_FillRect(w, 0, 0) != 0) {
		should_terminate = 1;
		printf("ERROR: failed to draw rect: %s\n", SDL_GetError());
		SDL_FreeSurface(w);
		return 1;
	}

	/*draw all buttons*/
	for (int slot_num = 0; slot_num < NUM_SAVE_SLOTS; slot_num++)
	{
		SDL_Rect r = { 20 + (slot_num % BTNS_PER_LINE)*(LOAD_BTN_W + 20), 
			40 + (LOAD_BTN_H + 20) * (slot_num / BTNS_PER_LINE), LOAD_BTN_W, LOAD_BTN_H };

		/*show EMPTY button if the current slot is free. otherwise show USED button*/
		char * image = is_save_slot_free(slot_num) ? EMPTY_IMG : USED_IMG;

		draw_image(r, imgrect, image, w, 0);
		if (should_terminate)
		{
			SDL_FreeSurface(w);
			return 1;
		}
	}


	SDL_Rect cancel_rect = { 40 + (BTNS_PER_LINE - 1)*(LOAD_BTN_W + 20),
		55 + (LOAD_BTN_H + 20) * (num_lines - 1), LOAD_BTN_W, LOAD_BTN_H };

	/*add cancel button*/
	draw_image(cancel_rect, imgrect, CANCEL_IMG, w, 0);
	if (should_terminate)
	{
		SDL_FreeSurface(w);
		return 1;
	}


	/* We finished drawing */
	if (SDL_Flip(w) != 0) {
		should_terminate = 1;
		printf("ERROR: failed to flip buffer: %s\n", SDL_GetError());
		SDL_FreeSurface(w);
		return 1;
	}

	/*buttons events*/
	while (!quit) {
		while (SDL_PollEvent(&e) != 0) {
			switch (e.type) {
			case (SDL_QUIT) :
				quit = 1;
				break;
			case (SDL_KEYUP) :
				if (e.key.keysym.sym == SDLK_ESCAPE)
					quit = 1;
				break;
			case (SDL_MOUSEBUTTONUP) :
				for (int slot_num = 0; slot_num < NUM_SAVE_SLOTS; slot_num++)
				{
					SDL_Rect r = { 20 + (slot_num % BTNS_PER_LINE)*(LOAD_BTN_W + 20),
						90 + (LOAD_BTN_H + 20) * (slot_num / BTNS_PER_LINE), LOAD_BTN_W, LOAD_BTN_H };
					if (is_in_rect(e.button.x, e.button.y, r))
					{
						if (!is_save_slot_free(slot_num))
						{
							load_game_from_slot(slot_num);
							quit = 1;
						}
					}
				}
				if (is_in_rect(e.button.x, e.button.y, cancel_rect))
				{
					main_window();
					quit = 1;
				}
				 break;

			default:
				break;
			}
		}
	}
	SDL_FreeSurface(w);
	return 0;
}

/*gets a slot, and returns the name of the matching file.
fname is the output buffer to put the file name in*/
char* get_fname_from_slot_num(int slot_num, char *fname)
{
	fname[0] = '\0';
	strcat(fname, SAVE_FILES_PATH);
	char slot_ascii[1024];
	_itoa(slot_num, slot_ascii, 10);
	strcat(fname, slot_ascii);
	return fname;
}

/*returns 1 if game slot is free (file matching slot_num does not exist). else returns 0*/
int is_save_slot_free(int slot_num)
{
	char fname[2048];
	get_fname_from_slot_num(slot_num, fname);
	int file_exists = 0;
	FILE * f = fopen(fname, "rb");
	if (f != NULL)
	{
		/*file exist*/
		fclose(f);
		return 0;
	}
	else
		return 1;
}

/*load the game's configuration from the config file matching the given slot number*/
void load_game_from_slot(int slot_num)
{
	char fname[2048]; /*the name of the config file*/
	if (is_save_slot_free(slot_num))
	{
		/*pressed a "free" button. do nothing*/
		return;
	}
	/*get the config file data*/
	get_fname_from_slot_num(slot_num, fname);
	FILE * f = fopen(fname, "rb");
	fseek(f, 0, SEEK_END);
	int file_len = ftell(f);
	char *file_data = malloc(file_len);
	fclose(f);
	if (file_data == NULL)
	{
		should_terminate = 1;
		perror_message("malloc");
		return;
	}
	/*use the data to load the configuration*/
	load_config(file_data);
	free(file_data);
	if (should_terminate)
		return;

}

/*draw the main window of the game*/
int main_window()
{
	SDL_Event e;
	SDL_Rect game_prog = { 10, 35, 280, MAIN_IMG_H };
	SDL_Rect new_game = { 20, 120, MAIN_IMG_W, MAIN_IMG_H };
	SDL_Rect load_game = { 20, 210, MAIN_IMG_W, MAIN_IMG_H };
	SDL_Rect quit_game = { 20, 300, MAIN_IMG_W, MAIN_IMG_H };
	SDL_Rect game_prog_rect = { 0, 0, 280, MAIN_IMG_H };
	SDL_Rect imgrect = { 0, 0, MAIN_IMG_W, MAIN_IMG_H };

	SDL_WM_SetCaption("Main", "Main");
	SDL_Surface *w = SDL_SetVideoMode(MAIN_WIN_W, MAIN_WIN_H, 0, SDL_HWSURFACE | SDL_DOUBLEBUF);
	if (w == NULL) {
		should_terminate = 1;
		printf("ERROR: failed to set video mode: %s\n", SDL_GetError());
		return 1;
	}

	/* Clear window to BLACK*/
	if (SDL_FillRect(w, 0, 0) != 0) {
		should_terminate = 1;
		printf("ERROR: failed to draw rect: %s\n", SDL_GetError());
		SDL_FreeSurface(w);
		return 1;
	}


	draw_image(new_game, imgrect, NEW_GAME, w, 1);
	if (should_terminate)
	{
		SDL_FreeSurface(w);
		return 1;
	}

	draw_image(load_game, imgrect, LOAD_GAME, w, 1);
	if (should_terminate)
	{
		SDL_FreeSurface(w);
		return 1;
	}

	draw_image(quit_game, imgrect, QUIT, w, 1);
	if (should_terminate)
	{
		SDL_FreeSurface(w);
		return 1;
	}


	draw_image(game_prog, game_prog_rect, GAME_PROG, w, 0);
	if (should_terminate)
	{
		SDL_FreeSurface(w);
		return 1;
	}



	/* We finished drawing */
	if (SDL_Flip(w) != 0) {
		should_terminate = 1;
		printf("ERROR: failed to flip buffer: %s\n", SDL_GetError());
		SDL_FreeSurface(w);
		return 1;
	}

	int quit = 0;
	while (!quit) {

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
				{
					game_window();
				}
				if (is_in_rect(e.button.x, e.button.y, load_game))
				{
					load_game_wind();
					quit = 1;
				}
				break;
			default:
				break;
			}
		}

		/* Wait a little before redrawing*/
		SDL_Delay(100);
	}

	SDL_FreeSurface(w);
	return 0;
}

/*draws the img_rect rectangel of the image in the path img_path.
the img will be drawn on the surface w in the rect img_place
should fill - if TRUE, this func will fill the whole rect with white before drawing the image*/
void draw_image(SDL_Rect img_rect, SDL_Rect img_place, char* img_path, SDL_Surface* w, int should_fill)
{
	if (should_fill)
	{
		/* Whiten rectangle */
		if (SDL_FillRect(w, &img_rect, SDL_MapRGB(w->format, 255, 255, 255)) != 0) {
			should_terminate = 1;
			printf("ERROR: failed to draw rect: %s\n", SDL_GetError());
			return;
		}
	}

	/*load image*/
	SDL_Surface *img = SDL_LoadBMP(img_path);
	if (img == NULL) {
		should_terminate = 1;
		printf("ERROR: failed to load image: %s\n", SDL_GetError());
		return;
	}

	int blit_response = SDL_BlitSurface(img, &img_place, w, &img_rect);
	SDL_FreeSurface(img);
	/* Draw image sprites*/
	if (blit_response != 0) {
		should_terminate = 1;
		printf("ERROR: failed to blit image: %s\n", SDL_GetError());
		return;
	}
}

/*return 1 if given x,y coordinates are in the rect. else return 0*/
int is_in_rect(int x, int y, SDL_Rect rect)
{
	if ((x > rect.x) && (x < rect.x + rect.w) && (y > rect.y) && (y < rect.y + rect.h))
		return 1;
	return 0;
}
