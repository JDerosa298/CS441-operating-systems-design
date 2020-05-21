#ifndef _SIMPLE_KB_H_
#define _SIMPLE_KB_H_

#include "miniat/miniat.h"

typedef struct p_simple_kb p_simple_kb;

/**
 * Create a simple keyboard peripheral. This keyboard
 * peripheral only accepts read requests at the address
 * specified. It returns 1 byte of data read from stdin
 * when a request is on the bus and it is given a clock
 * tick.
 *
 * @param address
 * 	The address to register this peripheral too
 *
 * @return
 *  The simple keyboard instance
 */
extern p_simple_kb *p_simple_kb_new(m_uword address);

/**
 * Free an instance of a simple keyboard
 * @param k
 * 	The instance to free
 */
extern void p_simple_kb_free(p_simple_kb *k);

/**
 * Give the simple kb a clock. If req is high,
 * rW is low and address is set to the address
 * registered in m_simple_kb_new(address), the kb
 * reads one char from stdin and places it onto the
 * data bus. It then brings ack high.
 *
 * if ack is high and address is set to the address
 * for this peripheral, the terminal will then bring
 * ack back to 0.
 *
 * @param k
 * 	Free a simple keyboard.
 */
extern void p_simple_kb_clock(p_simple_kb *k);

/**
 * Get the state of the bus as known by the peripheral.
 *
 * @param k
 *  The keyboard to get the bus state from.
 * @return
 * 	The bus state.
 */
extern m_bus p_simple_kb_get_bus(p_simple_kb *k);

/**
 * Set the bus state to the peripheral.
 * @param k
 *  The keyboard to set the bus state on.
 * @param bus
 * 	The bus state to set.
 */
extern void p_simple_kb_set_bus(p_simple_kb *k, m_bus bus);

/**
 * Sets the bus connector on the keyboard. This memory maps the miniat's bus
 * with the peripherals bus so you don't need to shuffle the data back and
 * forth manually.
 *
 * @param k
 * 	Keyboard to set
 * @param bus
 * 	The bus connector to use
 */
extern void p_simple_kb_bus_connector_set(p_simple_kb *k, m_bus *bus);


#endif /* M_SIMPLE_KEYBOARD_H_ */
