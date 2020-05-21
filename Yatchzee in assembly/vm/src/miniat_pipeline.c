#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "miniat/miniat.h"
#include "miniat/miniat_error.h"
#include "miniat_decode.h"
#include "miniat_defines.h"
#include "miniat_enums.h"
#include "miniat_execute.h"
#include "miniat_fetch.h"
#include "miniat_hazards.h"
#include "miniat_memory.h"
#include "miniat_pc.h"
#include "miniat_structures.h"
#include "miniat_pipeline.h"
#include "miniat_typedefs.h"
#include "miniat_unions.h"
#include "miniat_writeback.h"

void m_pipeline_stalls_init(miniat *m);

void m_pipeline_init(miniat *m) {

	m_hazards_init(m);
	m_pipeline_stalls_init(m);
	m->pipeline.is_interrupt_return = false;
}

void m_pipeline_clock(miniat *m) {

	m_writeback_stage(m);

	/*
	 * When the writeback stage was an IRET instruction, the entire pipeline is
	 * restored to the normal context.  At this point the WB stage contains a
	 * restored instruction that was interrupted before it completed.  None of
	 * the pipleline stages should advance in this case, or the WB instruction
	 * will be lost and never complete.  This is effectively a stall at the WB
	 * stage, but is not implemented as such since the restored WB may already
	 * have been stalled before it was interrupted, and we must ensure this is
	 * not lost for correctness.
	 */
	if (m->pipeline.is_interrupt_return) {
		m->pipeline.is_interrupt_return = false;
		return;
	}

	/*
	 * Once the Write Back stage completes in the "first half" of the cycle,
	 * each additional stage can move its latches forward only if the
	 * stage ahead of it is not in hazard and the stage, itself, is not
	 * stalled.
	 */
	if (m->pipeline.execute.state != m_pipeline_stage_state_stalled
			&& m->pipeline.writeback.state
					== m_pipeline_stage_state_no_hazard) {
		m_execute_stage(m);
	}
	if (m->pipeline.decode.state != m_pipeline_stage_state_stalled
			&& m->pipeline.execute.state == m_pipeline_stage_state_no_hazard) {
		m_decode_stage(m);
	}
	if (m->pipeline.fetch.state != m_pipeline_stage_state_stalled
			&& m->pipeline.decode.state == m_pipeline_stage_state_no_hazard) {
		m_fetch_stage(m);
	}

	/*
	 * shuffle_latches
	 */
	if (m->pipeline.writeback.state == m_pipeline_stage_state_no_hazard) {
		m->pipeline.writeback.in.opcode = m->pipeline.execute.out.opcode;
		m->pipeline.writeback.in.hint = m->pipeline.execute.out.hint;
		m->pipeline.writeback.in.D = m->pipeline.execute.out.D;
		m->pipeline.writeback.in.NPC = m->pipeline.execute.out.NPC;
		m->pipeline.writeback.in.rX = m->pipeline.execute.out.rX;
		m->pipeline.writeback.in.result = m->pipeline.execute.out.result;
		m->pipeline.writeback.in.PC = m->pipeline.execute.out.PC;
	}

	if (m->pipeline.execute.state == m_pipeline_stage_state_no_hazard) {
		m->pipeline.execute.in.A = m->pipeline.decode.out.A;
		m->pipeline.execute.in.B = m->pipeline.decode.out.B;
		m->pipeline.execute.in.D = m->pipeline.decode.out.D;
		m->pipeline.execute.in.NPC = m->pipeline.decode.out.NPC;
		m->pipeline.execute.in.opcode = m->pipeline.decode.out.opcode;
		m->pipeline.execute.in.hint = m->pipeline.decode.out.hint;
		m->pipeline.execute.in.rX = m->pipeline.decode.out.rX;
		m->pipeline.execute.in.PC = m->pipeline.decode.out.PC;
	}

	if (m->pipeline.decode.state == m_pipeline_stage_state_no_hazard) {
		m->pipeline.decode.in.NPC = m->pipeline.fetch.out.NPC;
		m->pipeline.decode.in.data[M_LOW] = m->pipeline.fetch.out.data[M_LOW];
		m->pipeline.decode.in.data[M_HIGH] = m->pipeline.fetch.out.data[M_HIGH];
		m->pipeline.decode.in.PC = m->pipeline.fetch.out.PC;
	}
}

