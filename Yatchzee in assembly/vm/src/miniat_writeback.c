#include <errno.h>
#include <stdbool.h>
#include <string.h>


#include "miniat/miniat.h"

#include "miniat/miniat_error.h"
#include "miniat_bus.h"
#include "miniat_defines.h"
#include "miniat_enums.h"
#include "miniat_file.h"
#include "miniat_hazards.h"
#include "miniat_interrupts.h"
#include "miniat_memory.h"
#include "miniat_pc.h"
#include "miniat_pipeline.h"
#include "miniat_registers.h"
#include "miniat_structures.h"
#include "miniat_timers.h"
#include "miniat_typedefs.h"
#include "miniat_unions.h"

/*
 * Throws interrupts:
 *   M_IVT_BUS_ERROR
 */
void m_writeback_stage(miniat *m) {

	m_pipeline_writeback_latch_in *in = &(m->pipeline.writeback.in);
	m_memory_characteristics valid;
	m_uword stall_duration = 0;
	m_uword int_tripped = 0;
	m_uword ifr_tmp = 0;
	m_uword opcode = in->opcode;
	m_uword rX = in->rX;
	m_uword result = in->result;
	m_ubyte hint = in->hint;
	m_uword D = in->D;
	m_uword NPC = in->NPC;

	bool is_write_complete;

	switch (opcode) {
	/*
	 * Arithmetic Instructions
	 * All arithmetic instructions follow the same logic. They must remove the write register
	 * from the hazard queue, clear any hazards that may currently exist for that stage and
	 * write the result to register A.
	 */
	case M_ADD:
	case M_SUB:
	case M_MULT:
	case M_DIV:
	case M_MOD:
	case M_AND:
		m_hazards_remove(m, rX, true);
		m_pipeline_stalls_clear(m, m_pipeline_stage_writeback);
		m_registers_write(m, rX, result);
		break;

		/*
		 * Memory Instructions
		 * Load does not do anything different then the arithmetic instructions, however we kept it separate
		 * for clarity. The reason for it not doing anything different is because the memory/execute stage takes
		 * care of the memory read, the only thing left is to write it to the register file.
		 */
	case M_LOAD:
		m_hazards_remove(m, rX, true);
		m_pipeline_stalls_clear(m, m_pipeline_stage_writeback);
		m_registers_write(m, rX, result);
		break;

		/*
		 * The Store instruction must be able to set the structural hazard. Storing to memory requires a
		 * multi-cycle time frame, and in order to do this, this stage must wait on the write to
		 * memory to finish and stall previous stages.
		 */
	case M_STORE:
		/*
		 * Now that the stall duration has been calculated, you must set the correct hazard based on
		 * whether it is a memory hazard or a peripheral hazard.
		 */
		if (m->pipeline.writeback.state == m_pipeline_stage_state_no_hazard) {
			valid = m_memory_address_lookup(D);

			/*
			 * Set the stall duration period, the decode stage ensures that the memory is valid,
			 * so no concerns here.
			 */
			if (valid.type != m_memory_type_peripheral) {

				stall_duration = m_memory_write_or_get_stall_time(m, D, result,
						false);

				if (stall_duration == M_NO_STALL_DURATION) {

					/*
					 * If the writeback memory type is a CSR register, and it is for an interrupt enable
					 * register, or the system register and the corresponding flush if modified flag is
					 * set, then the pipeline needs to be flushed.
					 */
					if (valid.type == m_memory_type_csr
							&& ((((D == M_CSR_INT_ENABLE_LOW)
									|| (D == M_CSR_INT_ENABLE_HIGH))
									&& m->csr->fi)
									|| ((D == M_CSR_SYSTEM_REGISTER)
											&& m->csr->fs))) {
						m_pipeline_flush(m);
						m_registers_write(m, M_REGISTER_PC_ONE, NPC);
					}

					/* No stall duration just write the data */
					m_memory_write(m, D, result);
					m_hazards_remove(m, D, false);
				} else {
					m_pipeline_stalls_set(m, m_pipeline_stage_writeback,
							m_pipeline_stage_state_structural_memory_hazard,
							stall_duration);
				}
			} else {
				m_pipeline_stalls_set(m, m_pipeline_stage_writeback,
						m_pipeline_stage_state_structural_peripheral_hazard, 0);
				m_bus_write(m, D, result, &int_tripped);
			}
		}
		/*This else if clause gets entered if the current stage is in a memory hazard and has waited long enough for the transfer to mem to complete*/
		else if (m->pipeline.writeback.state
				== m_pipeline_stage_state_structural_memory_hazard) {

			m->pipeline.writeback.duration--;
			if (m->pipeline.writeback.duration == M_NO_STALL_DURATION) {
				/*
				 * Since the transfer is complete you can go ahead and remove the hazard from hazard queue (data hazards) and clear the current stage of it's
				 * structural hazard and finish by writing the desired result to memory.
				 */
				m_hazards_remove(m, D, false);
				m_pipeline_stalls_clear(m, m_pipeline_stage_writeback);
				m_memory_write(m, D, result);
			}
		} else if (m->pipeline.writeback.state
				== m_pipeline_stage_state_structural_peripheral_hazard) {
			/*
			 * the read result gets stored in the out->result latch for execute, the data is valid when the bus completes
			 * it's request. This function will halt the pipeline and push NOPs until the request completes.
			 */
			is_write_complete = m_bus_write(m, D, result, &int_tripped);
			if (is_write_complete) {
				m_hazards_remove(m, D, false);
				m_pipeline_stalls_clear(m, m_pipeline_stage_writeback);
			}
		}
		break;
		/*
		 * RLOAD and RSTORE do exactly the same thing in the writeback stage, as weird as this seems,
		 * it is true. This is because a MUX in the execute stage performs a "switch" on the latches
		 * so the values go into the correct latch piece based on the action (rstore or rload).
		 */
	case M_RLOAD:
	case M_RSTORE:
		m_hazards_remove(m, rX, true);
		m_pipeline_stalls_clear(m, m_pipeline_stage_writeback);
		m_registers_write(m, rX, result);
		break;
		/*
		 * Logic instructions need to remove their writeback register from the data hazard queue,
		 * clear any hazards that may be marked for the stage, and write the result to the register
		 * file.
		 */
	case M_OR:
	case M_EXOR:
	case M_SHL:
	case M_SHR:
		m_hazards_remove(m, rX, true);
		m_pipeline_stalls_clear(m, m_pipeline_stage_writeback);
		m_registers_write(m, rX, result);
		break;
		/*
		 * Branches in the writeback all do the same thing. They either take the branch because
		 * they were hinted false (no flush delay slot) or they inappropriately took a branch
		 * early and need to flush the pipeline. Flushing the pipeline involves setting all the
		 * latches to NOPs, removing all hazards from the data hazard queue, flushing the PC queue,
		 * removing all the stalls and setting the new PC to the NPC of the non-taken branch.
		 */
	case M_BRAE:
	case M_BRANE:
	case M_BRAL:
	case M_BRALE:
	case M_BRAG:
	case M_BRAGE:
		/*
		 * Result Hint Action
		 *   0     0 - Do Nothing
		 *   0     1 - Flush pipeline and set PC to NPC
		 *   1     0 - Take Branch
		 *   1     1 - Do Nothing
		 */
		if (!result && hint) {
			m_pipeline_flush(m);
			m_pc_add_last(m, NPC);

		} else if (result && !hint) {
			m_pc_add_last(m, D);
		}
		break;
	case M_INT:
		/*
		 * Write by bitwise "oring" the global version of the IFR with the per stage version
		 * carried through the pipeline. This ensures that we do not inadvertently clear other
		 * interrupts.
		 */
		m_memory_read(m, M_CSR_INT_FLAG_LOW, &ifr_tmp);
		ifr_tmp |= 1 << D;
		m_memory_write(m, M_CSR_INT_FLAG_LOW, ifr_tmp);
		m_hazards_remove(m, M_CSR_INT_FLAG_LOW, false);
		break;
	case M_IRET:
		/*
		 * The IRET (interrupt return) function clears the currently
		 * running interrupt register, turns down its flag in ifr
		 * (interrupt flag register), flush's the pipeline and
		 * restores execution to normal context code.
		 *
		 * If another interrupt was to execute, the interrupt
		 * routine catches this.
		 */
		*(m->interrupts.ifr_l) &= ~(m->interrupts.irr_l);
		*(m->interrupts.ifr_h) &= ~(m->interrupts.irr_h);
		m->interrupts.irr_l = 0;
		m->interrupts.irr_h = 0;

		m_pipeline_flush(m);
		m->pc.queue[0] = m->interrupts.ivbt[M_IVBT_PC].pc.ivbt_queue[0];
		m->pc.queue[1] = m->interrupts.ivbt[M_IVBT_PC].pc.ivbt_queue[1];
		m->pc.queue[2] = m->interrupts.ivbt[M_IVBT_PC].pc.ivbt_queue[2];

		m->pc.size = m->interrupts.ivbt[M_IVBT_PC].pc.size;

		memcpy(&(m->pipeline.register_hazards),
				&(m->interrupts.ivbt[M_IVBT_PC].register_hazards),
				sizeof(m->pipeline.register_hazards));
		memcpy(&(m->pipeline.memory_hazards),
				&(m->interrupts.ivbt[M_IVBT_PC].memory_hazards),
				sizeof(m->pipeline.memory_hazards));
		memcpy(&(m->pipeline.decode.in),
				&(m->interrupts.ivbt[M_IVBT_PC].decode),
				sizeof(m->pipeline.decode.in));
		memcpy(&(m->pipeline.execute.in),
				&(m->interrupts.ivbt[M_IVBT_PC].execute),
				sizeof(m->pipeline.execute.in));
		memcpy(&(m->pipeline.writeback.in),
				&(m->interrupts.ivbt[M_IVBT_PC].writeback),
				sizeof(m->pipeline.writeback.in));

		m->pipeline.is_interrupt_return = true;

		break;
	default:
		THROW(EINVAL, "Invalid opcode got: 0x%X\n", opcode);
		/* no break */
	}

	if (int_tripped) {
		m_pipeline_morph_to_INT(m, m_pipeline_stage_writeback, int_tripped);
		m_hazards_remove(m, D, true);
	}
	return;
}

