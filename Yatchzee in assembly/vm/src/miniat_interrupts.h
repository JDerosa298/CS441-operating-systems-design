#ifndef _MINIAT_PRIV_INTERRUUPTS_H_
#define _MINIAT_PRIV_INTERRUUPTS_H_

#include "miniat/miniat.h"

/**
 * Cause the miniat to handle interrupts, should be called on every clock cycle.
 * @param m
 *  The miniat whose interrupts to handle
 */
extern void m_interrupts_handle_interrupt(miniat *m);

/**
 * Initialize the miniat interrupt subsytem
 * @param m
 *  The miniat whose interrupt subsytem needs initialization
 */
extern void m_interrupts_init(miniat *m);

#endif
