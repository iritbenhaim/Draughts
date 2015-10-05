#define _CRT_SECURE_NO_DEPRECATE
#include "Gui.h"
#include "Game_flow.h"
#include "Minimax.h"
#include "Chess_logic.h"
#include <stdio.h>
#include <SDL.h>


int show_moves_on = 0;
SDL_Rect current_board_tiles_marked[32];
int num_tiles_marked = 0;
int use_fancy_tools = 1;


/*draw the game window of the game*/
int game_window()
{
	char *images[GAME_WIND_BUTTONS] = { MAIN_MENU, SAVE_GAME, SHOW_MOVES, BEST_MOVE, FANCY, QUIT };
	num_tiles_marked = 0; /*if a previous window marked a tile, we dont care*/

	SDL_Event e;
	int buttons_x = (int)(SQUERE_S * (0.25 + BOARD_SIZE) + 5);
	int buttons_y = SQUERE_S;
	SDL_Rect imgrect = { 0, 0, GAME_IMG_W, GAME_IMG_H };

	int button_count = 6;
	SDL_Rect top_button = { buttons_x, buttons_y, GAME_IMG_W, GAME_IMG_H };

	int quit = 0;
	int redraw = 1;/*when this is 1, will paint the screen from scratch*/
	SDL_Surface *w = NULL;

	char *end_game_img = NULL; /*img for "check" "mate" or "tie" pictures. if null, no pucture is there*/
	SDL_Rect end_game = { SQUERE_S * 3, 0, 150, 45 };
	SDL_Rect end_game_place = { 0, 0, end_game.w, end_game.h };

	while (!quit)
	{
		if ((player_vs_player != 1 && (player_vs_player == 0 || next_player != user_color)))
		{/*computer turn*/
			SDL_Delay(300);
			if (1 == do_computer_move(next_player))
			{/*game ended*/
				if (DEBUG)
				{
					getchar();
				}
			}
			next_player = flip_color(next_player);
			redraw = 1;
		}
		else
		{/*user turn*/
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
				paint_all_buttons(w, images, GAME_WIND_BUTTONS, top_button, imgrect);
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

			int button_num;
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
					button_num = get_num_button_pressed(e.button.x, e.button.y, top_button, GAME_WIND_BUTTONS);
					if (button_num != -1)
					{
						switch (button_num) {
						case 0: /*main menu*/
							SDL_FreeSurface(w);
							quit = 1;
							break;
						case 1: /*save game*/
							SDL_FreeSurface(w);
							load_save_game_wind(1);
							if (should_terminate)
							{
								return 1;
							}
							redraw = 1;
							break;
						case 2: /*show moves*/
							/*turn show_moves on\off*/
							show_moves_on = !show_moves_on;

							/*underline the show moves button*/
							int color = show_moves_on ? SDL_MapRGB(w->format, 0, 255, 0) : SDL_MapRGB(w->format, 255, 255, 255);
							SDL_Rect show_moves = { buttons_x, buttons_y + GAME_IMG_H * (button_num + 1), GAME_IMG_W, GAME_IMG_H };
							SDL_Rect underline = { show_moves.x + 4, show_moves.y + show_moves.h - 8, show_moves.w - 8, 4 };
							if (SDL_FillRect(w, &underline, color) != 0){
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
							break;
						case 3: /*best move*/
							/*unmark all previously marked tiles on the board*/
							for (int i = 0; i < num_tiles_marked; i++)
							{
								int color = (get_tile_row(current_board_tiles_marked[i].y) + get_tile_col(current_board_tiles_marked[i].x)) % 2 != 0 ?
									SDL_MapRGB(w->format, 255, 255, 255) : SDL_MapRGB(w->format, 75, 75, 75);
								paint_rect_edges(current_board_tiles_marked[i], w, color);
								if (should_terminate)
								{
									SDL_FreeSurface(w);
									return 1;
								}
							}
							num_tiles_marked = 2;

							linked_list moves_list = get_best_moves(board, next_player, minimax_depth);
							game_move move = *((game_move*)moves_list.first->data);
							get_board_rect(move.start.char_indexer, move.start.int_indexer, &current_board_tiles_marked[0]);
							get_board_rect(move.end.char_indexer, move.end.int_indexer, &current_board_tiles_marked[1]);
							free_moves(moves_list);
							/*mark start tile*/
							paint_rect_edges(current_board_tiles_marked[0], w, SDL_MapRGB(w->format, 0, 0, 255));
							if (should_terminate)
							{
								SDL_FreeSurface(w);
								return 1;
							}
							/*mark end tile*/
							paint_rect_edges(current_board_tiles_marked[1], w, SDL_MapRGB(w->format, 255, 0, 0));
							if (should_terminate)
							{
								SDL_FreeSurface(w);
								return 1;
							}
							if (SDL_Flip(w) != 0) {
								should_terminate = 1;
								printf("ERROR: failed to flip buffer: %s\n", SDL_GetError());
								SDL_FreeSurface(w);
								return 1;
							}
							break;
						case 4: /*fancy*/
							use_fancy_tools = 1 - use_fancy_tools;
							redraw = 1;
							break;
						case 5: /*quit game*/
							SDL_FreeSurface(w);
							return 1;
						}
					}
					else
					{ /*no button was pressed*/
						int press_resault = handle_board_press(e, w);
						if (should_terminate)
						{
							SDL_FreeSurface(w);
							return 1;
						}
						if (press_resault == 1)
						{/*game ended*/
							/*todo - handle end game*/
							SDL_FreeSurface(w);
							if (DEBUG)
							{
								getchar();
							}
							return 1;
						}
						else if (press_resault == 0)
						{
							next_player = flip_color(next_player);
						}
					}
					break;
				default:
					break;
				}
			}
		}

		/*handle endgame*/
		if (mate || check || tie || end_game_img != NULL)
		{
			if ((!mate && !check && !tie) || ((end_game_img != NULL) && (strcmp(end_game_img, CHECK) && (mate || tie))))
			{ /*no end_game anymore, or endgame moved from check to mate or tie. clear endgame rect*/
				end_game_img = NULL;
				if (SDL_FillRect(w, &end_game, 0) != 0) {
					should_terminate = 1;
					printf("ERROR: failed to draw rect: %s\n", SDL_GetError());
					SDL_FreeSurface(w);
					return 1;
				}
			}
			if (mate)
			{
				end_game_img = MATE;
			}
			else if (tie)
			{
				end_game_img = END_TIE;
			}
			else if (check)
			{
				end_game_img = CHECK;
			}
			if (end_game_img != NULL)
			{
				draw_image(end_game, end_game_place, end_game_img, w, 1);
				if (should_terminate)
				{
					SDL_FreeSurface(w);
					return 1;
				}
			}
			if (SDL_Flip(w) != 0) {
				should_terminate = 1;
				printf("ERROR: failed to flip buffer: %s\n", SDL_GetError());
				SDL_FreeSurface(w);
				return 1;
			}
		}
	}
	SDL_FreeSurface(w);
	return 0;
}

