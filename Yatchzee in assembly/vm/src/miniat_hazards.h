#ifndef _MINIAT_PRIV_HAZARDS_H_
#define _MINIAT_PRIV_HAZARDS_H_

#include <stdbool.h>

#include "miniat/miniat.h"

/**
 * Initialize the hazard subsystem
 * @param m
 *  The miniat whose hazard subsyetm needs initialization.
 */
extern void m_hazards_init(miniat *m);

/**
 * Clear all the hazards from the queue.
 * @param m
 *  The miniat whose hazards need to be cleared.
 */
extern void m_hazards_clear(miniat *m);

/**
 * Add a hazard to the hazard queue.
 * @param m
 *  The miniat whose hazard queue to add too.
 * @param address
 *  The address of the hazard.
 * @param is_reg
 *  Whether or not the hazard is a register or memory.
 */
extern void m_hazards_add(miniat *m, m_uword address, bool is_reg);

/** Remove a hazard from the queue.
 *
 * This function removes a memory address to the hazard queue. In the event that the specified
 * memory address is not found, nothing happens. You must ensure that an instruction that places
 * a hazard also gets the chance to remove it. Any instructions changed to NOPs, for some reason,
 * must call this function FIRST to remove the marked hazard.
 *
 * @param m
 *  The miniat whose hazard to remove.
 * @param address
 *  The address of the hazard to remove.
 * @param is_reg
 *  Whether or not the hazard is memory or register.
 */
extern void m_hazards_remove(miniat *m, m_uword address, bool is_reg);

/** Check the queue for a hazard.
 * This function tests to see if the address exists in the hazard queue. The boolean is_reg is true
 * if the hazard you are checking should be in the register data hazard and is false if the hazard
 * is a memory data hazard.
 *
 * @param m
 *  The miniat whose hazard queue to check.
 * @param address
 *  The address of the hazard.
 * @param is_reg
 *  Whether or not the hazard is register or memory.
 * @return
 *  True if the hazard is in the queue, false otherwise.
 */
extern bool m_hazards_check(miniat *m, m_uword address, bool is_reg);

#endif
