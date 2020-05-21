#include <stdio.h>
#include <stdlib.h>

#include "../../src/miniat_defines.h"
#include "../../src/miniat_hazards.h"
#include "../../src/miniat_registers.h"
#include "../../src/miniat_structures.h"
#include "miniat/miniat.h"
#include "CuTest.h"

void t_error___exec_timers_time(CuTest *tc) {
/* TODO: rewrite with context of correct stall durations */
#if 0
	int i;
	FILE *bin_file;
	miniat *miniat;
	m_uword ticks;
	int tripped = 0;
	bin_file = fopen(BIN_OUT_DIR"/timer_time.bin", "r+b");
	CuAssert(tc, "Should not be NULL", bin_file != NULL);

	miniat = miniat_new (bin_file, NULL);
	CuAssert(tc, "Should not be NULL", miniat != NULL);

	for (i=0; i <= 1000; i++) {

		miniat_clock(miniat);

		if ((miniat->pipeline.writeback.in.NPC == 0x2016 && !tripped) ||
				(miniat->pipeline.writeback.in.NPC == 0x3806)) {

			CuAssert(tc, "CPS Incorrect", miniat->timers.general_purpose_timer[m_timer_id_0].control->CPS == 4);
			CuAssert(tc, "Enable incorrect", miniat->timers.general_purpose_timer[m_timer_id_0].control->enable == 1);
			CuAssert(tc, "Mode incorrect", miniat->timers.general_purpose_timer[m_timer_id_0].control->mode == m_timer_time);
			CuAssert(tc, "Reset should never be !0", miniat->timers.general_purpose_timer[m_timer_id_0].control->reset == 0);
			CuAssert(tc, "Compare incorrect", miniat->timers.general_purpose_timer[m_timer_id_0].compare->all == 10);
			CuAssert(tc, "count incorrect", *(miniat->timers.general_purpose_timer[m_timer_id_0].count) == 0);
			ticks = miniat->ticks;
			tripped = 1;
		}
		else if (miniat->pipeline.fetch.out.NPC == 0x3802) {

			CuAssert(tc, "CPS Incorrect", miniat->timers.general_purpose_timer[m_timer_id_0].control->CPS == 4);
			CuAssert(tc, "Enable incorrect", miniat->timers.general_purpose_timer[m_timer_id_0].control->enable == 0);
			CuAssert(tc, "Mode incorrect", miniat->timers.general_purpose_timer[m_timer_id_0].control->mode == m_timer_time);
			CuAssert(tc, "Reset should never be !0", miniat->timers.general_purpose_timer[m_timer_id_0].control->reset == 0);
			CuAssert(tc, "Compare incorrect", miniat->timers.general_purpose_timer[m_timer_id_0].compare->all == 10);
			CuAssert(tc, "count incorrect", *(miniat->timers.general_purpose_timer[m_timer_id_0].count) == 0);
			CuAssert(tc, "count incorrect", miniat->timers.general_purpose_timer[m_timer_id_0].internal_count == 0);

			CuAssert(tc, "should have a delay of 50 cycles (4 + 1 PS)", (miniat->ticks - ticks) == 50);
		}
	}
	miniat_free(miniat);
#endif
	return;
}

void t_error___exec_timers_count(CuTest *tc) {
/* TODO: rewrite with context of correct stall durations */
#if 0
	int i;
	FILE *bin_file;
	miniat *miniat;
	int tripped = 0;
	int toggle = 0;
	int old;

	bin_file = fopen(BIN_OUT_DIR"/timer_count.bin", "r+b");
	CuAssert(tc, "Should not be NULL", bin_file != NULL);

	miniat = miniat_new (bin_file, NULL);
	CuAssert(tc, "Should not be NULL", miniat != NULL);

	for (i=0; i <= 1000; i++) {

		old = miniat->timers.general_purpose_timer[m_timer_id_0].io->line.input;
		miniat->timers.general_purpose_timer[m_timer_id_0].io->line.input = !miniat->timers.general_purpose_timer[m_timer_id_0].io->line.input;

		if (!old && miniat->timers.general_purpose_timer[m_timer_id_0].io->line.input && miniat->timers.general_purpose_timer[m_timer_id_0].control->enable) {
			toggle++;
		}

		miniat_clock(miniat);

		if ((miniat->pipeline.writeback.in.NPC == 0x2016 && !tripped) ||
				(miniat->pipeline.writeback.in.NPC == 0x3806)) {

			CuAssert(tc, "CPS Incorrect", miniat->timers.general_purpose_timer[m_timer_id_0].control->CPS == 4);
			CuAssert(tc, "Enable incorrect", miniat->timers.general_purpose_timer[m_timer_id_0].control->enable == 1);
			CuAssert(tc, "Mode incorrect", miniat->timers.general_purpose_timer[m_timer_id_0].control->mode == m_timer_count);
			CuAssert(tc, "Reset should never be !0", miniat->timers.general_purpose_timer[m_timer_id_0].control->reset == 0);
			CuAssert(tc, "Compare incorrect", miniat->timers.general_purpose_timer[m_timer_id_0].compare->all == 10);
			CuAssert(tc, "count incorrect", *(miniat->timers.general_purpose_timer[m_timer_id_0].count) == 0);
			tripped = 1;
		}
		else if (miniat->pipeline.fetch.out.NPC == 0x3802) {

			CuAssert(tc, "CPS Incorrect", miniat->timers.general_purpose_timer[m_timer_id_0].control->CPS == 4);
			CuAssert(tc, "Enable incorrect", miniat->timers.general_purpose_timer[m_timer_id_0].control->enable == 0);
			CuAssert(tc, "Mode incorrect", miniat->timers.general_purpose_timer[m_timer_id_0].control->mode == m_timer_count);
			CuAssert(tc, "Reset should never be !0", miniat->timers.general_purpose_timer[m_timer_id_0].control->reset == 0);
			CuAssert(tc, "Compare incorrect", miniat->timers.general_purpose_timer[m_timer_id_0].compare->all == 10);
			CuAssert(tc, "count incorrect", *(miniat->timers.general_purpose_timer[m_timer_id_0].count) == 0);
			CuAssert(tc, "count incorrect", miniat->timers.general_purpose_timer[m_timer_id_0].internal_count == 0);

			CuAssert(tc, "Toggle is not 50", toggle == 50);
			toggle = 0;
		}
	}
	miniat_free(miniat);
#endif
	return;
}

