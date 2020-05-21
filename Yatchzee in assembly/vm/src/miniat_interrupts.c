#include "miniat_interrupts.h"

#include <stdlib.h>
#include <string.h>

#include "miniat/miniat.h"
#include "miniat_defines.h"
#include "miniat/miniat_error.h"
#include "miniat_enums.h"
#include "miniat_pipeline.h"
#include "miniat_structures.h"
#include "miniat_typedefs.h"
#include "miniat_unions.h"

static void m_interrupts_load_ivbt(miniat *m, m_uword trans_bits_l,
		m_uword trans_bits_h);
static int m_interrupts_priority_scan(m_uword in_register_l,
		m_uword in_register_h);

void m_interrupts_init(miniat *m) {

	m->interrupts.ier_l = &(m->mem.data[M_CSR_INT_ENABLE_LOW]);
	m->interrupts.ier_h = &(m->mem.data[M_CSR_INT_ENABLE_HIGH]);
	m->interrupts.ifr_l = &(m->mem.data[M_CSR_INT_FLAG_LOW]);
	m->interrupts.ifr_h = &(m->mem.data[M_CSR_INT_FLAG_HIGH]);
	m->interrupts.sys_reg = &(m->mem.data[M_CSR_SYSTEM_REGISTER]);

	return;
}

/**
 * Find the lowest order set bit (if any) in a 32 bit value.
 * @note
 *  Big O Log(n) -- Backed by binary bitwise search
 *
 * @param bits
 *  The 32 bit value to scan
 * @return
 *  The bit position of the lowest set bit.  -1 if no bits
 */
static int m_util_priority_scan_lowest(m_uword bits) {

	int n;

	if (bits == 0) {
		return -1;
	}

	n = 0;

	if (!(bits & 0x0000FFFF)) { n += 16; bits >>= 16; }
	if (!(bits & 0x000000FF)) { n +=  8; bits >>=  8; }
	if (!(bits & 0x0000000F)) { n +=  4; bits >>=  4; }
	if (!(bits & 0x00000003)) { n +=  2; bits >>=  2; }
	if (!(bits & 0x00000001))   n +=  1;

	return n;
}

/**
 * Find the highest order set bit (if any) in a 32 bit value.
 * @note
 *  Big O Log(n) -- Backed by binary bitwise search
 *
 * @param bits
 *  The 32 bit value to scan
 * @return
 *  The bit position of the highest set bit.  -1 if no bits
 */
static int m_util_priority_scan_highest(m_uword bits) {

	int n;

	if (bits == 0) {
		return -1;
	}

	n = 0;

	if (bits & 0xFFFF0000) { n += 16; bits >>= 16; }
	if (bits & 0x0000FF00) { n +=  8; bits >>=  8; }
	if (bits & 0x000000F0) { n +=  4; bits >>=  4; }
	if (bits & 0x0000000C) { n +=  2; bits >>=  2; }
	if (bits & 0x00000002)   n +=  1;

	return n;
}

