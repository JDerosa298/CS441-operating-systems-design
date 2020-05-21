#include "miniat_timers.h"

#include <string.h>

#include "miniat/miniat.h"
#include "miniat/miniat_error.h"
#include "miniat_defines.h"
#include "miniat_enums.h"
#include "miniat_memory.h"
#include "miniat_structures.h"
#include "miniat_typedefs.h"
#include "miniat_unions.h"

void m_timers_init(miniat *m) {

	m_timer_control *control = NULL;
	m_timers_io *timer_io = NULL;

	memset(&(m->timers), 0, sizeof(m_timers));

	m_watchdog *watchdog = &(m->timers.watchdog);
	m_timer *timers = (m_timer *)(&(m->timers.general_purpose_timer));

	/*Initialize General Purpose Timer Compares by setting the pointers equal to their spot in the memory map*/
	timers[m_timer_id_0].compare = (m_timer_compare *)&(m->mem.data[M_CSR_TIMER0_COMPARE]);
	timers[m_timer_id_1].compare = (m_timer_compare *)&(m->mem.data[M_CSR_TIMER1_COMPARE]);
	timers[m_timer_id_2].compare = (m_timer_compare *)&(m->mem.data[M_CSR_TIMER2_COMPARE]);
	timers[m_timer_id_3].compare = (m_timer_compare *)&(m->mem.data[M_CSR_TIMER3_COMPARE]);

	/*initialize General Purpose Controls*/
	control = (m_timer_control *)&(m->mem.data[M_CSR_TIMER_CONTROL]);
	timers[m_timer_id_0].control = &(control[0]);
	timers[m_timer_id_1].control = &(control[1]);
	timers[m_timer_id_2].control = &(control[2]);
	timers[m_timer_id_3].control = &(control[3]);

	/*int the IO*/
	timer_io = (m_timers_io *)&(m->mem.data[M_CSR_TIMER_IO]);
	timers[m_timer_id_0].io = &(timer_io[0]);
	timers[m_timer_id_1].io = &(timer_io[1]);
	timers[m_timer_id_2].io = &(timer_io[2]);
	timers[m_timer_id_3].io = &(timer_io[3]);

	/*Initialize General Purpose Timer Counters*/
	timers[m_timer_id_0].count = &(m->mem.data[M_CSR_TIMER0_COUNT]);
	timers[m_timer_id_1].count = &(m->mem.data[M_CSR_TIMER1_COUNT]);
	timers[m_timer_id_2].count = &(m->mem.data[M_CSR_TIMER2_COUNT]);
	timers[m_timer_id_3].count = &(m->mem.data[M_CSR_TIMER3_COUNT]);

	/*Initialize Watchdog Compare*/
	watchdog->compare = (m_watchdog_compare *)&(m->mem.data[M_CSR_WATCHDOG_COMPARE]);
	watchdog->control = (m_watchdog_control *)&(m->mem.data[M_CSR_WATCHDOG_CONTROL]);
}

void m_timers_clock(miniat *m) {

	m_timers_watchdog_clock(m);
	m_timers_general_purpose_timers_clock(m);
}

