#define _CRT_SECURE_NO_DEPRECATE
#include "Gui.h"
#include "Game_flow.h"
#include "Chess_logic.h"
#include <stdio.h>
#include <SDL.h>


int show_moves_on = 0;
SDL_Rect current_board_tiles_marked[32];
int num_tiles_marked = 0;


/*draw the game window of the game*/
int game_window()
{
	SDL_Event e;
	int buttons_x = (int)(SQUERE_S * (0.25 + BOARD_SIZE) + 5);
	SDL_Rect imgrect = { 0, 0, GAME_IMG_W, GAME_IMG_H };

	SDL_Rect quit_game = { buttons_x, GAME_WIN_H - GAME_IMG_H - 10, GAME_IMG_W, GAME_IMG_H };
	SDL_Rect main_menu = { buttons_x, 30, GAME_IMG_W, GAME_IMG_H };
	SDL_Rect save_game = { buttons_x, (int)(30 + GAME_IMG_H * 1.5), GAME_IMG_W, GAME_IMG_H };
	SDL_Rect show_moves = { buttons_x, 30 + GAME_IMG_H * 3, GAME_IMG_W, GAME_IMG_H };
	SDL_Rect best_move = { buttons_x, (int)(30 + GAME_IMG_H * 4.5), GAME_IMG_W, GAME_IMG_H };

	int quit = 0;
	int redraw = 1;
	SDL_Surface *w = NULL;


	while (!quit) {
		if (!(player_vs_player == 1 || (player_vs_player == 0 && next_player == user_color)))
		{/*computer turn*/
			if (1 == do_computer_move(next_player))
			{/*game ended*/
				if (DEBUG)
				{
					getchar();
				}
			}
			next_player = flip_color(next_player);
		}
		if (redraw)
		{
			SDL_WM_SetCaption("Game Play", "Game Play");
			w = SDL_SetVideoMode(GAME_WIN_W, GAME_WIN_H, 0, SDL_HWSURFACE | SDL_DOUBLEBUF);
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
			draw_image(quit_game, imgrect, QUIT, w, 1);
			if (should_terminate)
			{
				SDL_FreeSurface(w);
				return 1;
			}
			draw_image(main_menu, imgrect, MAIN_MENU, w, 1);
			if (should_terminate)
			{
				SDL_FreeSurface(w);
				return 1;
			}
			draw_image(save_game, imgrect, SAVE_GAME, w, 1);
			if (should_terminate)
			{
				SDL_FreeSurface(w);
				return 1;
			}
			draw_image(show_moves, imgrect, SHOW_MOVES, w, 1);
			if (should_terminate)
			{
				SDL_FreeSurface(w);
				return 1;
			}
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
			redraw = 0;
		}


		/* Poll for keyboard & mouse events*/
		while (!quit && SDL_PollEvent(&e) != 0) {
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
				else if (is_in_rect(e.button.x, e.button.y, main_menu))
				{
					SDL_FreeSurface(w);
					quit = 1;
				}
				else if (is_in_rect(e.button.x, e.button.y, save_game))
				{
					SDL_FreeSurface(w);
					load_save_game_wind(0);
					if (should_terminate)
					{
						return 1;
					}
					redraw = 1;
				}
				else if (is_in_rect(e.button.x, e.button.y, best_move))
				{
					//todo
				}
				else if (is_in_rect(e.button.x, e.button.y, show_moves))
				{
					show_moves_on = !show_moves_on;
					int color = show_moves_on ? SDL_MapRGB(w->format, 0, 255, 0) : SDL_MapRGB(w->format, 255, 255, 255);
					SDL_Rect point = { show_moves.x + 4, show_moves.y + show_moves.h - 8, show_moves.w - 8, 4 };
					
					if (SDL_FillRect(w, &point, color) != 0){
						should_terminate = 1;
						printf("ERROR: failed to draw rect: %s\n", SDL_GetError());
						SDL_FreeSurface(w);
						return 1;
					}
					if (SDL_Flip(w) != 0) {
						should_terminate = 1;
						printf("ERROR: failed to flip buffer: %s\n", SDL_GetError());
						SDL_FreeSurface(w);
						return 1;
					}

					//todo - finish me
				}
				else
				{
					handle_board_press(e, w);
					if (should_terminate)
					{
						SDL_FreeSurface(w);
						return 1;
					}
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

/*handles an event of click in the chess board area*/
void handle_board_press(SDL_Event e,SDL_Surface *w)
{
	SDL_Rect board_rect = { (int)(SQUERE_S*0.25), SQUERE_S, SQUERE_S *BOARD_SIZE, SQUERE_S*BOARD_SIZE};
	if (!is_in_rect(e.button.x, e.button.y, board_rect))
		return; /*not inside the board*/

	int selected_col = get_tile_col(e.button.x);
	int selected_row = get_tile_row(e.button.y);
	board_tile selected_tile = board[selected_col][selected_row];
	if (selected_tile.color != next_player)
	{/*tile does not contain a piece in current player color*/
		if (num_tiles_marked == 0)
			return; 
		game_move move;
		move.start = board[get_tile_col(current_board_tiles_marked[0].x)][get_tile_row(current_board_tiles_marked[0].y)];
		move.end = board[selected_col][selected_row];

		int legal = is_legal_move(move, next_player);
		if (should_terminate)
		{
			return -1;
		}
		if (legal)
		{
			do_move(board, move);
		}

		return check_game_end(next_player);
		/*TODO */
	}

	/*unmark all previously marked tiles on the board*/
	for (int i = 0; i < num_tiles_marked; i++)
	{
		int color = (get_tile_row(current_board_tiles_marked[i].y) + get_tile_col(current_board_tiles_marked[i].x)) % 2 == 0 ?
			SDL_MapRGB(w->format, 255, 255, 255) : SDL_MapRGB(w->format, 75, 75, 75);
		paint_rect_edges(current_board_tiles_marked[i], w, color);
	}
	num_tiles_marked = 1;
	get_board_rect(selected_col, selected_row, &current_board_tiles_marked[0]);
	paint_rect_edges(current_board_tiles_marked[0], w, SDL_MapRGB(w->format, 0, 0, 255));
	if (show_moves_on)
	{
		//TODO - generate moves and color
	}

	if (SDL_Flip(w) != 0) {
		should_terminate = 1;
		printf("ERROR: failed to flip buffer: %s\n", SDL_GetError());
		return;
	}
}

/*returns the clonum of an x coordinate on the board*/
int get_tile_col(int x)
{
	return ((x - (int)(SQUERE_S*0.25)) / SQUERE_S);
}

/*returns the rownum of a y coordinate on the board*/
int get_tile_row(int y)
{
	return ((y - SQUERE_S) / SQUERE_S);
}

/*gets the rect on the board matching colnum and rownum*/
void get_board_rect(int colnum, int rownum, SDL_Rect *out_rect)
{
	out_rect->x = (int)(SQUERE_S * (0.25 + colnum));
	out_rect->y = SQUERE_S*(1 + rownum);
	out_rect->w = SQUERE_S;
	out_rect->h = SQUERE_S;
}

/*draws the current board as saved in the "board" variable to the surface w*/
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
			if (should_terminate)
			{
				should_terminate = 1;
				SDL_FreeSurface(symbols_img);
				return;
			}
			if (tool_rect != NULL)
			{
				SDL_Rect rect = { (int)(SQUERE_S * (0.25 + i)), SQUERE_S * (1 + j), SQUERE_S, SQUERE_S };
				int blit_response = SDL_BlitSurface(symbols_img, tool_rect, w, &rect);
				free(tool_rect);
				/* Draw image sprites*/
				if (blit_response != 0) {
					should_terminate = 1;
					printf("ERROR: failed to blit image: %s\n", SDL_GetError());
					SDL_FreeSurface(symbols_img);
					return;
				}
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

/*draw the load and save windows
returns -1 if the program should end, 1 if game was loaded, and 0 otherwise*/
int load_save_game_wind(int is_save)
{
	SDL_Event e;
	int quit = 0;

	/*number of button lines - we save one spot for the cancel button*/
	int num_lines = 1 + ((NUM_SAVE_SLOTS) / BTNS_PER_LINE); 
	SDL_Rect imgrect = { 0, 0, LOAD_BTN_W, LOAD_BTN_H };
	if (is_save)
	{
		SDL_WM_SetCaption("Save Game", "Save Game");

	}
	else
	{
		SDL_WM_SetCaption("Load Game", "Load Game");
	}

	/*surface for load\save window*/
	SDL_Surface *w = SDL_SetVideoMode(BTNS_PER_LINE * (LOAD_BTN_W + 20) + 20,
		num_lines * (LOAD_BTN_H + 20) + 50, 0, SDL_HWSURFACE | SDL_DOUBLEBUF);
	if (w == NULL) {
		should_terminate = 1;
		printf("ERROR: failed to set video mode: %s\n", SDL_GetError());
		return -1;
	}

	/* Clear window to BLACK*/
	if (SDL_FillRect(w, 0, 0) != 0) {
		should_terminate = 1;
		printf("ERROR: failed to draw rect: %s\n", SDL_GetError());
		SDL_FreeSurface(w);
		return -1;
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
			return -1;
		}
	}


	SDL_Rect cancel_rect = { 40 + (BTNS_PER_LINE - 1)*(LOAD_BTN_W + 20),
		55 + (LOAD_BTN_H + 20) * (num_lines - 1), LOAD_BTN_W, LOAD_BTN_H };

	/*add cancel button*/
	draw_image(cancel_rect, imgrect, CANCEL_IMG, w, 0);
	if (should_terminate)
	{
		SDL_FreeSurface(w);
		return -1;
	}


	/* We finished drawing */
	if (SDL_Flip(w) != 0) {
		should_terminate = 1;
		printf("ERROR: failed to flip buffer: %s\n", SDL_GetError());
		SDL_FreeSurface(w);
		return -1;
	}

	/*buttons events*/
	while (!quit) {
		while (!quit && SDL_PollEvent(&e) != 0) {
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
					/*pressed a slot button*/
					SDL_Rect r = { 20 + (slot_num % BTNS_PER_LINE)*(LOAD_BTN_W + 20),
						90 + (LOAD_BTN_H + 20) * (slot_num / BTNS_PER_LINE), LOAD_BTN_W, LOAD_BTN_H };
					if (is_in_rect(e.button.x, e.button.y, r))
					{
						if (save_load_game_from_slot(slot_num, is_save))
						{
							SDL_FreeSurface(w);
							return 1;
						}
					}
				}
				if (is_in_rect(e.button.x, e.button.y, cancel_rect))
				{
					SDL_FreeSurface(w);
					return 0;
				}
				 break;

			default:
				break;
			}

		}
	}
	SDL_FreeSurface(w);
	return -1;
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

/*if is_save=0, loads the game's configuration from the config file matching the given slot number
otherwise saves the game's current game to that file
returns 1 if game was saved\loaded*/
int save_load_game_from_slot(int slot_num, int is_save)
{
	char fname[2048]; /*the name of the config file*/
	if (!is_save && is_save_slot_free(slot_num))
	{
		/*pressed an "empty" button while trying to load. do nothing*/
		return 0;
	}
	if (is_save && !is_save_slot_free(slot_num))
	{
		/*pressed a "used" button while trying to save. do nothing*/
		return 0;
	}
	get_fname_from_slot_num(slot_num, fname);
	if (!is_save)
	{
		load_config(fname);
	}
	else
	{
		save_config(fname);
	}
	if (should_terminate)
		return 0;
	return 1;

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
	SDL_Surface *w = NULL;

	int redraw = 1; /*detarmines if the screen should be drawn again*/
	int quit = 0;

	while (!quit) {

		if (redraw)
		{
			SDL_WM_SetCaption("Main", "Main");
			w = SDL_SetVideoMode(MAIN_WIN_W, MAIN_WIN_H, 0, SDL_HWSURFACE | SDL_DOUBLEBUF);
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


			/*draw buttons*/
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
			redraw = 0;

		}
		/* Poll for keyboard & mouse events*/
		while (!quit && SDL_PollEvent(&e) != 0) {
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
					SDL_FreeSurface(w);
					if (game_window() || should_terminate)
					{
						return 1;
					}
					redraw = 1;
				}
				if (is_in_rect(e.button.x, e.button.y, load_game))
				{
					SDL_FreeSurface(w);
					int result = load_save_game_wind(0);
					if (result == -1 || should_terminate)
					{
						return 1;
					}
					if (result == 1)
					{
						if (game_window() || should_terminate)
						{
							return 1;
						}
					}
					redraw = 1;
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

/*patins the edges of rect in the surface w to the color color*/
void paint_rect_edges(SDL_Rect rect, SDL_Surface *w, int color)
{
	SDL_Rect edge1 = { rect.x, rect.y, 3, rect.h };
	if (SDL_FillRect(w, &edge1, color) != 0) {
		should_terminate = 1;
		printf("ERROR: failed to draw rect: %s\n", SDL_GetError());
		return;
	}
	
	SDL_Rect edge2 = { rect.x, rect.y, rect.w, 3 };
	if (SDL_FillRect(w, &edge2, color) != 0) {
		should_terminate = 1;
		printf("ERROR: failed to draw rect: %s\n", SDL_GetError());
		return;
	}
	SDL_Rect edge3 = { rect.x + rect.w - 3, rect.y, 3, rect.h };
	if (SDL_FillRect(w, &edge3, color) != 0) {
		should_terminate = 1;
		printf("ERROR: failed to draw rect: %s\n", SDL_GetError());
		return;
	}
	SDL_Rect edge4 = { rect.x, rect.y + rect.h - 3, rect.w, 3 };
	if (SDL_FillRect(w, &edge4, color) != 0) {
		should_terminate = 1;
		printf("ERROR: failed to draw rect: %s\n", SDL_GetError());
		return;
	}
}