void m_interrupts_handle_interrupt(miniat *m) {

	m_uword *ier_l;
	m_uword *ier_h;
	m_uword *ifr_l;
	m_uword *ifr_h;
	m_uword *oifr_l;
	m_uword *oifr_h;
	m_uword *irr_l;
	m_uword *irr_h;
	m_uword *iqr_l;
	m_uword *iqr_h;
	m_ivbt *ivbt;

	m_uword trans_bits_l = 0;
	m_uword trans_bits_h = 0;
	m_uword running_int = 0;
	m_uword high_queue = 0;

	/* Set up pointers for convenience */
	ier_l = m->interrupts.ier_l;
	ier_h = m->interrupts.ier_h;

	ifr_l = m->interrupts.ifr_l;
	ifr_h = m->interrupts.ifr_h;

	oifr_l = &(m->interrupts.oifr_l);
	oifr_h = &(m->interrupts.oifr_h);

	irr_l = &(m->interrupts.irr_l);
	irr_h = &(m->interrupts.irr_h);

	iqr_l = &(m->interrupts.iqr_l);
	iqr_h = &(m->interrupts.iqr_h);

	ivbt = m->interrupts.ivbt;

	/*
	 * This checks to see if interrupts are globally enabled and if anything has changed state
	 * or needs to be scheduled
	 */
	if (m->csr->ie &&
			((*ier_l & *ifr_l) || (*ier_h & *ifr_h)
			|| (*iqr_l & *ier_l) || (*iqr_h & *ier_h))) {

		trans_bits_l = *ier_l & ~(*oifr_l) & *ifr_l;
		trans_bits_h = *ier_h & ~(*oifr_h) & *ifr_h;

		/* if anything transitioned it needs some scheduling */
		if (trans_bits_l || trans_bits_h) {

			/* Load the ivbt with information from the ivt */
			m_interrupts_load_ivbt(m, trans_bits_l, trans_bits_h);

			/* queue everything that isn't already running and that transitioned over */
			*iqr_l |= ~(*irr_l) & trans_bits_l;
			*iqr_h |= ~(*irr_h) & trans_bits_h;

		}
		/* Determine which is the highest interrupt in the running and queued registers */
		running_int = m_interrupts_priority_scan(*irr_l, *irr_h);
		high_queue = m_interrupts_priority_scan(*iqr_l, *iqr_h);

		/*
		 * if the running interrupt has a lower priority than a queued interrupt, schedule the
		 * queued interrupt
		 */
		if (running_int > high_queue) {
			/*
			 * Back up the state of the current pipeline
			 */
			ivbt[running_int].pc.ivbt_queue[0] = m->pc.queue[0];
			ivbt[running_int].pc.ivbt_queue[1] = m->pc.queue[1];
			ivbt[running_int].pc.ivbt_queue[2] = m->pc.queue[2];

			ivbt[running_int].pc.size = m->pc.size;

			memcpy(&(ivbt[running_int].register_hazards),
					&(m->pipeline.register_hazards),
					sizeof(m->pipeline.register_hazards));
			memcpy(&(ivbt[running_int].memory_hazards),
					&(m->pipeline.memory_hazards),
					sizeof(m->pipeline.memory_hazards));
			memcpy(&(ivbt[running_int].decode), &(m->pipeline.decode),
					sizeof(m->pipeline.decode));
			memcpy(&(ivbt[running_int].execute), &(m->pipeline.execute),
					sizeof(m->pipeline.execute));
			memcpy(&(ivbt[running_int].writeback), &(m->pipeline.writeback),
					sizeof(m->pipeline.writeback));

			/*
			 * Clear out the pipeline by flushing it
			 */
			m_pipeline_flush(m);

			/*
			 * Restore the pipeline based on what is in IVBT[high_queue], this will kick off running the new code, it is ok if everything is UNIT, you kicked off the pipeline into
			 * a initial state of all NOPs
			 */
			m->pc.queue[0] = ivbt[high_queue].pc.ivbt_queue[0];
			m->pc.queue[1] = ivbt[high_queue].pc.ivbt_queue[1];
			m->pc.queue[2] = ivbt[high_queue].pc.ivbt_queue[2];

			m->pc.size = ivbt[high_queue].pc.size;

			memcpy(&(m->pipeline.register_hazards),
					&(ivbt[high_queue].register_hazards),
					sizeof(m->pipeline.register_hazards));
			memcpy(&(m->pipeline.memory_hazards),
					&(ivbt[high_queue].memory_hazards),
					sizeof(m->pipeline.memory_hazards));
			memcpy(&(m->pipeline.decode), &(ivbt[high_queue].decode),
					sizeof(m->pipeline.decode));
			memcpy(&(m->pipeline.execute), &(ivbt[high_queue].execute),
					sizeof(m->pipeline.execute));
			memcpy(&(m->pipeline.writeback), &(ivbt[high_queue].writeback),
					sizeof(m->pipeline.writeback));

			/*
			 * Queue the currently running interrupt
			 */
			*iqr_l |= *irr_l;
			*iqr_h |= *irr_h;

			/*need to split up the bit shifting by m_word*/
			if (high_queue < (sizeof(m_uword) * 8)) {
				*irr_l = (1 << high_queue);
				*irr_h = 0;
			} else {
				*irr_h = (1 << (high_queue - (sizeof(m_uword) * 8)));
				*irr_l = 0;
			}

			/*turn off who is running from the queue*/
			*iqr_l ^= *irr_l;
			*iqr_h ^= *irr_h;
		}
	}
	/*
	 * If reset is tripped, reset the machine
	 */
	if (*ifr_l & (1 << M_INT_RESET)) {
		miniat_reset(m);
	}

	return;
}

