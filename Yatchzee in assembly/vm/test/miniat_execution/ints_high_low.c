#include <stdio.h>
#include <stdlib.h>

#include "../../src/miniat_defines.h"
#include "../../src/miniat_hazards.h"
#include "../../src/miniat_registers.h"
#include "../../src/miniat_structures.h"
#include "miniat/miniat.h"
#include "CuTest.h"

void t_exec___ints_high_low (CuTest *tc) {

	FILE *bin_file;
	miniat *miniat;
	int cycles = 0;

	/* Read in the binary testing file and instantiate a miniat. */
	char *path = BIN_OUT_DIR"/ints_high_low.bin";
	bin_file = fopen(path, "r+b");
	CuAssert(tc, "Should not be NULL", bin_file != NULL);

	miniat = miniat_new (bin_file, NULL);
	CuAssert(tc, "Should not be NULL", miniat != NULL);

	do {
		miniat_clock(miniat);
		cycles++;
	} while (miniat->reg[5] != 1 && cycles < 100);
	CuAssert(tc, "Interrupt path is incorrect... r5 should be 1 by now", miniat->reg[5] == 1 && cycles < 100);

	do {
		miniat_clock(miniat);
		cycles++;
	} while (miniat->reg[5] != 2 && cycles < 100);
	CuAssert(tc, "Interrupt path is incorrect... r5 should be 2 by now", miniat->reg[5] == 2 && cycles < 100);

	do {
		miniat_clock(miniat);
		cycles++;
	} while (miniat->reg[5] != 3 && cycles < 100);
	CuAssert(tc, "Interrupt path is incorrect... r5 should be 3 by now", miniat->reg[5] == 3 && cycles < 100);

	do {
		miniat_clock(miniat);
		cycles++;
	} while (miniat->reg[5] != 4 && cycles < 100);
	CuAssert(tc, "Interrupt path is incorrect... r5 should be 4 by now", miniat->reg[5] == 4 && cycles < 100);

	do {
		miniat_clock(miniat);
		cycles++;
	} while (miniat->reg[5] != 5 && cycles < 100);
	CuAssert(tc, "Interrupt path is incorrect... r5 should be 5 by now", miniat->reg[5] == 5 && cycles < 100);

	miniat_free(miniat);

	return;
}
