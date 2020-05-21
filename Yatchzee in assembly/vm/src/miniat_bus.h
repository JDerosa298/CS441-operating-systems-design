#ifndef _MINIAT_PRIV_BUS_H_
#define _MINIAT_PRIV_BUS_H_

#include <stdbool.h>

#include "miniat/miniat.h"

/**
 *  Implements the basic function for reading over a miniat bus.
 * @param m
 *  The miniat who's bus to read
 * @param address
 *  The address to read from
 * @param data
 *  A pointer to a location to store the data
 * @param int_tripped
 *  Interrupt if a BUS_ERROR occurred. Bus errror's occur on an invalid bus state transition.
 * @return
 *  A boolean indicating whether or not the bus transaction is complete.
 * @throw
 *  EINVAL
 */
bool m_bus_read(miniat *m, m_uword address, m_uword *data,
		m_uword *int_tripped);

/**
 *  Implements the basic function for writing over a miniat bus.
 * @param m
 *  The miniat who's bus to read
 * @param address
 *  The address to read from
 * @param data
 *  The data to write
 * @param int_tripped
 *  Interrupt if a BUS_ERROR occurred. Bus errror's occur on an invalid bus state transition.
 * @return
 *  A boolean indicating whether or not the bus transaction is complete.
 */
bool m_bus_write(miniat *m, m_uword address, m_uword data,
		m_uword *int_tripped);

/**
 * The Miniat's bus is a return to zero bus. This function resets the bus to the initial (zero)
 * state.
 * @param m
 *  The miniat whose bus to reset.
 */
void m_bus_reset(miniat *m);

/**
 * The address provided by a bus connector is to a separate region of memory then the actual bus
 * implementation uses. This way peripherals cannot try to drive lines directly that they should
 * only have read only access on.
 *
 * @param m
 *  The miniat whose public bus should be copied to the private bus used by the system.
 */
void m_bus_shuffle(miniat *m);
#endif
