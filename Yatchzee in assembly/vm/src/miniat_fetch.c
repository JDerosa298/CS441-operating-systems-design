#include "miniat_fetch.h"

#include "miniat/miniat.h"
#include "miniat_defines.h"
#include "miniat_enums.h"
#include "miniat_interrupts.h"
#include "miniat_memory.h"
#include "miniat_pc.h"
#include "miniat_pipeline.h"
#include "miniat_registers.h"
#include "miniat_structures.h"
#include "miniat_typedefs.h"
#include "miniat_unions.h"

/*
 * This function implements the fetch stage of the MiniAT
 * Interrupts: Illegal Instruction
 */
void m_fetch_stage(miniat *m) {

	m_uword pc;
	m_uword NPC;
	m_memory_characteristics valid;
	m_uword data[M_INSTRUCTION_SIZE_WORDS];
	m_uword int_tripped = 0;

	bool read_pc = false;

	/*
	 * peek the pc queue, we only remove the item when
	 * a valid pc and pc +1 read occurs.
	 */
	pc = m_pc_peek(m);
	NPC = pc + M_INSTRUCTION_SIZE_WORDS;

	/* Everything is ok, go ahead and try and fetch the instruction */
	if (m->pipeline.fetch.state == m_pipeline_stage_state_no_hazard) {

		/*Check for a bad address*/
		valid = m_memory_address_lookup(pc);

		bool valid_address = valid.type == m_memory_type_flash
				|| valid.type == m_memory_type_ram;
		if (!valid_address) {
			int_tripped = M_INT_ILLEGAL_ADDRESS;
			goto out;
		}

		/* Since were reading both address, take the longest stall */
		m_uword a = m_memory_read_or_get_stall_time(m, pc, &data[M_LOW], true);
		m_uword b = m_memory_read_or_get_stall_time(m, pc + 1, &data[M_HIGH],
				true);

		m_uword stall = a < b ? b : a;
		if (stall != M_NO_STALL_DURATION) {
			/* There was a stall, set the stage to stall */
			m_pipeline_stalls_set(m, m_pipeline_stage_fetch,
					m_pipeline_stage_state_structural_memory_hazard, stall);
		} else {
			read_pc = true;
		}
	}
	/* We are stalled waiting on reads from memory ie cache miss */
	else if (m->pipeline.fetch.state
			== m_pipeline_stage_state_structural_memory_hazard) {

		m->pipeline.fetch.duration--;
		if (m->pipeline.fetch.duration == M_NO_STALL_DURATION) {
			/*
			 * If the PC queue is empty, no deeper stages are trying to branch so its ok to add the NPC to the PC queue. Else, someone else is
			 * branching and my NPC doesn't matter because it is past the delay slot (your the tail instruction in the delay slot)
			 */
			m_memory_read(m, pc, &data[M_LOW]);
			m_memory_read(m, pc + 1, &data[M_HIGH]);
			m_pipeline_stalls_clear(m, m_pipeline_stage_fetch);
			read_pc = true;
		}
	}

	/*
	 * If we successfully read the pc, remove it from the queue and update
	 */
	if (read_pc) {
		m_pc_remove_first(m);
		if (m->pc.size == 0) {
			m_pc_add_last(m, NPC);
		}
	}

	out:
	/*
	 * Fetch the low and high words from memory and pass it through to the output latches, including NPC
	 */
	if (m->pipeline.fetch.state == m_pipeline_stage_state_no_hazard
			&& !int_tripped) {
		m->pipeline.fetch.out.data[M_LOW] = data[M_LOW];
		m->pipeline.fetch.out.data[M_HIGH] = data[M_HIGH];
		m->pipeline.fetch.out.NPC = NPC;
		m->pipeline.fetch.out.PC = pc;
	} else if (int_tripped) {
		m_pipeline_morph_to_INT(m, m_pipeline_stage_fetch, int_tripped);
	} else {
		m_pipeline_set_latch_to_NOP(m, m_pipeline_stage_fetch);
	}
	return;
}
