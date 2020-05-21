#ifndef _MINIAT_PC_H_
#define _MINIAT_PC_H_

#include "miniat/miniat.h"

/**
 * Initializes the miniat pc queue
 * @param m
 *  The miniat whose pc queue to initialize
 */
extern void m_pc_init(miniat *m);

/**
 * Adds to the tail of the pc queue
 * @param m
 *  The miniat whose pc queue to add to
 * @param address
 *  The address to add to the queue, typically NPC.
 */
extern void m_pc_add_last(miniat *m, m_uword address);

/**
 * This function will pop the queue values into the PC register. Note that it leaves it in the PC register.
 * Higher queue values will rotate down and the effective pc is left in the pc register.
 * @param m
 *  The miniat whose pc queue to get from
 * @return
 *  The target address
 */
extern m_uword m_pc_remove_first(miniat *m);

extern m_uword m_pc_peek(miniat *m);

/**
 * Clear the PC queue. This is used on a flush of the pipeline
 * @param m
 *  The miniat whose pc to clear
 */
extern void m_pc_clear(miniat *m);

#endif