/*returns the number of button pressed (zero base) if no button was pressed, returns -1*/
int get_num_button_pressed(int x, int y, SDL_Rect top_button, int button_count)
{
	for (int i = 0; i < button_count; ++i)
	{
		if (is_in_rect(x, y, top_button))
		{
			return i;
		}
		top_button.y += (int)(top_button.h * 1.5);
	}
	return -1;
}

/*paints the buttons of a single window.
goes other the images in images list (img_count is the size of that list) and paints them
the first button will be in top_button place*/
void paint_all_buttons(SDL_Surface *w, char ** images, int img_count, SDL_Rect top_button, SDL_Rect imgrect)
{
	for (int i = 0; i < img_count; i++)
	{
		char * img = images[i];
		draw_image(top_button, imgrect, img, w, 1);
		if (should_terminate)
		{
			return;
		}
		top_button.y += (int)(top_button.h * 1.5);

	}
}

/*handles an event of click in the chess board area
returns 1 if game ended, 0 if user turn ended, and 2 otherwise*/
int handle_board_press(SDL_Event e,SDL_Surface *w)
{
	SDL_Rect board_rect = { (int)(SQUERE_S*0.25), SQUERE_S, SQUERE_S *BOARD_SIZE, SQUERE_S*BOARD_SIZE};
	if (!is_in_rect(e.button.x, e.button.y, board_rect))
		return 2; /*not inside the board*/

	int selected_col = get_tile_col(e.button.x);
	int selected_row = get_tile_row(e.button.y);
	board_tile selected_tile = board[selected_col][selected_row];
	if (selected_tile.color != next_player || selected_tile.type == EMPTY)
	{/*tile does not contain a piece in current player color*/
		if (num_tiles_marked == 0)
			return 2; 
		/*check if the move from current_board_tiles_marked[0] to the tile pressed is legal. if so, do it*/
		game_move move;
		move.start = board[get_tile_col(current_board_tiles_marked[0].x)][get_tile_row(current_board_tiles_marked[0].y)];
		move.end = board[selected_col][selected_row];
		move.promote = WHITE_Q; /*defult promotion to pass is_legal check*/
		int legal = is_legal_move(move, next_player);
		if (should_terminate || !legal)
		{
			return 2;
		}
		/*do promotion*/
		if (promotion(move.start) == 1)
		{	
			move.promote = do_pawn_promotion(w);
			if (should_terminate)
			{
				return 2;
			}
		}
		do_move(board, move);
		flip_color(next_player);
		draw_current_board(w);
		if (should_terminate)
			return 2;
		if (SDL_Flip(w) != 0) {
			should_terminate = 1;
			printf("ERROR: failed to flip buffer: %s\n", SDL_GetError());
			return 2;
		}

		return check_game_end(next_player);
	}

	/*unmark all previously marked tiles on the board*/
	for (int i = 0; i < num_tiles_marked; i++)
	{
		int color = (get_tile_row(current_board_tiles_marked[i].y) + get_tile_col(current_board_tiles_marked[i].x)) % 2 != 0 ?
			SDL_MapRGB(w->format, 255, 255, 255) : SDL_MapRGB(w->format, 75, 75, 75);
		paint_rect_edges(current_board_tiles_marked[i], w, color);
		if (should_terminate)
		{
			return 2;
		}
	}
	num_tiles_marked = 1;
	get_board_rect(selected_col, selected_row, &current_board_tiles_marked[0]);
	/*mark selected tile*/
	paint_rect_edges(current_board_tiles_marked[0], w, SDL_MapRGB(w->format, 0, 0, 255));
	if (should_terminate)
	{
		return 2;
	}
	if (show_moves_on)
	{
		/*mark all tiles possible for current move*/
		linked_list l = new_list();
		generate_piece_moves(board, board[selected_col][selected_row],&l);
		if (should_terminate)
		{
			free_moves(l);
			return 2;
		}
		filter_moves_with_check(board, &l, next_player);
		if (should_terminate)
		{
			free_moves(l);
			return 2;
		}
		node* n = l.first;
		for (int i=0; i < l.len ; i++, n=n->next)
		{
			board_tile cur_move_end = ((game_move*)n->data)->end;
			get_board_rect(cur_move_end.char_indexer, cur_move_end.int_indexer, &current_board_tiles_marked[num_tiles_marked]);
			paint_rect_edges(current_board_tiles_marked[num_tiles_marked], w, SDL_MapRGB(w->format, 255, 0, 0));
			if (should_terminate)
			{
				return 2;
			}
			num_tiles_marked++;
		}
	}

	if (SDL_Flip(w) != 0) {
		should_terminate = 1;
		printf("ERROR: failed to flip buffer: %s\n", SDL_GetError());
		return 2;
	}
	return 2;
}

