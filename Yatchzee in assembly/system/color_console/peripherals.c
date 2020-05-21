#include <curses.h>

#include "miniat/miniat.h"
#include "peripherals.h"
#include "util.h"
#include "term.h"
#include "keyb.h"


#define P_KEYB           0x4000
#define P_TERM           0x4001
#define P_TERM_FIXED     0x4002
#define P_RAW_TERM       0x4003
#define P_RAW_TERM_FIXED 0x4004
#define P_REFRESH        0x4005

#define P_CURSOR_X       0x4010
#define P_CURSOR_Y       0x4011
#define P_CURSOR_Y_X     0x4012
#define P_FG_COLOR       0x4013
#define P_BG_COLOR       0x4014
#define P_ATTR_UNDERLINE 0x4015
#define P_ATTR_REVERSE   0x4016
#define P_ATTR_BLINK     0x4017
#define P_ATTR_BOLD      0x4018
#define P_ATTR_INVIS     0x4019
#define P_ALL_ATTRIBUTES 0x401A
#define P_ATTR_STACK     0x401B
#define P_COORD_STACK    0x401C

#define P_FLUSH_BUFFER   0x4020
#define P_SHOW_CURSOR    0x4021
#define P_BEEP           0x4022
#define P_ERASE          0x4023
#define P_TABSIZE        0x4024
#define P_AUTO_REFRESH   0x4025

#define P_EXIT           0x4500


int keyb_update_counter = KEYB_REFRESH_PERIOD;


void peripherals_cleanup() {

	/* nothing yet */

	return;
}