void m_timers_general_purpose_timers_clock(miniat *m) {

	int i;
	m_timer *timer;
	m_uword interrupt = 0;
	m_uword tmp = 0;
	m_timer *timers = m->timers.general_purpose_timer;

	for (i = 0; i < M_NUM_OF_TIMERS; i++) {

		timer = &(timers[i]);

		if (timer->control->reset) {
			timer->control->reset = 0;
			(*timer->count) = 0;
			timer->internal_count = 0;
			timer->control->enable = 0;
		}
		else if (timer->control->enable) {

			if ((timer->control->mode == m_timer_count) && (!timer->old_in && timer->io->line.input)) {
				timer->internal_count++;
			}
			else if (timer->control->mode != m_timer_count) {
				timer->internal_count++;
			}

			/*
			 * Since the pre-scale is calculated as CPS + 1 a greater than
			 * will suffice and the addition can be dropped since X >= N + 1
			 * is equivalent to X > N
			 */
			if (timer->internal_count > timer->control->CPS) {

				/* Reset the "modulus" counter */
				timer->internal_count = 0;

				/* Pre-scaled tick happens */
				(*timer->count)++;
			}

			switch (timer->control->mode) {
			case m_timer_time:

				/*
				 * Check to see if the timer went off, if
				 * it did, reset it, disable the timer
				 * and trip the interrupt.
				 */
				if ((*timer->count) >= timer->compare->all) {

					interrupt |= 1 << (M_INT_TIMER0 + i);
					timer->internal_count = 0;
					*(timer->count) = 0;
					timer->control->enable = 0;
				}
				break;

			case m_timer_count:

				if (!timer->old_in && timer->io->line.input) {

					/*
					 * Check to see if the timer went off, if
					 * it did, reset it, disable the timer
					 * and trip the interrupt.
					 */
					if ((*timer->count) >= timer->compare->all) {

						interrupt |= 1 << (M_INT_TIMER0 + i);
						timer->internal_count = 0;
						(*timer->count) = 0;
						timer->control->enable = 0;
					}
				}
				break;

			case m_timer_toggle:

				/*
				 * Check to see if the timer went off, if
				 * it did toggle the pin
				 */
				if (*(timer->count) >= timer->compare->all) {
					timer->internal_count = 0;
					*(timer->count) = 0;
					timer->io->line.output = !timer->io->line.output;
				}
				break;

			case m_timer_pwm:

				/*
				 * For example a wavelength of 10 with a 3 cycle duty would produce the following waveform...
				 *
				 *            level ---_______
				 * pre-scaled cycle 0123456789
				 */
				if (*(timer->count) >= timer->compare->wavelength) {

					timer->internal_count = 0;
					*(timer->count) = 0;
				}

				if (*(timer->count) < timer->compare->duty) {
					timer->io->line.output = M_HIGH;
				}
				else {
					timer->io->line.output = M_LOW;
				}
				break;

			} /*end switch on timer mode*/
		} /* end else if enabled timers */

		/*Update all the timers old_in field*/
		timer->old_in = timer->io->line.input;

	} /*end for all timers loop*/

	/*After running all the timers, or in any interrupts*/
	if (interrupt) {
		m_memory_read(m, M_CSR_INT_FLAG_LOW,&tmp);
		tmp |= interrupt;
		m_memory_write(m, M_CSR_INT_FLAG_LOW,tmp);
	}
}

void m_timers_watchdog_clock(miniat *m) {

	m_watchdog *wd = &(m->timers.watchdog);

	/*
	 * If the reset bit is high, clear the control m_word of the watchdog timer to clear the following bits:
	 * Enable, Reset, Count and the compare word which consists of CPS and Compare.
	 * This has the side effect of clearing reserved bits, if someone was hiding data, we wiped it.
	 */
	if (wd->control->reset) {
		wd->control->enable = M_LOW;
		wd->internal_count = M_LOW;
		wd->control->count = M_LOW;
		wd->control->reset = M_LOW;
	}

	/*
	 * Else if I am enabled, provide CPS ticks to count and check to see if count >= compare
	 */
	else if (wd->control->enable) {

		/* Increment the private counter */
		(wd->internal_count)++;

		/*
		 * If the internal count % 2^CPS (1 << CPS) == 0, then provide a CPS tick to count
		 */
		if (!(wd->internal_count % (1 << wd->compare->CPS)) && wd->internal_count != 0) {
			(wd->control->count)++;
		}

		/*
		 * If the count is greater than or equal to compare, trip the interrupt
		 */
		if (wd->control->count >= wd->compare->value) {
			wd->control->reset = M_HIGH;

			m_uword watchdog_ivt_pointer;

			/*Check to see if IVBT is NULL pointer*/
			m_memory_read(m, M_IVT_WATCHDOG, &watchdog_ivt_pointer);

			m_uword interrupt = watchdog_ivt_pointer ? M_INT_WATCHDOG : M_INT_RESET;

			m_memory_read(m, M_CSR_INT_FLAG_LOW, &watchdog_ivt_pointer);
			watchdog_ivt_pointer |= 1 << interrupt;
			m_memory_write(m, M_CSR_INT_FLAG_LOW, watchdog_ivt_pointer);
		}
	}
}