/*handles pawn promotion.
returns the char indicating the chosen piece type to promote to.
returns EMPTY on error*/
char do_pawn_promotion(SDL_Surface *w)
{
	SDL_Event e;
	int quit = 0;

	int y_coordinate = next_player == WHITE ? 0 : SQUERE_S *(BOARD_SIZE + 1);
	SDL_Rect all_place = { SQUERE_S * 3, y_coordinate, SQUERE_S * 4, SQUERE_S };

	paint_boarder_pieces(1, next_player, next_player == WHITE, w);
	if (should_terminate)
		return 0;

	if (SDL_Flip(w) != 0) {
		should_terminate = 1;
		printf("ERROR: failed to flip buffer: %s\n", SDL_GetError());
		return 0;
	}

	char choice = EMPTY;
	/*buttons events*/
	while (!quit)
	{
		while (!quit && SDL_PollEvent(&e) != 0)
		{
			switch (e.type) 
			{
			case (SDL_QUIT) :
				quit = 1;
				break;
			case (SDL_KEYUP) :
				if (e.key.keysym.sym == SDLK_ESCAPE)
					quit = 1;
				break;
			case (SDL_MOUSEBUTTONUP) :
				choice = get_boarder_pieces_choice(e.button.x, e.button.y, 1, next_player == WHITE);
				if (choice != '\0')
					quit = 1;
			}
		}
	}

	/* Clear back to BLACK - remove promotion options*/
	if (SDL_FillRect(w, &all_place, 0) != 0) {
		should_terminate = 1;
		printf("ERROR: failed to draw rect: %s\n", SDL_GetError());
		return 1;
	}
	return choice;
}