void t_error___exec_timers_toggle(CuTest *tc) {

/* TODO: rewrite with context of correct stall durations */
#if 0
	int i;
	FILE *bin_file;
	miniat *miniat;
	m_uword ticks = 0;
	int tripped = 0;
	int old;
	bin_file = fopen(BIN_OUT_DIR"/timer_toggle.bin", "r+b");
	CuAssert(tc, "Should not be NULL", bin_file != NULL);

	miniat = miniat_new (bin_file, NULL);
	CuAssert(tc, "Should not be NULL", miniat != NULL);

	for (i=0; i <= 1000; i++) {

		old = miniat->timers.general_purpose_timer[m_timer_id_0].io->line.output;

		miniat_clock(miniat);

		if (miniat->pipeline.writeback.in.NPC == 0x2016 && !tripped) {

			CuAssert(tc, "CPS Incorrect", miniat->timers.general_purpose_timer[m_timer_id_0].control->CPS == 4);
			CuAssert(tc, "Enable incorrect", miniat->timers.general_purpose_timer[m_timer_id_0].control->enable == 1);
			CuAssert(tc, "Mode incorrect", miniat->timers.general_purpose_timer[m_timer_id_0].control->mode == m_timer_toggle);
			CuAssert(tc, "Reset should never be !0", miniat->timers.general_purpose_timer[m_timer_id_0].control->reset == 0);
			CuAssert(tc, "Compare incorrect", miniat->timers.general_purpose_timer[m_timer_id_0].compare->all == 10);
			CuAssert(tc, "count incorrect", *(miniat->timers.general_purpose_timer[m_timer_id_0].count) == 0);
			ticks = miniat->ticks;
			tripped = 1;
		}
		else if (((miniat->ticks - ticks) % 50) == 0) {
			CuAssert(tc, "aww snap", old != miniat->timers.general_purpose_timer[m_timer_id_0].io->line.output);
		}
		else {
			CuAssert(tc, "aww snap", old == miniat->timers.general_purpose_timer[m_timer_id_0].io->line.output);
		}



	}
	miniat_free(miniat);
#endif
	return;
}

void t_error___exec_timers_pwm(CuTest *tc) {
/* TODO: rewrite with context of correct stall durations */
#if 0
	int i;
	FILE *bin_file;
	miniat *miniat;
	m_uword ticks = 0;
	int tripped = 0;
	int old = 0;
	bin_file = fopen(BIN_OUT_DIR"/timer_pwm.bin", "r+b");
	CuAssert(tc, "Should not be NULL", bin_file != NULL);

	miniat = miniat_new (bin_file, NULL);
	CuAssert(tc, "Should not be NULL", miniat != NULL);

	for (i=0; i <= 1000; i++) {

		if (miniat->pipeline.writeback.in.NPC == 0x2018) {
			old = miniat->timers.general_purpose_timer[m_timer_id_0].io->line.output;
		}

		miniat_clock(miniat);

		if (miniat->pipeline.writeback.in.NPC == 0x2016 && !tripped) {

			CuAssert(tc, "CPS Incorrect", miniat->timers.general_purpose_timer[m_timer_id_0].control->CPS == 0);
			CuAssert(tc, "Enable incorrect", miniat->timers.general_purpose_timer[m_timer_id_0].control->enable == 1);
			CuAssert(tc, "Mode incorrect", miniat->timers.general_purpose_timer[m_timer_id_0].control->mode == m_timer_pwm);
			CuAssert(tc, "Reset should never be !0", miniat->timers.general_purpose_timer[m_timer_id_0].control->reset == 0);
			CuAssert(tc, "Compare incorrect", miniat->timers.general_purpose_timer[m_timer_id_0].compare->duty == 50);
			CuAssert(tc, "Compare incorrect", miniat->timers.general_purpose_timer[m_timer_id_0].compare->wavelength == 100);
			CuAssert(tc, "count incorrect", *(miniat->timers.general_purpose_timer[m_timer_id_0].count) == 0);
			ticks = miniat->ticks;
			tripped = 1;
			old = 1;
		}
		else if (tripped && ((miniat->ticks - ticks) % 50) == 0) {
			CuAssert(tc, "aww snap", old != miniat->timers.general_purpose_timer[m_timer_id_0].io->line.output);
			old = miniat->timers.general_purpose_timer[m_timer_id_0].io->line.output;
		}
		else if (tripped){
			CuAssert(tc, "aww snap", old == miniat->timers.general_purpose_timer[m_timer_id_0].io->line.output);
		}

	}
	miniat_free(miniat);
#endif
	return;
}
