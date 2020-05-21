#include <errno.h>
#include <string.h>
#include <stdbool.h>

#include "miniat/miniat.h"
#include "miniat/miniat_error.h"
#include "miniat_bus.h"
#include "miniat_defines.h"
#include "miniat_enums.h"
#include "miniat_hazards.h"
#include "miniat_interrupts.h"
#include "miniat_memory.h"
#include "miniat_pipeline.h"
#include "miniat_registers.h"
#include "miniat_structures.h"
#include "miniat_typedefs.h"
#include "miniat_unions.h"

/*
 * Interrupts: M_IVT_DIVIDE_BY_ZERO, M_IVT_BUS_ERROR
 */
void m_execute_stage(miniat *m) {

	m_pipeline_execute_latch_in *in = &(m->pipeline.execute.in);
	m_pipeline_execute_latch_out *out = &(m->pipeline.execute.out);
	m_memory_characteristics valid;
	m_uword stall_duration = 0;

	/* this must be at the top because of write to rX */
	/* pass the NPC and rX through the stage */
	m_uword rX = in->rX;
	m_uword opcode = in->opcode;
	m_word A;
	A.u = in->A;
	m_word D;
	D.u = in->D;
	m_word B;
	B.u = in->B;
	m_word result;
	result.u = 0;
	m_uword int_tripped = 0;

	m_dword overflow_B;
	m_dword overflow_D;
	m_dword overflow_result;

	bool is_read_complete;

	switch (opcode) {
	/*Arithmetic*/
	case M_ADD:

		/*
		 * Perform the addition in the appropriate mode
		 */
		if (m->csr->u) {
			overflow_B.u = B.u;
			overflow_D.u = D.u;

			result.u = B.u + D.u;
			overflow_result.u = overflow_B.u + overflow_D.u;

			if (m->csr->se) {
				if (result.u < overflow_result.u) {
					result.u = ~0;
				} else if (result.u > overflow_result.u) {
					result.u = 0;
				}
			}
		} else {
			overflow_B.s = B.s;
			overflow_D.s = D.s;

			result.s = B.s + D.s;
			overflow_result.s = overflow_B.s + overflow_D.s;

			if (m->csr->se) {
				if (result.s < overflow_result.s) {
					result.s = ~(1 << 31);
				} else if (result.s > overflow_result.s) {
					result.s = 1 << 31;
				}
			}
		}
		break;
	case M_SUB:
		if (m->csr->u) {
			overflow_B.u = B.u;
			overflow_D.u = D.u;

			result.u = B.u - D.u;
			overflow_result.u = overflow_B.u - overflow_D.u;

			if (m->csr->se) {
				if (result.u < overflow_result.u) {
					result.u = ~0;
				} else if (result.u > overflow_result.u) {
					result.u = 0;
				}
			}
		} else {
			overflow_B.s = B.s;
			overflow_D.s = D.s;

			result.s = B.s - D.s;
			overflow_result.s = overflow_B.s - overflow_D.s;

			if (m->csr->se) {
				if (result.s < overflow_result.s) {
					result.s = ~(1 << 31);
				} else if (result.s > overflow_result.s) {
					result.s = 1 << 31;
				}
			}
		}
		break;
	case M_MULT:
		if (m->csr->u) {
			overflow_B.u = B.u;
			overflow_D.u = D.u;

			result.u = B.u * D.u;
			overflow_result.u = overflow_B.u * overflow_D.u;

			if (m->csr->se) {
				if (result.u < overflow_result.u) {
					result.u = ~0;
				} else if (result.u > overflow_result.u) {
					result.u = 0;
				}
			}
		} else {
			overflow_B.s = B.s;
			overflow_D.s = D.s;

			result.s = B.s * D.s;
			overflow_result.s = overflow_B.s * overflow_D.s;

			if (m->csr->se) {
				if (result.s < overflow_result.s) {
					result.s = ~(1 << 31);
				} else if (result.s > overflow_result.s) {
					result.s = 1 << 31;
				}
			}
		}
		break;
	case M_DIV:
		if (D.u) {
			if (m->csr->u) {
				overflow_B.u = B.u;
				overflow_D.u = D.u;

				result.u = B.u / D.u;
				overflow_result.u = overflow_B.u / overflow_D.u;

				if (m->csr->se) {
					if (result.u < overflow_result.u) {
						result.u = ~0;
					} else if (result.u > overflow_result.u) {
						result.u = 0;
					}
				}
			} else {
				overflow_B.s = B.s;
				overflow_D.s = D.s;

				result.s = B.s / D.s;
				overflow_result.s = overflow_B.s / overflow_D.s;

				if (m->csr->se) {
					if (result.s < overflow_result.s) {
						result.s = ~(1 << 31);
					} else if (result.s > overflow_result.s) {
						result.s = 1 << 31;
					}
				}
			}
		} else {
			int_tripped = M_INT_DIVIDE_BY_ZERO;
		}
		break;
	case M_MOD:
		if (D.u) {
			if (m->csr->u) {
				overflow_B.u = B.u;
				overflow_D.u = D.u;

				result.u = B.u % D.u;
				overflow_result.u = overflow_B.u % overflow_D.u;

				if (m->csr->se) {
					if (result.u < overflow_result.u) {
						result.u = ~0;
					} else if (result.u > overflow_result.u) {
						result.u = 0;
					}
				}
			} else {
				overflow_B.s = B.s;
				overflow_D.s = D.s;

				result.s = B.s % D.s;
				overflow_result.s = overflow_B.s % overflow_D.s;

				if (m->csr->se) {
					if (result.s < overflow_result.s) {
						result.s = ~(1 << 31);
					} else if (result.s > overflow_result.s) {
						result.s = 1 << 31;
					}
				}
			}
		} else {
			int_tripped = M_INT_DIVIDE_BY_ZERO;
		}
		break;
	case M_LOAD:
		valid = m_memory_address_lookup(D.u);
		/*
		 * Since the RAM and FLASH time duration structural hazards have yet set a hazard,
		 * they just merely determined the stall duration, then if the pipeline has no
		 * hazard(which it should) set the hazard for a memory hazard and set the stall time.
		 * Calculating stall time is the current tick + how long you want to wait for. So if
		 * you are on tick 5 and want to wait for 4 ticks, then you must set the stall duration
		 * to 9.
		 */
		if (m->pipeline.execute.state == m_pipeline_stage_state_no_hazard) {
			/*
			 * If you are going to RAM or FLASH, the stall period should be set for the correct duration
			 */
			/*Set the stall duration and stall for memory*/
			if (valid.type != m_memory_type_peripheral) {

				stall_duration = m_memory_read_or_get_stall_time(m, D.u,
						&result.u, false);
				if (stall_duration != M_NO_STALL_DURATION) {
					m_pipeline_stalls_set(m, m_pipeline_stage_execute,
							m_pipeline_stage_state_structural_memory_hazard,
							stall_duration);
				}
			}
			/* For peripheral hazards, there is no time limit. Thus you must set the state to
			 * request, This will initialize the "bus state machine" You must also tell the
			 * pipeline that you are setting up a structural peripheral hazard.
			 */
			else {
				m_pipeline_stalls_set(m, m_pipeline_stage_execute,
						m_pipeline_stage_state_structural_peripheral_hazard, 0);
				m_bus_read(m, D.u, &(result.u), &int_tripped);
			}
		}
		/*
		 * If you are in a structural stall for a timed event, such as a memory hazard, and your
		 * time has come where you can complete the action, then you must clear the stages
		 * hazard and perform the READ from memory. Since this is a READ from memory, the
		 * address the read occurred from was never marked in the data hazard queue. Writes are
		 * the only things marked "dirty" in the hazard queue. Also, since this occurs late in
		 * the cycle, no other stages can run, so we must return true, indicating that we have
		 * exhausted the clock cycle.
		 */
		else if (m->pipeline.execute.state
				== m_pipeline_stage_state_structural_memory_hazard) {

			m->pipeline.execute.duration--;
			if (m->pipeline.execute.duration == M_NO_STALL_DURATION) {
				m_pipeline_stalls_clear(m, m_pipeline_stage_execute);
				m_memory_read(m, D.u, &result.u);
			}
		}
		/*
		 * So if your a structural hazard for a peripheral, you must enter the "peripheral state machine"
		 */
		else if (m->pipeline.execute.state
				== m_pipeline_stage_state_structural_peripheral_hazard) {
			/*
			 * the read result gets stored in the result latch for execute, the data is valid when the bus completes
			 * it's request. This function will halt the pipeline and push NOPs until the request completes.
			 */
			is_read_complete = m_bus_read(m, D.u, &(result.u), &int_tripped);
			if (is_read_complete) {
				m_pipeline_stalls_clear(m, m_pipeline_stage_execute);
			}
		}
		break;
	case M_STORE:
		result.u = A.u;
		break;
	case M_RLOAD:
		m_registers_read(m, D.u, &result.u);
		break;
	case M_RSTORE:
		result.u = A.u;
		rX = D.u;
		break;
	case M_AND:
		result.s = B.s & D.s;
		break;
	case M_OR:
		result.u = B.u | D.u;
		break;
	case M_EXOR:
		result.u = B.u ^ D.u;
		break;
	case M_SHL:
		result.u = B.u << D.u;
		break;
	case M_SHR:
		result.u = B.u >> D.u;
		break;
	case M_BRAE:
		result.u = A.u == B.u;
		break;
	case M_BRANE:
		result.u = A.u != B.u;
		break;
	case M_BRAL:
		if (m->csr->u) {
			result.u = A.u < B.u;
		} else {
			result.s = A.s < B.s;
		}
		break;
	case M_BRALE:
		if (m->csr->u) {
			result.u = A.u <= B.u;
		} else {
			result.s = A.s <= B.s;
		}
		break;
	case M_BRAG:
		if (m->csr->u) {
			result.u = A.u > B.u;
		} else {
			result.s = A.s > B.s;
		}
		break;
	case M_BRAGE:
		if (m->csr->u) {
			result.u = A.u >= B.u;
		} else {
			result.s = A.s >= B.s;
		}
		break;
	case M_INT:
		break;
	case M_IRET:
		break;
	default:
		THROW(EINVAL, "Invalid opcode, got: 0x%X", opcode);
		/* no break */
	}

	if (m->pipeline.execute.state == m_pipeline_stage_state_no_hazard
			&& !int_tripped) {
		out->D = D.u;
		out->hint = in->hint;
		out->opcode = opcode;
		out->rX = rX;
		out->result = result.u;
		out->NPC = in->NPC;
		out->PC = in->PC;
	} else if (int_tripped) {
		m_hazards_remove(m, rX, true);
		m_pipeline_morph_to_INT(m, m_pipeline_stage_execute, int_tripped);
	} else {
		m_pipeline_set_latch_to_NOP(m, m_pipeline_stage_execute);
	}

	return;
}
