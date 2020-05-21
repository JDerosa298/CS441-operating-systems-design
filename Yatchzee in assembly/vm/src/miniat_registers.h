#ifndef _MINIAT_PRIV_REGISTERS_H_
#define _MINIAT_PRIV_REGISTERS_H_

#include "miniat/miniat.h"

/**
 * Write to a register.
 *
 * @note
 *  Ensures that r0 cannot be modified
 *  Writes to the PC are correctly added to the queue
 *
 * @param m
 *  The miniat whose register to write
 * @param reg
 *  The register to write to, such as 5 for reg5
 * @param data
 *  The value to store
 */
extern void m_registers_write(miniat *m, m_ubyte reg, m_uword data);

/**
 * Read from a register
 * @param m
 *  The minat whose register to read from
 * @param reg
 *  The register to read, such as a 4 for reg4
 * @param data
 *  The reult of the read is stored here
 */
extern void m_registers_read(miniat *m, m_ubyte reg, m_uword *data);

#endif
