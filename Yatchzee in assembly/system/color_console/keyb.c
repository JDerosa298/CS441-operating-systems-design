#include <curses.h>

#include "util.h"
#include "keyb.h"
#include "term.h"


#define INITIAL_BUFFER_SIZE 32

#define KEYB_ESC_KEY 27


static int *buffer = NULL;
static int size = 0;
static int total_size = 0;
static int newest = 0; /* points to next unused space */
static int oldest = 0; /* poitns to oldest buffered data */


void keyb_cleanup() {

	if (buffer) {
		free(buffer);
	}

	return;
}


void keyb_init() {

	if (buffer) {
		IMPOSSIBLE("Bug reports are most politely written by hand");
	}

	SAFE_MALLOC(buffer, int *, sizeof(int) * INITIAL_BUFFER_SIZE);
	total_size = INITIAL_BUFFER_SIZE;

	return;
}


void keyb_flush() {

	size = 0;
	oldest = 0;
	newest = 0;

	return;
}

int keyb_getch() {

	int c = ERR;

	if (!buffer) {
		IMPOSSIBLE("Drop to the binary");
	}

	if (size > 0) {
		c = buffer[oldest];
		oldest = (oldest + 1) % total_size;
		size--;
	}

	return c;
}


void keyb_update() {

	int c;

	while ((c = getch()) != ERR) {
		if (c == KEYB_ESC_KEY) {
			exit(EXIT_SUCCESS);
		}

		if (c == KEY_RESIZE) {
			terminal_should_be_big_enough();
			continue;
		}

		/* out of space... double it */
		if (size == total_size) {
			SAFE_REALLOC(buffer, int *, sizeof(int) * total_size * 2);
			/* copy the first part of the old buffer to second half of the old buffer */
			memcpy(buffer + total_size, buffer, sizeof(int) * newest);
			newest += total_size;
			total_size *= 2;
		}

		buffer[newest] = c;
		newest = (newest + 1) % total_size;
		size++;
	}
}