void peripherals_clock(miniat *m) {

	m_bus bus;

	if (!m) {
		IMPOSSIBLE("It was never meant to be");
	}

/*
 * Don't update the key buffer every cycle because curses refreshes the screen
 * whenever it is polled!  Whose idea was that?
 */
	if (keyb_update_counter <= 0) {
		keyb_update();
		keyb_update_counter = KEYB_REFRESH_PERIOD;
	}
	keyb_update_counter--;
	bus = miniat_pins_bus_get(m);


	if (bus.req == M_HIGH && bus.ack == M_LOW) {
		/*
		 * The MiniAT is waiting for a peripheral acknowledgement
		 */
		bool handled = true;
		if (bus.rW == M_HIGH) {
			int tmp_x;
			int tmp_y;
			raw_terminal_word raw;

			/* STOR to peripheral */

			switch (bus.address) {
			case P_TERM:
				terminal_printc(bus.data, true, false);
				break;
			case P_TERM_FIXED:
				terminal_printc(bus.data, false, false);
				break;
			case P_RAW_TERM:
				terminal_printc(bus.data, true, true);
				break;
			case P_RAW_TERM_FIXED:
				terminal_printc(bus.data, false, true);
				break;
			case P_REFRESH:
				terminal_refresh();
				break;
			case P_CURSOR_X:
				if ((m_sword)bus.data >= 0 && bus.data < TERMINAL_WIDTH) {
					cursor_col = bus.data;
				}
				terminal_move_cursor(cursor_row, cursor_col);
				break;
			case P_CURSOR_Y:
				if ((m_sword)bus.data >= 0 && bus.data < TERMINAL_HEIGHT) {
					cursor_row = bus.data;
				}
				terminal_move_cursor(cursor_row, cursor_col);
				break;
			case P_CURSOR_Y_X:
				tmp_x = bus.data & ~((unsigned)~0 << 16);
				tmp_y = (bus.data >> 16) & ~((unsigned)~0 << 16);
				if (tmp_x >= 0 && tmp_y >= 0 && tmp_x < TERMINAL_WIDTH && tmp_y < TERMINAL_HEIGHT) {
					cursor_col = tmp_x;
					cursor_row = tmp_y;
					terminal_move_cursor(cursor_row, cursor_col);
				}
				break;
			case P_FG_COLOR:
				if (bus.data <= TERM_COLOR_MAX) {
					terminal_set_fg_color(bus.data);
				}
				break;
			case P_BG_COLOR:
				if (bus.data <= TERM_COLOR_MAX) {
					terminal_set_bg_color(bus.data);
				}
				break;
			case P_ATTR_UNDERLINE: if (bus.data) terminal_attributes |= A_UNDERLINE; else terminal_attributes &= ~A_UNDERLINE; break;
			case P_ATTR_REVERSE:   if (bus.data) terminal_attributes |= A_REVERSE;   else terminal_attributes &= ~A_REVERSE;   break;
			case P_ATTR_BLINK:     if (bus.data) terminal_attributes |= A_BLINK;     else terminal_attributes &= ~A_BLINK;     break;
			case P_ATTR_BOLD:      if (bus.data) terminal_attributes |= A_BOLD;      else terminal_attributes &= ~A_BOLD;      break;
			case P_ATTR_INVIS:     if (bus.data) terminal_attributes |= A_INVIS;     else terminal_attributes &= ~A_INVIS;     break;
			case P_ALL_ATTRIBUTES:
				raw.all.u = bus.data;
				terminal_set_raw(raw);
				break;
			case P_ATTR_STACK:
				terminal_push_attributes();
				break;
			case P_COORD_STACK:
				terminal_push_coords();
				break;
			case P_FLUSH_BUFFER:
				keyb_flush();
				break;
			case P_SHOW_CURSOR:
				if (bus.data) {
					if (curs_set(1) == ERR) {
						curs_set(2);
					}
				}
				else {
					curs_set(0);
				}
				break;
			case P_BEEP:
				beep();
				break;
			case P_ERASE:
				terminal_erase();
				terminal_move_cursor(0, 0);
				break;
			case P_TABSIZE:
				if (bus.data > 1 && bus.data < TERMINAL_WIDTH) {
					TABSIZE = bus.data;
				}
				break;
			case P_AUTO_REFRESH:
				terminal_auto_refresh = bus.data;
				break;
			case P_EXIT:
				exit(EXIT_SUCCESS);
				break;
			default:
				ERROR("There is no peripheral accepting writes at 0x%04X", bus.address);
				break;
			}
		}
		else {

			/* LOAD from peripheral */

			switch (bus.address) {
			case P_KEYB:
				keyb_update();
				keyb_update_counter = KEYB_REFRESH_PERIOD;
				bus.data = (m_uword)keyb_getch();
				break;
			case P_RAW_TERM:
				bus.data = terminal_get_raw_at_cursor(true).all.u;
				break;
			case P_RAW_TERM_FIXED:
				bus.data = terminal_get_raw_at_cursor(false).all.u;
				break;
			case P_CURSOR_X:
				bus.data = cursor_col;
				break;
			case P_CURSOR_Y:
				bus.data = cursor_row;
				break;
			case P_ALL_ATTRIBUTES:
				bus.data = terminal_get_raw().all.u;
				break;
			case P_ATTR_STACK:
				terminal_pop_attributes();
				break;
			case P_COORD_STACK:
				terminal_pop_coords();
				break;
			case P_TABSIZE:
				bus.data = TABSIZE;
				break;
			case P_ATTR_UNDERLINE: bus.data = terminal_attributes & A_UNDERLINE; break;
			case P_ATTR_REVERSE:   bus.data = terminal_attributes & A_REVERSE;   break;
			case P_ATTR_BLINK:     bus.data = terminal_attributes & A_BLINK;     break;
			case P_ATTR_BOLD:      bus.data = terminal_attributes & A_BOLD;      break;
			case P_ATTR_INVIS:     bus.data = terminal_attributes & A_INVIS;     break;
			default:
				ERROR("There is no peripheral accepting reads at 0x%04X", bus.address);
				break;
			}
		}

		if (handled) {
			bus.ack = M_HIGH;
		}
	}
	else if (bus.ack) {
		/*
		 * The MiniAT is waiting for the peripheral to lower the ACK pin
		 */
		switch (bus.address) {
		default:
			bus.ack = M_LOW;
			break;
		}
	}

	miniat_pins_bus_set(m, bus);
	if (terminal_auto_refresh) {
		terminal_refresh();
	}

	return;
}
