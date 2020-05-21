#ifndef _MINIAT_PRIV_TIMERS_H_
#define _MINIAT_PRIV_TIMERS_H_

#include "miniat/miniat.h"

/**
 *
 * @param m
 */
extern void m_timers_init(miniat *m);

/**
 *  Provide all timers a clock tick
 * @param m
 *  The miniat whose timers to tick
 */
extern void m_timers_clock(miniat *m);

/**
 * Provide the general purpose timers a clock tick
 *
 * @param m
 *  The miniat whose timers to tick
 */
extern void m_timers_general_purpose_timers_clock(miniat *m);

/**
 * Provide the watchdog a clock tick
 *
 * @param m
 *  The miniat whose timers to tick
 */
extern void m_timers_watchdog_clock(miniat *m);

#endif
