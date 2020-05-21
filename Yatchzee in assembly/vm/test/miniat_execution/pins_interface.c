#include <stdio.h>
#include <stdlib.h>

#include "../../src/miniat_defines.h"
#include "../../src/miniat_hazards.h"
#include "../../src/miniat_registers.h"
#include "../../src/miniat_structures.h"
#include "miniat/miniat.h"
#include "CuTest.h"

void t_error___pins_interface_test(CuTest *tc) {

	unsigned i;
	FILE *bin_file;
	miniat *miniat;

	bin_file = fopen(BIN_OUT_DIR"/bus_read.bin", "r+b");
	CuAssert(tc, "Should not be NULL", bin_file != NULL);

	miniat = miniat_new (bin_file, NULL);
	CuAssert(tc, "Should not be NULL", miniat != NULL);

	m_wyde port_pins = {
		.u = 42
	};

	m_timers_io timer_pins = { .all = 0 };
	m_byte xint_pin;
	timer_pins.line.input = timer_pins.line.output = 1;
	m_bus bus;
	m_bus bus_cmp;

	for (i=m_gpio_id_A; i < m_gpio_id_cnt; i++) {
		port_pins.u+=i;
		miniat_pins_set_gpio_port(miniat, i, port_pins);
	}

	port_pins.u = 42;
	for (i=m_gpio_id_A; i < m_gpio_id_cnt; i++) {
		port_pins.u += i;
		CuAssert(tc, "Pins not right", miniat_pins_get_gpio_port(miniat, i).u == port_pins.u);
	}

	for (i=m_timer_id_0; i<m_timer_id_cnt; i++) {
		miniat_pins_set_timers(miniat, i, timer_pins);
	}

	timer_pins.line.output = 0;

	for (i=m_timer_id_0; i<m_timer_id_cnt; i++) {
		CuAssert(tc, "Pins not right", miniat_pins_get_timers(miniat, i).all == timer_pins.all);
	}

	for (i=m_xint_id_0; i < m_xint_id_cnt; i++) {
		xint_pin.u = ((unsigned)~0<<1) ^ (i & 1);
		miniat_pins_set_xint(miniat, i, xint_pin);
	}

	for (i=m_xint_id_0; i < m_xint_id_cnt; i++) {
		xint_pin.u = i & 1;
		CuAssert(tc, "Pins not right", miniat_pins_get_xint(miniat, i).u == xint_pin.u);
	}


	bus.ack = M_HIGH;
	bus.rW = M_HIGH;
	bus.req = M_HIGH;
	bus.address = 32; /* Only controllable from inside the VM */
	bus.data = 64;

	miniat_pins_bus_set(miniat, bus);

	bus_cmp = miniat_pins_bus_get(miniat);

	CuAssert(tc, "Bus incorrect ack", bus.ack == bus_cmp.ack);
	CuAssert(tc, "Bus incorrect rW", bus.rW != bus_cmp.rW);
	CuAssert(tc, "Bus incorrect req", bus.req != bus_cmp.req);
	CuAssert(tc, "Bus incorrect address", bus.address != bus_cmp.address);
	CuAssert(tc, "Bus incorrect data", bus.data == bus_cmp.data);

}
