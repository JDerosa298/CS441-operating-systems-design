#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../../src/miniat_defines.h"
#include "../../src/miniat_hazards.h"
#include "../../src/miniat_registers.h"
#include "../../src/miniat_structures.h"
#include "miniat/miniat.h"
#include "CuTest.h"

void t_error___port_input_w_cache_script(CuTest *tc) {

	FILE *bin_file;
	miniat *m;
	int cycles;
	m_wyde base;

	char *bin_path = BIN_OUT_DIR"/port_input.bin";
	char *cache_path = CACHE_DIR"/hit.lua";

	bin_file = fopen(bin_path, "r+b");
	CuAssert(tc, "Open binary file", bin_file != NULL);

	m = miniat_new (bin_file, cache_path);
	CuAssert(tc, "Create MiniAT instance", m != NULL);

	/* Allow up tp 100 cycles for the ports to be set as input by the ASM. */
	cycles = 0;
	do {
		base.u = 0xBA53;
		miniat_pins_set_gpio_port(m, m_gpio_id_A, base);
		base.u++;
		miniat_pins_set_gpio_port(m, m_gpio_id_B, base);
		base.u++;
		miniat_pins_set_gpio_port(m, m_gpio_id_C, base);
		base.u++;
		miniat_pins_set_gpio_port(m, m_gpio_id_D, base);
		miniat_clock(m);
		cycles++;
	} while (cycles < 100);

	base.u = 0xBA53;
	CuAssert(tc, "LOAD from port set register correctly", m->reg[1] == base.u + 0);
	CuAssert(tc, "LOAD from port set register correctly", m->reg[2] == base.u + 1);
	CuAssert(tc, "LOAD from port set register correctly", m->reg[3] == base.u + 2);
	CuAssert(tc, "LOAD from port set register correctly", m->reg[4] == base.u + 3);

	miniat_free(m);
	return;
}

void t_error___port_input_wo_cache_script(CuTest *tc) {

	FILE *bin_file;
	miniat *m;
	int cycles;
	m_wyde base;

	char *bin_path = BIN_OUT_DIR"/port_input.bin";

	bin_file = fopen(bin_path, "r+b");
	CuAssert(tc, "Open binary file", bin_file != NULL);

	m = miniat_new (bin_file, NULL);
	CuAssert(tc, "Create MiniAT instance", m != NULL);

	/* Allow up tp 100 cycles for the ports to be set as input by the ASM. */
	cycles = 0;
	do {
		base.u = 0xBA53;
		miniat_pins_set_gpio_port(m, m_gpio_id_A, base);
		base.u++;
		miniat_pins_set_gpio_port(m, m_gpio_id_B, base);
		base.u++;
		miniat_pins_set_gpio_port(m, m_gpio_id_C, base);
		base.u++;
		miniat_pins_set_gpio_port(m, m_gpio_id_D, base);
		miniat_clock(m);
		cycles++;
	} while (cycles < 100);

	base.u = 0xBA53;
	CuAssert(tc, "LOAD from port set register correctly", m->reg[1] == base.u + 0);
	CuAssert(tc, "LOAD from port set register correctly", m->reg[2] == base.u + 1);
	CuAssert(tc, "LOAD from port set register correctly", m->reg[3] == base.u + 2);
	CuAssert(tc, "LOAD from port set register correctly", m->reg[4] == base.u + 3);

	miniat_free(m);
	return;
}

