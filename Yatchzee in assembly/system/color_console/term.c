#include <curses.h>
#include <ctype.h>
#include <stdbool.h>

#include "miniat/miniat.h"
#include "term.h"
#include "util.h"
#include "macs.h"
#include "peripherals.h"


#define STACK_SIZE 128

/* abstract cursor position */
int cursor_row = 0;
int cursor_col = 0;

int terminal[TERMINAL_HEIGHT][TERMINAL_WIDTH];

/* where the bounded console output goes */
int term_x = 0;
int term_y = 0;

/* actual bounds of the client terminal */
int width = TERMINAL_WIDTH;
int height = TERMINAL_HEIGHT;

unsigned int terminal_attributes = 0;    /* current attribute OR-mask applied to new characters */
int terminal_color_pair;    /* current FG/BG pair number applied to new characters */
bool terminal_auto_refresh = true;    /* whether to render the buffer window each cycle */

static unsigned int color_map[TERM_COLOR_CNT];    /* maps ASM colors to ncurses colors */
static unsigned int fg_color;
static unsigned int bg_color;
static unsigned int color_mask;
static unsigned int color_base_bit;

static raw_terminal_word attribute_stack[STACK_SIZE];
static int coord_stack[STACK_SIZE][2];    /* [][0] = Y, [][1] = X */
static int attribute_stack_top = 0;
static int coord_stack_top = 0;


void terminal_push_attributes() {

	if (attribute_stack_top < STACK_SIZE) {
		attribute_stack[attribute_stack_top] = terminal_get_raw();
		attribute_stack_top++;
	}

	return;
}


void terminal_pop_attributes() {

	if (attribute_stack_top > 0) {
		raw_terminal_word raw;

		attribute_stack_top--;
		raw = attribute_stack[attribute_stack_top];
		terminal_set_raw(raw);
	}

	return;
}


void terminal_push_coords() {

	if (coord_stack_top < STACK_SIZE) {
		coord_stack[coord_stack_top][0] = cursor_row;
		coord_stack[coord_stack_top][1] = cursor_col;
		coord_stack_top++;
	}

	return;
}


void terminal_pop_coords() {

	if (coord_stack_top > 0) {
		coord_stack_top--;
		terminal_move_cursor(coord_stack[coord_stack_top][0], coord_stack[coord_stack_top][1]);
	}

	return;
}


void terminal_move_cursor(int y, int x) {

	cursor_row = y;
	cursor_col = x;
	move(term_y + y, term_x + x);

	return;
}

void terminal_cleanup() {

	endwin();

	return;
}

void terminal_setup() {

	int warnings = 0;

	if (initscr() == NULL) {
		ERROR("Could not initialize ncurses");
	}

	if (start_color() == ERR) {
		ERROR("Could not initialize terminal colors");
	}

	if (has_colors() == false) {
		ERROR("Your terminal does not support color");
	}

	if (cbreak() == ERR) {
		ERROR("Could not disable character buffering");
	}

	if (noecho() == ERR) {
		ERROR("Could not disable echo");
	}

	if (nodelay(stdscr, true) == ERR) {
		ERROR("Could not disable blocking on \"getch\"");
	}

	if (keypad(stdscr, true) == ERR) {
		warnings++;
		printw("\n%d)\tCould not enable keypad usage.  Using non-ASCII keys for input will not work.\n", warnings);
	}

	if (curs_set(1) == ERR) {
		if (curs_set(2) == ERR) {
			warnings++;
			printw("\n%d)\tCould not initially turn cursor on.  Showing/hiding the cursor may not work.\n", warnings);
		}
	}

	#ifndef _WIN32
	if (getenv("ESCDELAY") == NULL) {
		ESCDELAY = TERM_ESCDELAY;
	}
	#endif

	if (getenv("TABSIZE") == NULL) {
		TABSIZE = TERM_TABSIZE;
	}

	if (warnings > 0) {
		printw("\n\nBecause of the issues listed above, the MiniAT color console may not perform\n");
		printw("as expected with all binary images.  Many will run perfectly, and most will run\n");
		printw("well enough to be completely usable.\n");
		printw("\nTry another terminal emulator or change the emulation of this client to correct\n.");
		printw("these issue.\n");
		printw("\n\nPress any key to continue...");
		refresh();

		while (getch() == ERR) ; /* wait for keypress */
		erase();
		refresh();
	}

	return;
}

