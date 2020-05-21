#include <errno.h>
#include <string.h>

#include "miniat/miniat.h"
#include "miniat/miniat_error.h"
#include "miniat_bus.h"
#include "miniat_defines.h"
#include "miniat_dso.h"
#include "miniat_enums.h"
#include "miniat_memory.h"
#include "miniat_structures.h"
#include "miniat_typedefs.h"
#include "miniat_unions.h"

/*
 * Since this file is public, all definitions are in miniat.h
 */
PUBLIC void miniat_pins_bus_set(miniat *m, m_bus bus) {

	if (!m) {
		THROW(EINVAL, "MiniAT cannot be NULL");
	}

	memcpy(&m->bus.pub, &bus, sizeof(bus));
	/*
	 * An immediate call to "get" should
	 * return the new data from private.
	 */
	m_bus_shuffle(m);
}

PUBLIC m_bus miniat_pins_bus_get(miniat *m) {

	m_bus bus;

	if (!m) {
		THROW(EINVAL, "MiniAT cannot be NULL");
	}

	memcpy(&bus, &m->bus.priv, sizeof(bus));
	return bus;
}

PUBLIC void miniat_pins_set_xint(miniat *m, m_xint xint, m_byte data) {

	if (!m) {
		THROW(EINVAL, "MiniAT cannot be NULL");
	}

	if ((signed)xint < 0 || xint >= m_xint_id_cnt) {
		THROW(ERANGE, "Xint is not within range, got %d expected 0 - %d",
				xint, m_xint_id_cnt-1);
	}

	(*m->interrupts.ifr_h) = ((*m->interrupts.ifr_h) & ~(1 << xint))
			| (data.bits.bit0 << xint);

	return;
}

PUBLIC m_byte miniat_pins_get_xint(miniat *m, m_xint xint) {

	m_byte data = { .u = 0 };

	if (!m) {
		THROW(EINVAL, "MiniAT cannot be NULL");
	}

	if ((signed)xint < 0 || xint >= m_xint_id_cnt) {
		THROW(ERANGE, "Xint is not within range, got %d expected 0 - %d",
				xint, m_xint_id_cnt-1);
	}

	data.u = ((m_word) ((*m->interrupts.ifr_h) >> xint)).bits.bit0;

	return data;
}

PUBLIC void miniat_pins_set_timers(miniat *m, m_timer_id timer,
		m_timers_io pins) {

	if (!m) {
		THROW(EINVAL, "MiniAT cannot be NULL");
	}

	if ((signed)timer <0 || timer >= m_timer_id_cnt) {
		THROW(ERANGE, "Timer is not within range, got %d expected 0 - %d",
				timer, m_timer_id_cnt-1);
	}

	m->timers.general_purpose_timer[timer].io->line.input = pins.line.input;
}

PUBLIC m_timers_io miniat_pins_get_timers(miniat *m, m_timer_id timer) {

	if (!m) {
		THROW(EINVAL, "MiniAT cannot be NULL");
	}

	if ((signed)timer <0 || timer >= m_timer_id_cnt) {
		THROW(ERANGE, "Timer is not within range, got %d expected 0 - %d",
				timer, m_timer_id_cnt-1);
	}

	return *(m->timers.general_purpose_timer[timer].io);
}

PUBLIC void miniat_pins_set_gpio_port(miniat *m, m_gpio_id port, m_wyde pins) {

	m_uword address = M_GPIO_PORT_A;

	if (!m) {
		THROW(EINVAL, "MiniAT cannot be NULL");
	}

	if ((signed)port < 0 || port >= m_gpio_id_cnt) {
		THROW(ERANGE, "Port is not within range, got %d expected 0 - %d",
				port, m_gpio_id_cnt-1);
	}

	address += port;
	m_memory_write(m, address, pins.u);
}

PUBLIC m_wyde miniat_pins_get_gpio_port(miniat *m, m_gpio_id port) {

	m_uword pins = 0;
	m_uword address = M_GPIO_PORT_A;

	if (!m) {
		THROW(EINVAL, "MiniAT cannot be NULL");
	}

	if ((signed)port < 0 || port >= m_gpio_id_cnt) {
		THROW(ERANGE, "Port is not within range, got %d expected 0 - %d",
				port, m_gpio_id_cnt-1);
	}

	address += port;
	m_memory_read(m, address, &pins);
	return *((m_wyde *) (&pins));
}

PUBLIC m_bus *miniat_conector_bus_get(miniat *m) {

	if (!m) {
		THROW(EINVAL, "MiniAT cannot be NULL");
	}

	return &m->bus.pub;
}