/*checks if the given x and y coordinates are inside a boarder piece rectangle.
if so, returns the matching piece. otherwise returns '\0'
if is promotion is 1, will only paint promoting pieces. otherwise paints all pieces
is top - if 1, will put the pieces on the top otherwise on bottom*/
char get_boarder_pieces_choice(int x, int y, int is_promotion, int is_top)
{

	int y_coordinate = is_top ? 0 : SQUERE_S * ( BOARD_SIZE + 1);
	int start_piece_offset = is_promotion ? SQUERE_S * 3 : SQUERE_S;
	SDL_Rect piece_place = { start_piece_offset, y_coordinate, SQUERE_S, SQUERE_S };
	char out_piece = '\0';
	if (!is_promotion)
	{
		if (is_in_rect(x, y, piece_place))
		{
			out_piece = EMPTY;
		}
		piece_place.x += SQUERE_S;
		if (is_in_rect(x, y, piece_place))
		{
			out_piece = WHITE_K;
		}
		piece_place.x += SQUERE_S;
	}

	if (is_in_rect(x, y, piece_place))
	{
		out_piece = WHITE_Q;
	}
	piece_place.x += SQUERE_S;
	if (is_in_rect(x, y, piece_place))
	{
		out_piece = WHITE_R;
	}
	piece_place.x += SQUERE_S;
	if (is_in_rect(x, y, piece_place))
	{
		out_piece = WHITE_B;
	}
	piece_place.x += SQUERE_S;
	if (is_in_rect(x, y, piece_place))
	{
		out_piece = WHITE_N;
	}
	if (!is_promotion)
	{
		piece_place.x += SQUERE_S;
		if (is_in_rect(x, y, piece_place))
		{
			out_piece = WHITE_P;
		}
	}
	return out_piece;
}

