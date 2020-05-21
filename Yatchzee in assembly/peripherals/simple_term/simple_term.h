#ifndef _SIMPLE_TERM_H_
#define _SIMPLE_TERM_H_

#include <miniat/miniat.h>

typedef struct p_simple_term p_simple_term;

/**
 * Creates an instance of a simple terminal peripheral.
 * The peripheral will only accept write requests at the
 * address specified when you create a simple terminal.
 * it writes one byte of data to stdout when a request is
 * on the bus and it is given a clock tick.
 *
 * @param address
 * 	The address this peripheral should be registered under.
 * @return
 * 	NULL on failure or the simple terminal instance.
 */
extern p_simple_term * p_simple_term_new(m_uword address);

/**
 * Free's a simple terminal.
 *
 * @param t
 * 	The simple terminal created by a call to m_simple_term_new().
 */
extern void p_simple_term_free(p_simple_term * t);

/**
 * Provide a clock tick to the terminal. If the request line
 * is high, the address set is M_SIMPLE_TERM_ADDRESS and the
 * request is for a write, the terminal will take the data off the bus,
 * ack back and print the value to stdout.
 * @param t
 * 	the terminal to provide the clock tick too.
 */
extern void p_simple_term_clock(p_simple_term *t);

/**
 * Get's the state of the bus from the peripheral. Ie it gets the
 * wires values. You use this function to "wire" the peripheral back
 * to the miniat.
 * @param t
 * 	The terminal whose state to get.
 * @return
 * 	The state of the bus.
 */
extern m_bus p_simple_term_get_bus(p_simple_term * t);

/**
 * Set's the state of the bus on the peripheral. Ie it sets the
 * wires values. You use this function to "wire" the miniat back
 * to the peripheral.
 * @param t
 * 	The terminal to set.
 * @param bus
 * 	The bus value to set.
 */
extern void p_simple_term_set_bus(p_simple_term * t, m_bus bus);

/**
 * Sets the bus connector on the terminal. This memory maps the miniat's bus
 * with the peripherals bus so you don't need to shuffle the data back and
 * forth manually.
 *
 * @param t
 * 	The terminal to set.
 * @param bus
 * 	The bus connector to set
 */
extern void p_simple_term_bus_connector_set(p_simple_term *t, m_bus *bus);

#endif