void terminal_init() {

	short fg;

	color_map[TERM_COLOR_BLACK] = COLOR_BLACK;
	color_map[TERM_COLOR_RED] = COLOR_RED;
	color_map[TERM_COLOR_GREEN] = COLOR_GREEN;
	color_map[TERM_COLOR_YELLOW] = COLOR_YELLOW;
	color_map[TERM_COLOR_BLUE] = COLOR_BLUE;
	color_map[TERM_COLOR_MAGENTA] = COLOR_MAGENTA;
	color_map[TERM_COLOR_CYAN] = COLOR_CYAN;
	color_map[TERM_COLOR_WHITE] = COLOR_WHITE;

	for (fg = TERM_COLOR_MIN; fg <= TERM_COLOR_MAX; fg++) {
		short bg;
		for (bg = TERM_COLOR_MIN; bg <= TERM_COLOR_MAX; bg++) {
			if (bg == TERM_COLOR_BLACK && fg == TERM_COLOR_WHITE) {
				continue;
			}
			terminal_color_pair = (bg * TERM_COLOR_CNT) + (TERM_COLOR_MAX - fg);
			if (init_pair(terminal_color_pair, color_map[fg], color_map[bg]) == ERR) {
				ERROR("Could not initialize the color pair (fg:bg) %d:%d", fg, bg);
			}
		}
	}

	color_mask = COLOR_PAIR(TERM_COLOR_INDEX_MAX - 1);
	if (color_mask == 0) {
		IMPOSSIBLE("There goes the neighborhood");
	}
	color_base_bit = 0;
	while ((color_mask & (1 << color_base_bit)) == 0) {
		color_base_bit++;
	}

	terminal_set_fg_color(COLOR_WHITE);
	terminal_set_bg_color(COLOR_BLACK);

	terminal_erase();
	terminal_refresh();

	return;
}


int generate_color_pair_mask(int bg, int fg) {

	int mask;

	mask = ((bg * TERM_COLOR_CNT) + (TERM_COLOR_MAX - fg)) << color_base_bit;

	return mask;
}


void terminal_set_fg_color(unsigned int c) {

	fg_color = c;
	terminal_color_pair = (bg_color * TERM_COLOR_CNT) + (TERM_COLOR_MAX - fg_color);
	terminal_attributes &= ~color_mask; /* remove old color */
	terminal_attributes |= terminal_color_pair << color_base_bit;

	if (color_set(terminal_color_pair, NULL) == ERR) {
		ERROR("Couldn't set foreground color to %d", c);
	}

	return;
}


void terminal_set_bg_color(unsigned int c) {

	bg_color = c;
	terminal_color_pair = (bg_color * TERM_COLOR_CNT) + (TERM_COLOR_MAX - fg_color);
	terminal_attributes &= ~color_mask; /* remove old color */
	terminal_attributes |= terminal_color_pair << color_base_bit;

	if (color_set(terminal_color_pair, NULL) == ERR) {
		ERROR("Couldn't set background color to %d", c);
	}

	return;
}


int terminal_color_map(unsigned int c) {

	if (c > TERM_COLOR_MAX) {
		ERROR("Invalid color mapping to %d when largest color is %d", c, TERM_COLOR_MAX);
	}

	return color_map[c];
}


raw_terminal_word terminal_get_raw() {

	raw_terminal_word raw;
	int color;
	int current = terminal_attributes;

	raw.reserved = 0;

	raw.underline = (current & A_UNDERLINE) ? 1 : 0;
	current &= ~A_UNDERLINE;

	raw.reverse = (current & A_REVERSE) ? 1 : 0;
	current &= ~A_REVERSE;

	raw.blink = (current & A_BLINK) ? 1 : 0;
	current &= ~A_BLINK;

	raw.bold = (current & A_BOLD) ? 1 : 0;
	current &= ~A_BOLD;

	raw.invisible = (current & A_INVIS) ? 1 : 0;
	current &= ~A_INVIS;

	color = current;
	color = (color & color_mask) >> color_base_bit;

	raw.bg = color / TERM_COLOR_CNT;
	raw.fg = TERM_COLOR_MAX - (color % TERM_COLOR_CNT);

	current &= ~color_mask;
	raw.character = 0;

	return raw;
}


