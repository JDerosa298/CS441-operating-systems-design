#include <stdio.h>
#include <stdlib.h>

#include "../../src/miniat_defines.h"
#include "../../src/miniat_hazards.h"
#include "../../src/miniat_registers.h"
#include "../../src/miniat_structures.h"
#include "miniat/miniat.h"
#include "CuTest.h"

void t_error___exec_watchdog(CuTest *tc) {
/* TODO: rewrite with context of correct stall durations */
#if 0
	int i;
	FILE *bin_file;
	miniat *miniat;
	m_uword ticks;

	bin_file = fopen(BIN_OUT_DIR"/watchdog.bin", "r+b");
	CuAssert(tc, "Should not be NULL", bin_file != NULL);

	miniat = miniat_new (bin_file, NULL);
	CuAssert(tc, "Should not be NULL", miniat != NULL);

	for (i=0; i <= 1000; i++) {

		miniat_clock(miniat);

		/*
		 * Since we're checking writeback in, the code were checking
		 * for still hasn't executed so we need to add 1 more to the ticks
		 * to account for that code actually being executed or check for NPC + 2
		 * for the instructions whose affect we are looking for.
		 */
		if (miniat->pipeline.writeback.in.NPC == 0x2016) {
			CuAssert(tc, "Count should be 0", miniat->timers.watchdog.control->count == 0);
			CuAssert(tc, "WD should be enabled", miniat->timers.watchdog.control->enable == 1);
			ticks = miniat->ticks;
		}
		else if (miniat->pipeline.writeback.in.NPC == 0x2022) {
			CuAssert(tc, "Count should be 0", miniat->timers.watchdog.control->count == 0);
			CuAssert(tc, "WD should be enabled", miniat->timers.watchdog.control->enable == 1);
			ticks = miniat->ticks;
		}
		else if (miniat->reg[M_REGISTER_PC] == 0x2018) {
			CuAssert(tc, "Count should be 40", miniat->timers.watchdog.control->count == 10);
			CuAssert(tc, "Timer should occur every 40 ticks", ((miniat->ticks - ticks) == 40));
		}
	}
	miniat_free(miniat);
#endif
	return;
}