void m_pipeline_stalls_init(miniat *m) {

	m_pipeline_stalls_clear_all(m);
}

void m_pipeline_stalls_clear_all(miniat *m) {

	m->pipeline.fetch.state =  m_pipeline_stage_state_no_hazard;
	m->pipeline.decode.state = m_pipeline_stage_state_no_hazard;
	m->pipeline.execute.state =  m_pipeline_stage_state_no_hazard;
	m->pipeline.writeback.state = m_pipeline_stage_state_no_hazard;
}

void m_pipeline_stalls_set(miniat *m, m_pipeline_stage stage,
		m_pipeline_stage_state state, m_uword duration) {

	switch (stage) {
	case m_pipeline_stage_writeback:
		if (stage == m_pipeline_stage_writeback) {
			m->pipeline.writeback.state = state;
			m->pipeline.writeback.duration = duration;
		} else {
			if (m->pipeline.writeback.state
					== m_pipeline_stage_state_no_hazard) {
				m->pipeline.writeback.state = m_pipeline_stage_state_stalled;
			} else {
				break;
			}
		}
		/* no break */
	case m_pipeline_stage_execute:
		if (stage == m_pipeline_stage_execute) {
			m->pipeline.execute.state = state;
			m->pipeline.execute.duration = duration;
		} else {
			if (m->pipeline.execute.state == m_pipeline_stage_state_no_hazard) {
				m->pipeline.execute.state = m_pipeline_stage_state_stalled;
			} else {
				break;
			}
		}
		/* no break */
	case m_pipeline_stage_decode:
		if (stage == m_pipeline_stage_decode) {
			m->pipeline.decode.state = state;
			m->pipeline.decode.duration = duration;
		} else {
			if (m->pipeline.decode.state == m_pipeline_stage_state_no_hazard) {
				m->pipeline.decode.state = m_pipeline_stage_state_stalled;
			} else {
				break;
			}
		}
		/* no break */
	case m_pipeline_stage_fetch:
		if (stage == m_pipeline_stage_fetch) {
			m->pipeline.fetch.state = state;
			m->pipeline.fetch.duration = duration;
		} else {
			if (m->pipeline.fetch.state == m_pipeline_stage_state_no_hazard) {
				m->pipeline.fetch.state = m_pipeline_stage_state_stalled;
			} else {
				break;
			}
		}
		break;
	default:
		THROW(EINVAL, "Invalid pipeline stage: %d\n", stage);
		/* no break */
	}
}

void m_pipeline_stalls_clear(miniat *m, m_pipeline_stage stage) {

	switch (stage) {
	case m_pipeline_stage_writeback:
		if (stage == m_pipeline_stage_writeback) {
			m->pipeline.writeback.state = m_pipeline_stage_state_no_hazard;
			m->pipeline.writeback.duration = M_NO_STALL_DURATION;
		} else {
			if (m->pipeline.writeback.state == m_pipeline_stage_state_stalled) {
				m->pipeline.writeback.state = m_pipeline_stage_state_no_hazard;
			} else {
				break;
			}
		}
		/* no break */
	case m_pipeline_stage_execute:
		if (stage == m_pipeline_stage_execute) {
			m->pipeline.execute.state = m_pipeline_stage_state_no_hazard;
			m->pipeline.execute.duration = M_NO_STALL_DURATION;
		} else {
			if (m->pipeline.execute.state == m_pipeline_stage_state_stalled) {
				m->pipeline.execute.state = m_pipeline_stage_state_no_hazard;
			} else {
				break;
			}
		}
		/* no break */
	case m_pipeline_stage_decode:
		if (stage == m_pipeline_stage_decode) {
			m->pipeline.decode.state = m_pipeline_stage_state_no_hazard;
			m->pipeline.decode.duration = M_NO_STALL_DURATION;
		} else {
			if (m->pipeline.decode.state == m_pipeline_stage_state_stalled) {
				m->pipeline.decode.state = m_pipeline_stage_state_no_hazard;
			} else {
				break;
			}
		}
		/* no break */
	case m_pipeline_stage_fetch:
		if (stage == m_pipeline_stage_fetch) {
			m->pipeline.fetch.state = m_pipeline_stage_state_no_hazard;
			m->pipeline.fetch.duration = M_NO_STALL_DURATION;
		} else {
			if (m->pipeline.fetch.state == m_pipeline_stage_state_stalled) {
				m->pipeline.fetch.state = m_pipeline_stage_state_no_hazard;
			} else {
				break;
			}
		}
		break;
	default:
		THROW(EINVAL, "Invalid pipeline stage: %d\n", stage);
		/* no break */
	}
}

