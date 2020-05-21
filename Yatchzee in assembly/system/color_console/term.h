#ifndef __TERM_H__
#define __TERM_H__

#include <stdbool.h>

#include "miniat/miniat.h"


typedef union raw_terminal_word raw_terminal_word;


union raw_terminal_word {
	struct {
		m_uword character: 16;
		m_uword underline:  1;
		m_uword reverse:    1;
		m_uword blink:      1;
		m_uword bold:       1;
		m_uword invisible:  1;
		m_uword reserved:   3;
		m_uword fg:         4;
		m_uword bg:         4;
	};
	m_word all;
};


/*
 * If these are changed, don't forget to change the "window too small"
 * message in the main loop.
 */
#define TERMINAL_WIDTH  80
#define TERMINAL_HEIGHT 24

#define TERM_TABSIZE     4
#define TERM_ESCDELAY   50

#define TERM_COLOR_MIN     0

#define TERM_COLOR_BLACK   0
#define TERM_COLOR_RED     1
#define TERM_COLOR_GREEN   2
#define TERM_COLOR_YELLOW  3
#define TERM_COLOR_BLUE    4
#define TERM_COLOR_MAGENTA 5
#define TERM_COLOR_CYAN    6
#define TERM_COLOR_WHITE   7

#define TERM_COLOR_MAX     7
#define TERM_COLOR_CNT (TERM_COLOR_MAX - TERM_COLOR_MIN + 1)

#define TERM_COLOR_INDEX_MAX (TERM_COLOR_CNT * TERM_COLOR_CNT)

extern int cursor_row;
extern int cursor_col;

extern int terminal[TERMINAL_HEIGHT][TERMINAL_WIDTH];
extern unsigned int terminal_attributes;
extern bool terminal_auto_refresh;

extern void terminal_push_attributes();
extern void terminal_pop_attributes();
extern void terminal_push_coords();
extern void terminal_pop_coords();

extern void terminal_cleanup();
extern void terminal_init();
extern int  terminal_color_map(unsigned int c);
extern void terminal_erase();
extern raw_terminal_word terminal_get_raw();
extern raw_terminal_word terminal_get_raw_at_cursor(bool move_cursor);
extern void terminal_set_raw(raw_terminal_word raw);
extern void terminal_move_cursor(int y, int x);
extern void terminal_printc(int data, bool move_cursor, bool is_raw);
extern void terminal_refresh();
extern void terminal_redraw();
extern void terminal_set_fg_color(unsigned int c);
extern void terminal_set_bg_color(unsigned int c);
extern void terminal_setup();
extern void terminal_should_be_big_enough();

#endif /* __TERM_H__ */