raw_terminal_word terminal_get_raw_at_cursor(bool move_cursor) {

	raw_terminal_word raw;
	int current = terminal[cursor_row][cursor_col];
	int color;

	raw.reserved = 0;

	raw.underline = (current & A_UNDERLINE) ? 1 : 0;
	current &= ~A_UNDERLINE;

	raw.reverse = (current & A_REVERSE) ? 1 : 0;
	current &= ~A_REVERSE;

	raw.blink = (current & A_BLINK) ? 1 : 0;
	current &= ~A_BLINK;

	raw.bold = (current & A_BOLD) ? 1 : 0;
	current &= ~A_BOLD;

	raw.invisible = (current & A_INVIS) ? 1 : 0;
	current &= ~A_INVIS;

	color = current;
	color = (color & color_mask) >> color_base_bit;

	raw.bg = color / TERM_COLOR_CNT;
	raw.fg = TERM_COLOR_MAX - (color % TERM_COLOR_CNT);

	current &= ~color_mask;
	raw.character = current;

	if (move_cursor) {
		cursor_col++;
		if (cursor_col >= TERMINAL_WIDTH) {
			cursor_col = 0;
			cursor_row = (cursor_row + 1) % TERMINAL_HEIGHT;
		}
	}
	terminal_move_cursor(cursor_row, cursor_col);

	return raw;
}


void terminal_set_raw(raw_terminal_word raw) {

	terminal_attributes = 0;

	if (raw.underline) { terminal_attributes |= A_UNDERLINE; }
	if (raw.reverse)   { terminal_attributes |= A_REVERSE;   }
	if (raw.blink)     { terminal_attributes |= A_BLINK;     }
	if (raw.bold)      { terminal_attributes |= A_BOLD;      }
	if (raw.invisible) { terminal_attributes |= A_INVIS;     }

	if (raw.fg <= TERM_COLOR_MAX) {
		terminal_set_fg_color(raw.fg);
	}
	else {
		terminal_set_fg_color(fg_color);
	}

	if (raw.bg <= TERM_COLOR_MAX) {
		terminal_set_bg_color(raw.bg);
	}
	else {
		terminal_set_bg_color(bg_color);
	}

	return;
}


void terminal_refresh() {

	refresh();

	return;
}


void terminal_redraw() {

	int x;
	int y;

	terminal_push_attributes();
	terminal_attributes = 0;
	terminal_set_bg_color(TERM_COLOR_BLACK);
	terminal_set_fg_color(TERM_COLOR_WHITE);

	erase();

	for (y = 0; y < TERMINAL_HEIGHT; y++) {
		move(term_y + y, term_x);
		for (x = 0; x < TERMINAL_WIDTH; x++) {
			addch(terminal[y][x]);
		}
	}
	terminal_move_cursor(cursor_row, cursor_col);

	terminal_refresh();

	terminal_pop_attributes();

	return;
}


void terminal_should_be_big_enough() {

	int current_width;
	int current_height;

	bool need_redraw = false;

	terminal_push_attributes();
	terminal_attributes = 0;
	terminal_set_bg_color(TERM_COLOR_BLACK);
	terminal_set_fg_color(TERM_COLOR_WHITE);

	for (;;){
		getmaxyx(stdscr, current_height, current_width);
		if (current_width >= TERMINAL_WIDTH && current_height >= TERMINAL_HEIGHT) {
			break;
		}

		int x;
		int y;
		int cx;
		int cy;

		/*
		 * What's important is it is true when this cycle is
		 * the previous cycle during the next cycle :-P
		 */
		need_redraw = true;

		for (y = 0; y < current_height; y++) {
			move(y, 0);
			for (x = 0; x < current_width; x++) {
				addch('@');
			}
		}
		cx = current_width  / 2;
		cy = current_height / 2;

		mvprintw(cy - 3, cx - 10, "                    ");
		mvprintw(cy - 2, cx - 10, "  Window too small  ");
		mvprintw(cy - 1, cx - 10, " resize to >= 80x24 ");
		mvprintw(cy    , cx - 10, "                    ");
		mvprintw(cy + 1, cx - 10, "  (clock stopped)   ");
		mvprintw(cy + 2, cx - 10, "                    ");

		refresh();
	}
	terminal_pop_attributes();

	if (current_width != width || current_height != height) {
		width = current_width;
		height = current_height;
		term_x = (width - TERMINAL_WIDTH) / 2;
		term_y = (height - TERMINAL_HEIGHT) / 2;
		need_redraw = true;
	}

	if (need_redraw) {
		terminal_redraw();
	}

	return;
}


void terminal_erase() {

	int x;
	int y;

	erase();

	for (y = 0; y < TERMINAL_HEIGHT; y++) {
		for (x = 0; x < TERMINAL_WIDTH; x++) {
			terminal[y][x] = ' ' | terminal_attributes;
		}
	}

	terminal_redraw();

	return;
}