/*paints pieces outside the boarders of the board. used for pawn promotion and board init
if is promotion is 1, will only paint promoting pieces. otherwise paints all pieces
color detarmines the color of the pieces
is top - if 1, will put the pieces on the top otherwise on bottom*/
void paint_boarder_pieces(int is_promotion, char color, int is_top, SDL_Surface *w)
{
	SDL_Surface *symbols_img = SDL_LoadBMP(SYMBOLS);
	if (symbols_img == NULL) {
		should_terminate = 1;
		printf("ERROR: failed to load image: %s\n", SDL_GetError());
		return;
	}

	/*paint tools images*/
	Uint32 green = SDL_MapRGB(symbols_img->format, 0, 255, 0);
	SDL_SetColorKey(symbols_img, SDL_SRCCOLORKEY, green);

	int y_coordinate = is_top ? 0 : SQUERE_S * (BOARD_SIZE + 1);
	int start_piece_offset = is_promotion ? SQUERE_S * 3 : SQUERE_S;
	int num_pieces = is_promotion ? 4 : 7;
	SDL_Rect all_place = { start_piece_offset, y_coordinate, SQUERE_S * num_pieces, SQUERE_S };
	SDL_Rect piece_place = { start_piece_offset, y_coordinate, SQUERE_S, SQUERE_S };


	/* Whiten rectangle */
	if (SDL_FillRect(w, &all_place, SDL_MapRGB(w->format, 150, 150, 150)) != 0) {
		should_terminate = 1;
		printf("ERROR: failed to draw rect: %s\n", SDL_GetError());
		return;
	}

	board_tile tile;
	tile.color = color;
	if (!is_promotion)
	{
		/*first tile is empty for putting empty tiles*/
		piece_place.x += SQUERE_S;
		tile.type = WHITE_K;
		SDL_Rect tool_rect;
		get_tool_rect(tile, &tool_rect);
		if (SDL_BlitSurface(symbols_img, &tool_rect, w, &piece_place) != 0) {
			should_terminate = 1;
			printf("ERROR: failed to blit image: %s\n", SDL_GetError());
			SDL_FreeSurface(symbols_img);
			return;
		}
		piece_place.x += SQUERE_S;
	}

	/*paint the options for promotion*/
	tile.type = WHITE_Q;
	SDL_Rect tool_rect;
	get_tool_rect(tile, &tool_rect);
	if (SDL_BlitSurface(symbols_img, &tool_rect, w, &piece_place) != 0) {
		should_terminate = 1;
		printf("ERROR: failed to blit image: %s\n", SDL_GetError());
		SDL_FreeSurface(symbols_img);
		return;
	}
	piece_place.x += SQUERE_S;
	tile.type = WHITE_R;
	get_tool_rect(tile, &tool_rect);
	if (SDL_BlitSurface(symbols_img, &tool_rect, w, &piece_place) != 0) {
		should_terminate = 1;
		printf("ERROR: failed to blit image: %s\n", SDL_GetError());
		SDL_FreeSurface(symbols_img);
		return;
	}
	piece_place.x += SQUERE_S;
	tile.type = WHITE_B;
	get_tool_rect(tile, &tool_rect);
	if (SDL_BlitSurface(symbols_img, &tool_rect, w, &piece_place) != 0) {
		should_terminate = 1;
		printf("ERROR: failed to blit image: %s\n", SDL_GetError());
		SDL_FreeSurface(symbols_img);
		return;
	}
	piece_place.x += SQUERE_S;
	tile.type = WHITE_N;
	get_tool_rect(tile, &tool_rect);
	if (SDL_BlitSurface(symbols_img, &tool_rect, w, &piece_place) != 0) {
		should_terminate = 1;
		printf("ERROR: failed to blit image: %s\n", SDL_GetError());
		SDL_FreeSurface(symbols_img);
		return;
	}
	if (!is_promotion)
	{
		piece_place.x += SQUERE_S;
		tile.type = WHITE_P;
		SDL_Rect tool_rect;
		get_tool_rect(tile, &tool_rect);
		if (SDL_BlitSurface(symbols_img, &tool_rect, w, &piece_place) != 0) {
			should_terminate = 1;
			printf("ERROR: failed to blit image: %s\n", SDL_GetError());
			SDL_FreeSurface(symbols_img);
			return;
		}
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
	return BOARD_SIZE - 1 - ((y - SQUERE_S) / SQUERE_S);
}

/*gets the rect on the board matching colnum and rownum*/
void get_board_rect(int colnum, int rownum, SDL_Rect *out_rect)
{
	out_rect->x = (int)(SQUERE_S * (0.25 + colnum));
	out_rect->y = SQUERE_S*(BOARD_SIZE - rownum);
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
			/*paint one tile*/
			int color = (i + j) % 2 != 0 ? SDL_MapRGB(w->format, 255, 255, 255) : SDL_MapRGB(w->format, 75, 75, 75);
			SDL_Rect rect;
			get_board_rect(i, j, &rect);
			if (SDL_FillRect(w, &rect, color) != 0)
			{
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
			if (board[i][j].color == EMPTY || board[i][j].type == EMPTY)
				continue;
			SDL_Rect tool_rect;
			get_tool_rect(board[i][j], &tool_rect);

			SDL_Rect rect;
			get_board_rect(i, j, &rect);
			/* Draw image sprites*/
			if (SDL_BlitSurface(symbols_img, &tool_rect, w, &rect) != 0)
			{
				should_terminate = 1;
				printf("ERROR: failed to blit image: %s\n", SDL_GetError());
				SDL_FreeSurface(symbols_img);
				return;
			}
			
		}
	}
	SDL_FreeSurface(symbols_img);
}

