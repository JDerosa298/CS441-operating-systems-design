#ifndef MINIAT_PRIV_CACHE_H_
#define MINIAT_PRIV_CACHE_H_

#include <stdbool.h>

#include "miniat_defines.h"
#include "miniat_structures.h"
#include "miniat_typedefs.h"

/**
 * Initialize the miniat cache subsystem.
 * @param m
 *  The miniat to initialize
 * @param lua_cache_file
 *  The lua script to use for the cache.
 */
void m_cache_init(miniat *m, const char *lua_cache_file);

/**
 * Indicate to the cache that a read is occurring.
 * @param m
 *  The miniat cache to read.
 * @param is_ins
 *  A boolean indicating whether or not it's an instruction (true) or data (false)
 *  access.
 * @param address
 *  The address the access is occurring at.
 * @return
 *  The cache stall duration. 0 indicates use the default stall durations.
 */
unsigned m_cache_read(miniat *m, bool is_ins, m_uword address);

/**
 * Indicate to the cache that a write is occurring.
 * @param m
 *  The miniat cache to write.
 * @param is_ins
 *  A boolean indicating whether or not it's an instruction (true) or data (false)
 *  access.
 * @param address
 *  The address the access is occurring at.
 * @return
 *  The cache stall duration. 0 indicates use the default stall durations.
 */
unsigned m_cache_write(miniat *m, bool is_ins, m_uword address);

/**
 * Destroys the cache
 * @param m
 *  The miniat whom's cache to destroy.
 */
void m_cache_destroy(miniat *m);

#endif /* MINIAT_PRIV_CACHE_H_ */
