#include <stdio.h>
#include <stdlib.h>

#include "../../src/miniat_defines.h"
#include "../../src/miniat_hazards.h"
#include "../../src/miniat_registers.h"
#include "../../src/miniat_structures.h"
#include "miniat/miniat.h"
#include "CuTest.h"

void t_exec___flush (CuTest *tc) {

	FILE *bin_file;
	miniat *miniat;
	int cycles = 0;

	/* Read in the binary testing file and instantiate a miniat. */
	char *path = BIN_OUT_DIR"/flush.bin";
	bin_file = fopen(path, "r+b");
	CuAssert(tc, "Should not be NULL", bin_file != NULL);

	miniat = miniat_new (bin_file, CACHE_DIR"/hit.lua");
	CuAssert(tc, "Should not be NULL", miniat != NULL);

	/*
	 * Starting values:
	 *   r1: 'B'
	 *   r2: 'n'
	 *   r3: 'p'
	 *   r4: 't'
	 *
	 * Expected ending values:
	 *   r1: 'A'
	 *   r2: 'm'
	 *   r3: 'o'
	 *   r4: 's'
	 */

	/* Process instructions until we reach the end of section 1. */
	while (miniat->reg[100] != 1) {
		miniat_clock(miniat);
		cycles++;
	}

	CuAssert(tc, "Register 1 should be 'A'", miniat->reg[1] == 'A');
	CuAssert(tc, "Register 2 should be 'm'", miniat->reg[2] == 'm');
	CuAssert(tc, "Register 3 should be 'o'", miniat->reg[3] == 'o');
	CuAssert(tc, "Register 4 should be 's'", miniat->reg[4] == 's');

	CuAssertIntEquals_Msg(tc, "Should have completed 15 cycles", 15, cycles);


	/* Process instructions until we reach the end of section 2. */
	while (miniat->reg[100] != 2) {
		miniat_clock(miniat);
		cycles++;
	}

	CuAssert(tc, "Register 1 should be 'A'", miniat->reg[1] == 'A');
	CuAssert(tc, "Register 2 should be 'm'", miniat->reg[2] == 'm');
	CuAssert(tc, "Register 3 should be 'o'", miniat->reg[3] == 'o');
	CuAssert(tc, "Register 4 should be 's'", miniat->reg[4] == 's');

	CuAssertIntEquals_Msg(tc, "Should have completed 27 cycles", 27, cycles);

	miniat_free(miniat);

	return;
}