/*gets a rect in the SYMBOLS image matching the given tool
returns null if given tool is EMPTY*/
void get_tool_rect(board_tile tool, SDL_Rect *out_rect)
{
	if (tool.color == EMPTY || tool.type == EMPTY)
		return;
	int rect_y = use_fancy_tools ? 150 : 0;
	int rect_x = 10;
	if (tool.color == BLACK)
	{
		rect_y += SQUERE_S;
		if (use_fancy_tools)
			rect_y += 7;
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
		rect_x += SQUERE_S * 3 + 3;
		break;
	case WHITE_N:
		rect_x += SQUERE_S * 4 + 7;
		break;
	case WHITE_P:
		rect_x += SQUERE_S * 5 + 10;
		break;
	}
	out_rect->h = SQUERE_S;
	out_rect->w = SQUERE_S;
	out_rect->x = rect_x;
	out_rect->y = rect_y;
	return;
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
						40 + (LOAD_BTN_H + 20) * (slot_num / BTNS_PER_LINE), LOAD_BTN_W, LOAD_BTN_H };
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
	SDL_Rect top_button = { 20, 120, MAIN_IMG_W, MAIN_IMG_H };
	SDL_Rect game_prog_rect = { 0, 0, 280, MAIN_IMG_H };
	SDL_Rect imgrect = { 0, 0, MAIN_IMG_W, MAIN_IMG_H };
	SDL_Surface *w = NULL;

	char *images[MAIN_WIND_BUTTONS] = { NEW_GAME, LOAD_GAME, QUIT };

	int redraw = 1; /*detarmines if the screen should be drawn again*/
	int quit = 0;

	while (!quit) {

		if (redraw)
		{
			init_game();
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
			paint_all_buttons(w, images, MAIN_WIND_BUTTONS, top_button, imgrect);
			if (should_terminate)
			{
				SDL_FreeSurface(w);
				return 1;
			}
			/*heading*/
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
		int num_button;
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
				num_button = get_num_button_pressed(e.button.x, e.button.y, top_button, MAIN_WIND_BUTTONS);
				if (num_button != -1)
				{
					if (num_button == 2)
					{/*quit game*/
						quit = 1;
					}
					else
					{ /*load or new game*/
						SDL_FreeSurface(w);
						if (num_button == 1)
						{ /*load saved game*/
							int result = load_save_game_wind(0);
							if (result == -1 || should_terminate)
							{
								return 1;
							}
							if (result == 0)
							{
								redraw = 1;
								continue;
							}
						}
						int answer = player_selection_window(); /*player setting*/
						if (should_terminate || answer == 1)
						{
							return 1;
						}
						if (answer == 2)
						{
							redraw = 1;
							continue;
						}
						if (player_vs_player == 2)
						{
							answer = ai_settings_window(); /*ai settings*/
							if (should_terminate || answer == 1)
							{
								return 1;
							}
							if (answer == 2)
							{
								redraw = 1;
								continue;
							}
						}
						/*run game*/
						if (game_window() || should_terminate)
						{
							return 1;
						}

						redraw = 1;
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

/*draw the player selection window of the game
returns 0 if finished, 1 on error and 2 if user pressed cancel*/
int player_selection_window()
{
	num_tiles_marked = 0; /*if a previous window marked a tile, we dont care*/

	SDL_Event e;
	SDL_Rect board_rect = { (int)(SQUERE_S*0.25), SQUERE_S, SQUERE_S *BOARD_SIZE, SQUERE_S*BOARD_SIZE };
	int buttons_x = board_rect.x + board_rect.w + 5;
	int buttons_y = 80;
	SDL_Rect imgrect = { 0, 0, PLAY_IMG_W, PLAY_IMG_H };

	SDL_Rect top_button = { buttons_x, buttons_y, PLAY_IMG_W, PLAY_IMG_H };

	int quit = 0;
	int redraw = 1;
	SDL_Surface *w = NULL;

	char piece = '\0';
	char color;

	while (!quit) {
		if (redraw)
		{
			char *versus_img = player_vs_player == 1 ? P_VS_P : P_VS_C;
			char *next_img = next_player == WHITE ? NEXT_WHITE : NEXT_BLACK;
			char *images[PLAY_WIND_BUTTONS] = { versus_img, next_img, FINISH, CANCEL_IMG }; /*-1 because cancel button isnt here*/
			SDL_WM_SetCaption("Player Selection Settings", "Player Selection Settings");
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


			paint_boarder_pieces(0, WHITE, 0, w);
			if (should_terminate)
				return 1;
			paint_boarder_pieces(0, BLACK, 1, w);
			if (should_terminate)
				return 1;


			/*draw buttons*/
			paint_all_buttons(w, images, PLAY_WIND_BUTTONS, top_button, imgrect);
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
		int num_button;
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
				num_button = get_num_button_pressed(e.button.x, e.button.y, top_button, PLAY_WIND_BUTTONS);
				if (num_button != -1)
				{
					switch (num_button){
					case 0: /*versus*/
						 /*change player_vs_player*/
							player_vs_player = player_vs_player % 2 + 1;
							redraw = 1;
							break;
					case 1: /*next player*/
						/*change next player*/
						next_player = flip_color(next_player);
						redraw = 1;
						break;
					case 2: /*finish*/
						/*move to next window*/
						if (is_board_init_legal())
						{
							SDL_FreeSurface(w);
							quit = 1;
						}
						else
						{
							SDL_Rect illegal_place = { 0, 0, PLAY_IMG_W, (int)(PLAY_IMG_H * 1.7) };
							SDL_Rect illegal = { buttons_x, (int)(buttons_y + PLAY_WIND_BUTTONS * (PLAY_IMG_H * 1.5)), illegal_place.w, illegal_place.h };
							draw_image(illegal, illegal_place, ILLEGAL, w, 1);
							if (should_terminate) {
								SDL_FreeSurface(w);
								return 1;
							}
							if (SDL_Flip(w) != 0) {
								should_terminate = 1;
								printf("ERROR: failed to flip buffer: %s\n", SDL_GetError());
								SDL_FreeSurface(w);
								return 1;
							}
							SDL_Delay(1500);

							/*clear back to black*/
							if (SDL_FillRect(w, &illegal, 0) != 0) {
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
						}
						break;
					case 3: /*cancel*/
						SDL_FreeSurface(w);
						return 2;
					}
				}
				else if (is_in_rect(e.button.x, e.button.y, board_rect))
				{ /*a press on the board. only matters if a piece was chosen*/
					if (piece != '\0')
					{
						handle_board_setting_press(e, w, piece, color);
						draw_current_board(w);
						if (SDL_Flip(w) != 0) {
							should_terminate = 1;
							printf("ERROR: failed to flip buffer: %s\n", SDL_GetError());
							SDL_FreeSurface(w);
							return 1;
						}
					}
				}
				else
				{
					piece = get_boarder_pieces_choice(e.button.x, e.button.y, 0, 1);
					color = BLACK;
					if (piece == '\0')
					{
						piece = get_boarder_pieces_choice(e.button.x, e.button.y, 0, 0);
						color = WHITE;
					}
					if (piece != '\0')
					{ /*paint piece edge*/
						if (num_tiles_marked > 0)
						{
							paint_rect_edges(current_board_tiles_marked[0], w, SDL_MapRGB(w->format, 150, 150, 150));
							if (should_terminate)
							{
								SDL_FreeSurface(w);
								return 1;
							}
						}
						SDL_Rect r= { e.button.x - e.button.x % SQUERE_S, e.button.y - e.button.y % SQUERE_S, SQUERE_S, SQUERE_S };
						current_board_tiles_marked[0] = r;
						num_tiles_marked = 1;
						paint_rect_edges(current_board_tiles_marked[0], w, SDL_MapRGB(w->format, 0, 255, 0));
						if (should_terminate)
						{
							SDL_FreeSurface(w);
							return 1;
						}

					}
					if (SDL_Flip(w) != 0) {
						should_terminate = 1;
						printf("ERROR: failed to flip buffer: %s\n", SDL_GetError());
						SDL_FreeSurface(w);
						return 1;
					}
				}
			default:
				break;
			}
		}
	}
	SDL_FreeSurface(w);
	return 0;
}


/*handles an event of click in the chess board area of the setting window*/
void handle_board_setting_press(SDL_Event e, SDL_Surface *w, char piece, char color)
{
	int selected_col = get_tile_col(e.button.x);
	int selected_row = get_tile_row(e.button.y);
	board[selected_col][selected_row].type = piece;
	if (piece == EMPTY)
		color = EMPTY;
	board[selected_col][selected_row].color = color;
}

/*draw the AI settings window of the game
returns 0 if finished, 1 on error and 2 if user pressed cancel*/
int ai_settings_window()
{
	SDL_Event e;
	int buttons_x = 10;
	SDL_Rect imgrect = { 0, 0, PLAY_IMG_W, PLAY_IMG_H };

	SDL_Rect top_button = { buttons_x, 50, PLAY_IMG_W, PLAY_IMG_H };

	int quit = 0;
	int redraw = 1;
	SDL_Surface *w = NULL;


	while (!quit) {
		if (redraw)
		{
			char diff_img[128];
			strcpy(diff_img, DIFFICULTY);
			diff_img[DIFF_GEN_OFFSET] = minimax_depth == -1 ? '0' : minimax_depth + '0';
			char *user_img = user_color == WHITE ? USER_WHITE : USER_BLACK;
			char *images[AI_WIND_BUTTONS] = { diff_img, user_img, FINISH, CANCEL_IMG }; /*-1 because cancel button isnt here*/


			SDL_WM_SetCaption("Player Selection Settings", "Player Selection Settings");
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
			paint_all_buttons(w, images, AI_WIND_BUTTONS, top_button, imgrect);
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

		int num_button;
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
				num_button = get_num_button_pressed(e.button.x, e.button.y, top_button, AI_WIND_BUTTONS);
				if (num_button != -1)
				{
					switch (num_button){
					case 0: /*difficulty*/
						/*change difficulty*/
						minimax_depth += 1;
						if (minimax_depth == 5)
							minimax_depth = -1;
						else if (minimax_depth == 0)
							minimax_depth = 1;
						redraw = 1;
						break;
					case 1: /*user color*/
						user_color = flip_color(user_color);
						redraw = 1;
						break;
					case 2: /*finish*/
						SDL_FreeSurface(w);
						quit = 1;
						break;
					case 3: /*cancel*/
						SDL_FreeSurface(w);
						return 2;
					}
				}
			default:
				break;
			}
		}
	}
	SDL_FreeSurface(w);
	return 0;
}