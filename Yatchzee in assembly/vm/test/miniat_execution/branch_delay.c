/*
 * This code is meant to test the behavior of the delay slots when
 * branching/ hinting true or false.
 *
 * The expected behavior is as follows:
 *    Hint      Result      Delay Slot(s)       Other Behavior
 *    -------------------------------------------------------------------------
 *    True      True             1              N/A
 *    False     True             3              N/A
 *    True      False            N/A            Flush should occur
 *    False     False            N/A            Code should fall right through.
 *
 *
 * Extra Note:
 *   In all tests below, the expected values of r5, r6 and r7 are all 1.
 *   These registers are incremented in turn at the branch target.
 *   The rational is that, if the flush (case 3 above) doesn't work properly,
 *   then they would be incremented multiple times.
 */

#include <stdio.h>
#include <stdlib.h>

#include "../../src/miniat_defines.h"
#include "../../src/miniat_hazards.h"
#include "../../src/miniat_registers.h"
#include "../../src/miniat_structures.h"
#include "miniat/miniat.h"
#include "CuTest.h"

void t_exec___branch_delay(CuTest *tc) {

	FILE *bin_file;
	miniat *miniat;

	/* Read in the binary testing file and instantiate a miniat. */
	char *path = BIN_OUT_DIR"/branch_delay.bin";
	bin_file = fopen(path, "r+b");
	CuAssert(tc, "Should not be NULL", bin_file != NULL);

	miniat = miniat_new (bin_file, CACHE_DIR"/hit.lua");
	CuAssert(tc, "Should not be NULL", miniat != NULL);

	/* Process instructions until we reach the end of section 1. */
	while (miniat->reg[100] != 1) {
		miniat_clock(miniat);
	}

	/*
	 * Now lets check the registers to see how many delay slots we ran.
	 *
	 * Instruction Sequence:                      (Hint True Correctly)
	 *   BRA [!done_1]
	 *   r1 =r1 + 1
	 *   r2 =r2 + 1
	 *   r3 =r3 + 1
	 *   r4 =r4 + 1
	 *
	 * Starting values:
	 *   r1: '1'
	 *   r2: '5'
	 *   r3: 'A'
	 *   r4: 'D'
	 *
	 * Expected ending values:
	 *   r1: '2'
	 *   r2: '5'
	 *   r3: 'A'
	 *   r4: 'D'
	 */
	CuAssert(tc, "Register 1 should be '2'", miniat->reg[1] == '2');
	CuAssert(tc, "Register 2 should be '5'", miniat->reg[2] == '5');
	CuAssert(tc, "Register 3 should be 'A'", miniat->reg[3] == 'A');
	CuAssert(tc, "Register 4 should be 'D'", miniat->reg[4] == 'D');
	CuAssert(tc, "Register 5 should be 1",   miniat->reg[5] ==  1 );
	CuAssert(tc, "Register 6 should be 1",   miniat->reg[6] ==  1 );
	CuAssert(tc, "Register 7 should be 1",   miniat->reg[7] ==  1 );

	/* Process instructions until we reach the end of section 2. */
	while (miniat->reg[100] != 2) {
		miniat_clock(miniat);
	}

	/*
	 * Now lets check the registers to see how many delay slots we ran.
	 *
	 * Instruction Sequence:                     (Hint False Correctly)
	 *   BRANE [!done_2 : F], r0 != r0
	 *   r1 =r1 + 1
	 *   r2 =r2 + 1
	 *   r3 =r3 + 1
	 *   r4 =r4 + 1
	 *
	 * Starting values:
	 *   r1: '1'
	 *   r2: '5'
	 *   r3: 'A'
	 *   r4: 'D'
	 *
	 * Expected ending values:
	 *   r1: '2'
	 *   r1: '6'
	 *   r3: 'B'
	 *   r4: 'E'
	 */
	CuAssert(tc, "Register 1 should be '2'", miniat->reg[1] == '2');
	CuAssert(tc, "Register 2 should be '6'", miniat->reg[2] == '6');
	CuAssert(tc, "Register 3 should be 'B'", miniat->reg[3] == 'B');
	CuAssert(tc, "Register 4 should be 'E'", miniat->reg[4] == 'E');
	CuAssert(tc, "Register 5 should be 1",   miniat->reg[5] ==  1 );
	CuAssert(tc, "Register 6 should be 1",   miniat->reg[6] ==  1 );
	CuAssert(tc, "Register 7 should be 1",   miniat->reg[7] ==  1 );

	/* Process instructions until we reach the end of section 3. */
	while (miniat->reg[100] != 3) {
		miniat_clock(miniat);
	}

	/*
	 * Now lets check the registers to see how many delay slots we ran.
	 *
	 * Instruction Sequence:                   (Hint False Incorrectly)
	 *   BRAE [!done_3 : F], r0 == r0
	 *   r1 =r1 + 1
	 *   r2 =r2 + 1
	 *   r3 =r3 + 1
	 *   r4 =r4 + 1
	 *
	 * Starting values:
	 *   r1: '1'
	 *   r2: '5'
	 *   r3: 'A'
	 *   r4: 'D'
	 *
	 * Expected ending values:
	 *   r1: '2'
	 *   r2: '6'
	 *   r3: 'B'
	 *   r4: 'D'
	 */
	CuAssert(tc, "Register 1 should be '2'", miniat->reg[1] == '2');
	CuAssert(tc, "Register 2 should be '6'", miniat->reg[2] == '6');
	CuAssert(tc, "Register 3 should be 'B'", miniat->reg[3] == 'B');
	CuAssert(tc, "Register 4 should be 'D'", miniat->reg[4] == 'D');
	CuAssert(tc, "Register 5 should be 1",   miniat->reg[5] ==  1 );
	CuAssert(tc, "Register 6 should be 1",   miniat->reg[6] ==  1 );
	CuAssert(tc, "Register 7 should be 1",   miniat->reg[7] ==  1 );

	/* Process instructions until we reach the end of section 4. */
	while (miniat->reg[100] != 4) {
		miniat_clock(miniat);
	}

	/*
	 * Now lets check the registers to see how many delay slots we ran.
	 *
	 * Instruction Sequence:                    (Hint True Incorrectly)
	 *   BRAE [!done_4 : T], r0 == r0
	 *   r1 =r1 + 1
	 *   r2 =r2 + 1
	 *   r3 =r3 + 1
	 *   r4 =r4 + 1
	 *
	 * Starting values:
	 *   r1: '1'
	 *   r2: '5'
	 *   r3: 'A'
	 *   r4: 'D'
	 *
	 * Expected ending values:
	 *   r1: '2'
	 *   r2: '6'
	 *   r3: 'B'
	 *   r4: 'E'
	 */
	CuAssert(tc, "Register 1 should be '2'", miniat->reg[1] == '2');
	CuAssert(tc, "Register 2 should be '6'", miniat->reg[2] == '6');
	CuAssert(tc, "Register 3 should be 'B'", miniat->reg[3] == 'B');
	CuAssert(tc, "Register 4 should be 'E'", miniat->reg[4] == 'E');
	CuAssert(tc, "Register 5 should be 1",   miniat->reg[5] ==  1 );
	CuAssert(tc, "Register 6 should be 1",   miniat->reg[6] ==  1 );
	CuAssert(tc, "Register 7 should be 1",   miniat->reg[7] ==  1 );

	/* Process instructions until we reach the end of section 5. */
	while (miniat->reg[100] != 5) {
		miniat_clock(miniat);
	}

	/*
	 * Now lets check the registers to see how many delay slots we ran.
	 *
	 * Instruction Sequence:         ((Explicitly) Hint True Correctly)
	 *   BRAE [!done_5 : F], r0 == r0
	 *   r1 =r1 + 1
	 *   r2 =r2 + 1
	 *   r3 =r3 + 1
	 *   r4 =r4 + 1
	 *
	 * Starting values:
	 *   r1: '1'
	 *   r2: '5'
	 *   r3: 'A'
	 *   r4: 'D'
	 *
	 * Expected ending values:
	 *   r1: '2'
	 *   r2: '5'
	 *   r3: 'A'
	 *   r4: 'D'
	 */
	CuAssert(tc, "Register 1 should be '2'", miniat->reg[1] == '2');
	CuAssert(tc, "Register 2 should be '5'", miniat->reg[2] == '5');
	CuAssert(tc, "Register 3 should be 'A'", miniat->reg[3] == 'A');
	CuAssert(tc, "Register 4 should be 'D'", miniat->reg[4] == 'D');
	CuAssert(tc, "Register 5 should be 1",   miniat->reg[5] ==  1 );
	CuAssert(tc, "Register 6 should be 1",   miniat->reg[6] ==  1 );
	CuAssert(tc, "Register 7 should be 1",   miniat->reg[7] ==  1 );

	miniat_free(miniat);
	return;
}