void terminal_printc(int data, bool move_cursor, bool is_raw) {

	int tmp_attributes = 0;
	int raw_attributes = 0;

	raw_terminal_word raw = {.all = (m_word)data};

	if (is_raw) {
		raw_attributes |= raw.underline ? A_UNDERLINE : 0;
		raw_attributes |= raw.reverse   ? A_REVERSE   : 0;
		raw_attributes |= raw.blink     ? A_BLINK     : 0;
		raw_attributes |= raw.bold      ? A_BOLD      : 0;
		raw_attributes |= raw.invisible ? A_INVIS     : 0;
		raw_attributes |= generate_color_pair_mask(raw.bg, raw.fg);
		data = raw.character;
	}

	switch (data) {
	case '\n': /* new line */
		if (!move_cursor) {
			break;
		}
		cursor_col = 0;
		cursor_row = (cursor_row + 1) % TERMINAL_HEIGHT;
		terminal_move_cursor(cursor_row, cursor_col);
		break;
	case 0x9: /* horizontal tab */
		if (!move_cursor) {
			break;
		}
		cursor_col = ((cursor_col + 1 + TABSIZE) / TABSIZE) * TABSIZE;
		if (cursor_col >= TERMINAL_WIDTH) {
			cursor_col = 0;
			cursor_row = (cursor_row + 1) % TERMINAL_HEIGHT;
		}
		terminal_move_cursor(cursor_row, cursor_col);
		break;
	/* TODO : Fix this... because I don't like it */
	case KEY_BACKSPACE:
	case '\b':
	case 127: /* BS is commonly 127 on many terminals */
		if (move_cursor) {
			if (cursor_col > 0) {
				cursor_col--;
			}
			else if (cursor_row > 0) {
				cursor_row--;
				cursor_col = TERMINAL_WIDTH - 1;
			}
			mvaddch(cursor_row, cursor_col, ' ' | terminal_attributes);
		}
		else {
			addch(' ' | terminal_attributes);
		}
		terminal[cursor_row][cursor_col] = ' ' | terminal_attributes;
		terminal_move_cursor(cursor_row, cursor_col);
		break;
	case KEY_LEFT:
		if (!move_cursor) {
			break;
		}
		if (cursor_col > 0) {
			cursor_col--;
			terminal_move_cursor(cursor_row, cursor_col);
		}
		break;
	case KEY_RIGHT:
		if (!move_cursor) {
			break;
		}
		if (cursor_col < TERMINAL_WIDTH) {
			cursor_col++;
			terminal_move_cursor(cursor_row, cursor_col);
		}
		break;
	case KEY_UP:
		if (!move_cursor) {
			break;
		}
		if (cursor_row > 0) {
			cursor_row--;
			terminal_move_cursor(cursor_row, cursor_col);
		}
		break;
	case KEY_DOWN:
		if (!move_cursor) {
			break;
		}
		if (cursor_row < TERMINAL_HEIGHT) {
			cursor_row++;
			terminal_move_cursor(cursor_row, cursor_col);
		}
		break;
	default:
		tmp_attributes = terminal_attributes;
		if (is_raw) {
			terminal_attributes = raw_attributes;
		}

		if (is_macs(data)) {
			int ch = macs_map(data) | terminal_attributes;
			addch(ch);
			terminal[cursor_row][cursor_col] = ch;

			if (move_cursor) {
				cursor_col++;
				if (cursor_col >= TERMINAL_WIDTH) {
					cursor_col = 0;
					cursor_row = (cursor_row + 1) % TERMINAL_HEIGHT;
				}
			}
			terminal_move_cursor(cursor_row, cursor_col);
		}
		else if (isprint(data)) {
			data |= terminal_attributes;
			addch(data);
			terminal[cursor_row][cursor_col] = data;

			if (move_cursor) {
				cursor_col++;
				if (cursor_col >= TERMINAL_WIDTH) {
					cursor_col = 0;
					cursor_row = (cursor_row + 1) % TERMINAL_HEIGHT;
				}
				terminal_move_cursor(cursor_row, cursor_col);
			}
		}
		else {
			#ifdef PRINT_UNPRINTABLE_CHARACTERS
				printw("<%d>", data);
				terminal_move_cursor(cursor_row, cursor_col);
			#endif /* PRINT_UNPRINTABLE_CHARACTERS */
		}
		terminal_attributes = tmp_attributes;
		break;
	}

	return;
}