/**
 * Resets a given miniat pipeline state to 0's
 *
 * @param m - the miniat whose pipeline state to clear
 */
static void m_pipeline_clear(miniat *m) {

	memset(&m->pipeline.fetch, 0, sizeof(m_pipeline_fetch));
	memset(&m->pipeline.decode, 0, sizeof(m_pipeline_decode));
	memset(&m->pipeline.execute, 0, sizeof(m_pipeline_execute));
	memset(&m->pipeline.writeback, 0, sizeof(m_pipeline_writeback));
}

void m_pipeline_set_latch_to_NOP(miniat *m, m_pipeline_stage stage) {

	switch (stage) {
	case m_pipeline_stage_fetch:
		memset(&(m->pipeline.fetch.out), 0, sizeof(m_pipeline_fetch_latch_out));
		break;
	case m_pipeline_stage_decode:
		memset(&(m->pipeline.decode.out), 0,
				sizeof(m_pipeline_decode_latch_out));
		break;
	case m_pipeline_stage_execute:
		memset(&(m->pipeline.execute.out), 0,
				sizeof(m_pipeline_execute_latch_out));
		break;
	default:
		THROW(EINVAL, "Invalid pipeline stage: %d\n", stage);
		/* no break */
	}
}

void m_pipeline_flush(miniat *m) {

	m_pc_clear(m);
	m_pipeline_stalls_clear_all(m);
	m_hazards_clear(m);
	m_pipeline_clear(m);
}

void m_pipeline_morph_to_INT(miniat *m, m_pipeline_stage stage,
		m_uword interrupt) {

	m_uword tmp = 0;
	m_pipeline_fetch_latch_out *fetch = &(m->pipeline.fetch.out);
	m_pipeline_decode_latch_out *decode = &(m->pipeline.decode.out);
	m_pipeline_execute_latch_out *execute = &(m->pipeline.execute.out);

	m_hazards_add(m, M_CSR_INT_FLAG_LOW, false);

	switch (stage) {
	case m_pipeline_stage_fetch:
		fetch->PC = 0;
		fetch->NPC = 0;
		fetch->data[1] = interrupt;
		((m_low_word_layout *) &fetch->data[0])->opcode = M_INT;
		break;

	case m_pipeline_stage_decode:
		decode->PC = 0;
		decode->NPC = 0;
		decode->opcode = M_INT;
		decode->A = 0;
		decode->B = 0;
		decode->D = interrupt;
		decode->hint = 0;
		decode->rX = 0;
		break;

	case m_pipeline_stage_execute:
		execute->PC = 0;
		execute->NPC = 0;
		execute->D = interrupt;
		execute->hint = 0;
		execute->opcode = M_INT;
		execute->rX = 0;
		break;

	case m_pipeline_stage_writeback:
		m_memory_read(m, M_CSR_INT_FLAG_LOW, &tmp);
		tmp |= (1 << interrupt);
		m_memory_write(m, M_CSR_INT_FLAG_LOW, tmp);
		m_hazards_remove(m, M_CSR_INT_FLAG_LOW, false);
		break;
	default:
		THROW(EINVAL, "Invalid pipeline stage: %d\n", stage);
		/* no break */
	}
	m_pipeline_stalls_clear(m, stage);
}
