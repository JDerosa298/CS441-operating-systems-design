#ifndef _MINIAT_PRIV_MEMORY_H_
#define _MINIAT_PRIV_MEMORY_H_

#include "miniat/miniat.h"
#include "miniat_typedefs.h"

#define M_NO_STALL_DURATION 0

/**
 * Get the memory characteristics for a memory address. used to help
 * determine what permissions are given to a particular memory address.
 * @param address
 *  The address to check
 * @return
 *  The characteristics of the memory
 */
extern m_memory_characteristics m_memory_address_lookup(m_uword address);

/**
 * Write to memory and verifies the validity of the memory address before
 * performing the write.
 *
 * @param m
 *  The miniat whose memory to write
 * @param address
 *  The address of the memory to write
 * @param data
 *  The data to write
 *
 * @note:
 *  This does not call through the cache, you may want m_memory_write_can_stall()
 */
extern void m_memory_write(miniat *m, m_uword address, m_uword data);

/**
 * Write to memory and verifies the validity of the memory address before
 * performing the write.
 *
 * @param m
 *  The miniat whose memory to write
 * @param address
 *  The address of the memory to write
 * @param data
 *  The data to write
 * @return
 *   The stall duration in cycles.
 */
extern m_uword m_memory_write_or_get_stall_time(miniat *m, m_uword address,
		m_uword data, bool is_ins);

/**
 * Read from memory
 * @param m
 *  The miniat whose memory to read
 * @param address
 *  The address to read
 * @param data
 *  The value of the read is stored here
 *
 * @note:
 *  This does not call through the cache, you may want m_memory_read_can_stall()
 */
extern void m_memory_read(miniat *m, m_uword address, m_uword *data);

/**
 * Read from memory
 * @param m
 *  The miniat whose memory to read
 * @param address
 *  The address to read
 * @param data
 *  The value of the read is stored here
 * @param is_ins
 *  True if its an instruction fetch, false for data.
 * @return
 *   The stall duration in cycles.
 */
extern m_uword m_memory_read_or_get_stall_time(miniat *m, m_uword address,
		m_uword *data, bool is_ins);

/**
 * Initialize the CSR's. This function maps the CSR pointers back into the proper spot in mem
 * @param m
 *  The miniat whose CSR's to map
 */
extern void m_memory_csr_init(miniat *m);

#endif
