#ifndef _DEBUGGER_H_
#define _DEBUGGER_H_

#include <stdio.h>
#include <string.h>

#include <miniat/miniat.h>

typedef struct debugger debugger;
typedef enum debugger_state debugger_state;

/**
 * Create a new instance of the miniat debugger
 * @param m
 * 	The miniat to debug
 * @return
 * 	an instance of the debugger
 */
extern debugger* debugger_new(miniat *m);

/**
 * Free a debugegr instance
 * @param d
 * 	The debugger to free
 */
extern void debugger_free(debugger *d);

/**
 * Clock the debugger. This cuases it to print a screen to the terminal for interaction.
 * @param d
 * 	The debugger to use
 * @param m
 * 	The miniat to use
 * 	TODO: Why pass the miniat twice to this library?
 */
extern void debugger_clock(debugger *d, miniat *m);

#endif
