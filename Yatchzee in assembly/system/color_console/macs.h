#ifndef __MACS_H__
#define __MACS_H__

#include <stdbool.h>
#include <curses.h>

#define MACS_MIN       -2

#define MACS_ULCORNER  -2
#define MACS_LLCORNER  -3
#define MACS_URCORNER  -4
#define MACS_LRCORNER  -5
#define MACS_LTEE      -6
#define MACS_RTEE      -7
#define MACS_BTEE      -8
#define MACS_TTEE      -9
#define MACS_HLINE    -10
#define MACS_VLINE    -11
#define MACS_PLUS     -12
#define MACS_S1       -13
#define MACS_S9       -14
#define MACS_DIAMOND  -15
#define MACS_CKBOARD  -16
#define MACS_DEGREE   -17
#define MACS_PLMINUS  -18
#define MACS_BULLET   -19
#define MACS_LARROW   -20
#define MACS_RARROW   -21
#define MACS_DARROW   -22
#define MACS_UARROW   -23
#define MACS_BOARD    -24
#define MACS_LANTERN  -25
#define MACS_BLOCK    -26

/*
 * These are likely to be available on System Vs
 */
#define MACS_S3       -27
#define MACS_S7       -28
#define MACS_LEQUAL   -29
#define MACS_GEQUAL   -30
#define MACS_PI       -31
#define MACS_NEQUAL   -32
#define MACS_STERLING -33

#define MACS_MAX      -33

#define MACS_CNT (MACS_MIN - MACS_MAX + 1)

extern bool is_macs(int ch);
extern void macs_cleanup();
extern void macs_init();
extern int macs_map(int ch);

#endif /* __MACS_H__ */
