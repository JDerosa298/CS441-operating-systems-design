#include "miniat_decode.h"

#include <string.h>

#include "miniat/miniat.h"
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

/*Interrupts: M_IVT_ILLEGAL_INSTRUCTION, M_IVT_ILLEGAL_ADDRESS*/
void m_decode_stage(miniat *m) {

	m_low_word_layout low_word;
	m_uword immediate = 0;
	m_uword D = 0;
	m_uword A = 0;
	m_uword B = 0;
	m_uword opcode = 0;
	m_uword int_tripped = 0;
	m_uword predicate_value = 0;

	m_ubyte predicate_register;
	m_ubyte hint;
	m_ubyte rA;
	m_ubyte rB;
	m_ubyte rC;

	m_pipeline_decode_latch_in *in = &(m->pipeline.decode.in);
	m_pipeline_decode_latch_out *out = &(m->pipeline.decode.out);

	m_memory_characteristics valid;

	/*
	 * The opcode is the low m_word or first m_word in memory that you have grabbed, get the m_word from the latch
	 * and parse it apart to get the relevant data
	 */
	low_word.all = in->data[M_LOW];
	immediate = in->data[M_HIGH];

	opcode = low_word.opcode;
	hint = low_word.hint;
	predicate_register = low_word.predicate;
	rA = low_word.rA;
	rB = low_word.rB;
	rC = low_word.rC;

	/*
	 * Set the output latches to the data you just gathered from parsing the opcode remember that this data could be invalid if a data hazard
	 * is detected below, if that happens the latches are overwritten....We did it this way to avoid having multiple exit points or flags
	 * within this function
	 */
	m_registers_read(m, rC, &D); //m->reg[rC] + immediate;
	D += immediate;
	m_registers_read(m, rB, &B); //m->reg[rB];
	m_registers_read(m, rA, &A); //m->reg[rA];

	/*
	 * All memory writes (both register, RAM and FLASH) are marked dirty in this stage. If a register is marked dirty,
	 * this stage cannot continue if it needs to read from aforementioned register. This a stall on a data hazard must occur.
	 */
	switch (opcode) {
	/*Arithmetic*/
	case M_ADD:
	case M_SUB:
	case M_MULT:
	case M_DIV:
	case M_MOD:
		/*If you check the queue and there is no hazard*/
		if (!(m_hazards_check(m, rB, true) || m_hazards_check(m, rC, true))) {

			m_pipeline_stalls_clear(m, m_pipeline_stage_decode);
			m_hazards_add(m, rA, true);
		}
		/*Else there is a hazard and you must set the hazard and push NOPs*/
		else {
			m_pipeline_stalls_set(m, m_pipeline_stage_decode,
					m_pipeline_stage_state_data_hazard, 0);
		}
		break;
	case M_LOAD:
		/*see if I am in the data hazard queue, and if I am not add me to the queue...*/
		if (!(m_hazards_check(m, rC, true) || m_hazards_check(m, D, false))) {

			valid = m_memory_address_lookup(D);

			/*if i am a valid address and readable*/
			if ((valid.permissions == m_memory_permissions_read_access
					|| valid.permissions == m_memory_permissions_full_access)
					&& ((valid.type == m_memory_type_ram)
							|| (valid.type == m_memory_type_flash)
							|| (valid.type == m_memory_type_id)
							|| (valid.type == m_memory_type_csr)
							|| (valid.type == m_memory_type_gpio)
							|| (valid.type == m_memory_type_ivt)
							|| (valid.type == m_memory_type_peripheral))) {

				m_pipeline_stalls_clear(m, m_pipeline_stage_decode);
				m_hazards_add(m, rA, true);
			} else {
				/* D is out of range, illegal address*/
				m_hazards_add(m, M_CSR_INT_FLAG_LOW, false);
				int_tripped = M_INT_ILLEGAL_ADDRESS;
			}
		} else {
			m_pipeline_stalls_set(m, m_pipeline_stage_decode,
					m_pipeline_stage_state_data_hazard, 0);
		}
		break;
	case M_STORE:
		if (!(m_hazards_check(m, rC, true) || m_hazards_check(m, rA, true))) {
			/*
			 * Check to make sure the memory WRITE address if valid, if it is not valid, then you should
			 * NOP the instruction and trip illegal address
			 */
			valid = m_memory_address_lookup(D);
			if (((valid.permissions == m_memory_permissions_write_access)
					|| (valid.permissions == m_memory_permissions_full_access))
					&& ((valid.type == m_memory_type_ram)
							|| (valid.type == m_memory_type_flash)
							|| (valid.type == m_memory_type_csr)
							|| (valid.type == m_memory_type_gpio)
							|| (valid.type == m_memory_type_ivt)
							|| (valid.type == m_memory_type_peripheral))) {
				m_pipeline_stalls_clear(m, m_pipeline_stage_decode);
				m_hazards_add(m, D, false);
			} else {
				m_hazards_add(m, M_CSR_INT_FLAG_LOW, false);
				int_tripped = M_INT_ILLEGAL_ADDRESS;
			}
		} else {
			m_pipeline_stalls_set(m, m_pipeline_stage_decode,
					m_pipeline_stage_state_data_hazard, 0);

		}
		break;
	case M_RLOAD:
		if (!(m_hazards_check(m, rC, true) || m_hazards_check(m, D, true))) {
			if (D < M_NUM_REGISTERS) {
				m_pipeline_stalls_clear(m, m_pipeline_stage_decode);
				m_hazards_add(m, rA, true);
			} else {

				m_hazards_add(m, M_CSR_INT_FLAG_LOW, false);
				int_tripped = M_INT_ILLEGAL_ADDRESS;
			}
		} else {
			m_pipeline_stalls_set(m, m_pipeline_stage_decode,
					m_pipeline_stage_state_data_hazard, 0);
		}
		break;
	case M_RSTORE:
		if (!(m_hazards_check(m, rC, true) || m_hazards_check(m, rA, true))) {
			if (D < M_NUM_REGISTERS) {
				m_pipeline_stalls_clear(m, m_pipeline_stage_decode);
				m_hazards_add(m, D, true);
			} else {
				m_hazards_add(m, M_CSR_INT_FLAG_LOW, false);
				int_tripped = M_INT_ILLEGAL_ADDRESS;
			}
		} else {
			m_pipeline_stalls_set(m, m_pipeline_stage_decode,
					m_pipeline_stage_state_data_hazard, 0);
		}
		break;
	case M_AND:
	case M_OR:
	case M_EXOR:
	case M_SHL:
	case M_SHR:
		/*if there is no hazard add my write register to the data hazard queue and clear any hazards that may exist*/
		if (!(m_hazards_check(m, rB, true) || m_hazards_check(m, rC, true))) {

			m_pipeline_stalls_clear(m, m_pipeline_stage_decode);
			m_hazards_add(m, rA, true);
		}
		/*if there is a data hazard set the hazard and push NOPs*/
		else {
			m_pipeline_stalls_set(m, m_pipeline_stage_decode,
					m_pipeline_stage_state_data_hazard, 0);
		}
		break;
	case M_BRAE:
	case M_BRANE:
	case M_BRAL:
	case M_BRALE:
	case M_BRAG:
	case M_BRAGE:
		/* if there are no hazards on input registers, clear any hazards that may exist */
		if (!(m_hazards_check(m, rA, true) || m_hazards_check(m, rB, true)
				|| m_hazards_check(m, rC, true))) {
			m_pipeline_stalls_clear(m, m_pipeline_stage_decode);

			valid = m_memory_address_lookup(D);
			if ((valid.type == m_memory_type_flash || valid.type == m_memory_type_ram)
					&& (valid.permissions == m_memory_permissions_read_access
							|| valid.permissions == m_memory_permissions_full_access)) {
				if (hint) {
					m_registers_write(m, M_REGISTER_PC_ONE, D);
				}
			} else {
				m_hazards_add(m, M_CSR_INT_FLAG_LOW, false);
				int_tripped = M_INT_ILLEGAL_ADDRESS;
			}
		} else {
			/*if there is a data hazard set the hazard and push NOPs*/
			m_pipeline_stalls_set(m, m_pipeline_stage_decode,
					m_pipeline_stage_state_data_hazard, 0);
		}
		break;
	case M_INT:
		if (!m_hazards_check(m, rC, true)) {
			if ((D >= M_INT_RESET && D <= M_INT_OVERFLOW)
					|| (D >= M_XINT0 && D <= M_XINT15)) {
				m_pipeline_stalls_clear(m, m_pipeline_stage_decode);
			} else {
				m_hazards_add(m, M_CSR_INT_FLAG_LOW, false);
				int_tripped = M_INT_ILLEGAL_ADDRESS;
			}
		} else {
			/*if there is a data hazard set the hazard and push NOPs*/
			m_pipeline_stalls_set(m, m_pipeline_stage_decode,
					m_pipeline_stage_state_data_hazard, 0);
		}
		break;
	case M_IRET:
		break;
	default:
		m_hazards_add(m, M_CSR_INT_FLAG_LOW, false);
		int_tripped = M_INT_ILLEGAL_INSTRUCTION;
		break;
	}

	/*
	 * If their is a write hazard on the predication register the pipeline must push a NOP
	 * until it clears
	 */
	if (!m_hazards_check(m, predicate_register, true)) {

		/*
		 * If their are no interrupts, the instruction is not predicated and
		 * the pipeline is not in a hazard (ie all is well) then go ahead
		 * and send everything to the output latches.
		 */
		if (!int_tripped && !predicate_value
				&& m->pipeline.decode.state == m_pipeline_stage_state_no_hazard) {

			out->A = A;
			out->B = B;
			out->D = D;
			out->hint = hint;
			out->opcode = opcode;
			out->rX = rA;
			out->NPC = in->NPC;
			out->PC = in->PC;
		}

		/* Else if their are no interrupts and it's predicated (run as NOP) or the pipeline is in a hazard, then push a NOP(S). */
		else if (!int_tripped
				&& (m->pipeline.decode.state != m_pipeline_stage_state_no_hazard
						|| predicate_value)) {
			m_pipeline_set_latch_to_NOP(m, m_pipeline_stage_decode);
		}

		/* Else their was an interrupt and we should morph the instruction into an INT instruction */
		else {
			m_pipeline_morph_to_INT(m, m_pipeline_stage_decode, int_tripped);
		}
	}
	/* Else their was a hazard on the predication register */
	else {
		m_pipeline_set_latch_to_NOP(m, m_pipeline_stage_decode);
	}
	return;
}