/**
 * Find the highest priority interrupt in the low and high registers.
 * @note
 *  Big O Log(n) -- Backed by binary bitwise search
 *
 * @param in_register_l
 *  The low register to check (IQR_L)
 * @param in_register_h
 *  The high register to check (IQR_H)
 * @return
 *  The highest priority interrupt
 */
static int m_interrupts_priority_scan(m_uword in_register_l,
		m_uword in_register_h) {

	int n;

	if (in_register_l) {
		n = m_util_priority_scan_lowest(in_register_l);
	} else if (in_register_h) {
		n = 32 + m_util_priority_scan_lowest(in_register_h);
	} else {
		n = 64;
	}

	return n;
}

/**
 * Loads the miniat's IVBT with the interrupts vector address located in the IVT for all interrupts
 * in trans_bits.
 * @param m
 * 	The miniat whose IVBT needs to be loades
 * @param trans_bitsionts_l
 *  The low transition bits
 * @param trans_bits_h
 *  The high transition bits
 */
static void m_interrupts_load_ivbt(miniat *m, m_uword trans_bits_l,
		m_uword trans_bits_h) {

	int i;
	m_uword trans;
	m_uword *ifr_low;
	m_uword vector_address = 0;

	ifr_low = m->interrupts.ifr_l;

	trans = trans_bits_l;
	while ((i = m_util_priority_scan_highest(trans)) >= 0) {
		vector_address = m->mem.data[i + M_IVT_START];

		if (!vector_address) {
			if (i == M_INT_ILLEGAL_ADDRESS) {
				*ifr_low |= 1 << M_INT_RESET;
			} else {
				*ifr_low |= 1 << M_INT_ILLEGAL_ADDRESS;
			}
		}

		m->interrupts.ivbt[i].pc.ivbt_queue[0] = vector_address;
		m->interrupts.ivbt[i].pc.ivbt_queue[1] = 0;
		m->interrupts.ivbt[i].pc.ivbt_queue[2] = 0;
		m->interrupts.ivbt[i].pc.size = 1;

		memset(&(m->interrupts.ivbt[i].decode), 0,
				sizeof(m->interrupts.ivbt[i].decode));
		memset(&(m->interrupts.ivbt[i].execute), 0,
				sizeof(m->interrupts.ivbt[i].execute));
		memset(&(m->interrupts.ivbt[i].writeback), 0,
				sizeof(m->interrupts.ivbt[i].writeback));

		trans ^= (1 << i);
	}

	trans = trans_bits_h;
	while ((i = m_util_priority_scan_highest(trans)) >= 0) {
		i += 32;
		vector_address = m->mem.data[i + M_IVT_START];

		if (!vector_address) {
			*ifr_low |= 1 << M_INT_ILLEGAL_ADDRESS;
		}

		m->interrupts.ivbt[i].pc.ivbt_queue[0] = vector_address;
		m->interrupts.ivbt[i].pc.ivbt_queue[1] = 0;
		m->interrupts.ivbt[i].pc.ivbt_queue[2] = 0;
		m->interrupts.ivbt[i].pc.size = 1;

		memset(&(m->interrupts.ivbt[i].decode), 0,
				sizeof(m->interrupts.ivbt[i].decode));
		memset(&(m->interrupts.ivbt[i].execute), 0,
				sizeof(m->interrupts.ivbt[i].execute));
		memset(&(m->interrupts.ivbt[i].writeback), 0,
				sizeof(m->interrupts.ivbt[i].writeback));

		trans ^= (1 << (i - 32));
	}

	return;
}
