#include <stdio.h>
#include <stdlib.h>

#include "../../src/miniat_defines.h"
#include "../../src/miniat_hazards.h"
#include "../../src/miniat_registers.h"
#include "../../src/miniat_structures.h"
#include "miniat/miniat.h"
#include "CuTest.h"

/*
 * Tests the following code:
 *
 * ADD r2 5
 * STOR r2 0x200
 * NOP
 * LOAD r1 0x200
 *
 * Expected pipeline sequence
 *
 *   0  1  2  3  4  5  6  7  8  9
 *
 * F 1  2  3  3  4  o  o  o  o  o
 *
 * D o  1  2  2  3  4  4  o  o  o
 *
 * E o  o  o  1  o  2  3  3  4  o
 *
 * W o  o  o  1  o  2  2  3  o  4
 * 1) t_error___exec_store_load: out/vm/test/miniat_execution/store_load_asm.c:58: PC should increment by 2 from 0x2000
 */
void t_error___exec_store_load(CuTest *tc) {
/* TODO Rewrite with context of correct stall durations */
#if 0
	int i;
	FILE *bin_file;
	miniat *miniat;
	m_uword data;
	bool tmp;

	bin_file = fopen(BIN_OUT_DIR"/store_load.bin", "r+b");
	CuAssert(tc, "Should not be NULL", bin_file != NULL);

	miniat = miniat_new (bin_file, NULL);
	CuAssert(tc, "Should not be NULL", miniat != NULL);

	for (i=0; i < 11; i++) {
		miniat_clock(miniat);

		m_registers_read(miniat, M_REGISTER_PC, &data);

		switch (i) {

		/*
		 * Fetch has instruction 1
		 */
		case 0:
			/*Check PCs and NPCs*/
			m_registers_read(miniat, M_REGISTER_PC, &data);

            printf("%x\n", data);
			CuAssert(tc, "PC should increment by 2 from 0x2000", data == 0x2002);
			CuAssert(tc, "NPC should be 0x2002 for an instruction at 0x2000",
					miniat->pipeline.fetch.out.NPC == 0x2002);

			/*Check fetched data in output latch*/
			CuAssert(tc, "Low word should be 0x20000", miniat->pipeline.fetch.out.data[M_LOW] == 0x20000);
			CuAssert(tc, "High word should be 0x5",miniat->pipeline.fetch.out.data[M_HIGH] == 0x5);

			/*Check fetched data in input latch*/
			CuAssert(tc, "Low word should be 0x20000", miniat->pipeline.decode.in.data[M_LOW] == 0x20000);
			CuAssert(tc, "High word should be 0x5",miniat->pipeline.decode.in.data[M_HIGH] == 0x5);
			break;

		/*
		 * Fetch has instruction 2
		 * Decode has instruction 1
		 */
		case 1:
			/*Check PCs and NPCs*/
			m_registers_read(miniat, M_REGISTER_PC, &data);

			CuAssert(tc, "PC should increment by 2 from 0x2002", data == 0x2004);
			CuAssert(tc, "NPC should be 0x2004 for an instruction at 0x2002",
					miniat->pipeline.fetch.out.NPC == 0x2004);

			/*Check fetched data in output latch of fetch*/
			CuAssert(tc, "Low word should be 0x22020000", miniat->pipeline.fetch.out.data[M_LOW] == 0x11020000);
			CuAssert(tc, "High word should be 0x200",miniat->pipeline.fetch.out.data[M_HIGH] == 0x200);

			/*Check fetched data in input latch of decode*/
			CuAssert(tc, "Low word should be 0x22020000", miniat->pipeline.decode.in.data[M_LOW] == 0x11020000);
			CuAssert(tc, "High word should be 0x200",miniat->pipeline.decode.in.data[M_HIGH] == 0x200);
			CuAssert(tc, "NPC should be 0x2004 for an instruction at 0x2002",
					miniat->pipeline.decode.in.NPC == 0x2004);

			/*Check decoded data in output latch of decode*/
			CuAssert(tc, "Should be Register 2", miniat->pipeline.decode.out.A == 0); /*Contents of r2 0*/
			CuAssert(tc, "Should be Register 0", miniat->pipeline.decode.out.B == 0); /*r0 == 0*/
			CuAssert(tc, "Should be 0x50", miniat->pipeline.decode.out.D == 5); /*r0 + I*/
			CuAssert(tc, "NPC should not change", miniat->pipeline.decode.out.NPC == 0x2002); /*NPC should not change*/
			CuAssert(tc, "It is not hinted", miniat->pipeline.decode.out.hint == 0); /*not hinted*/
			CuAssert(tc, "It is a STOR instruction", miniat->pipeline.decode.out.opcode == M_ADD); /*ADD*/
			CuAssert(tc, "No output reg on a STOR", miniat->pipeline.decode.out.rX == 2); /*Output to r2*/

			/*Check decoded data in input latch of execute*/
			/*Check decoded data in output latch of decode*/
			CuAssert(tc, "Should be Register 2", miniat->pipeline.execute.in.A == 0); /*Contents of r2 0*/
			CuAssert(tc, "Should be Register 0", miniat->pipeline.execute.in.B == 0); /*r0 == 0*/
			CuAssert(tc, "Should be 0x50", miniat->pipeline.execute.in.D == 5); /*r0 + I*/
			CuAssert(tc, "NPC should not change", miniat->pipeline.execute.in.NPC == 0x2002); /*NPC should not change*/
			CuAssert(tc, "It is not hinted", miniat->pipeline.execute.in.hint == 0); /*not hinted*/
			CuAssert(tc, "It is a STOR instruction", miniat->pipeline.execute.in.opcode == M_ADD); /*ADD*/
			CuAssert(tc, "No output reg on a STOR", miniat->pipeline.execute.in.rX == 2); /*Output to r2*/

			/*Register 2 should be in the hazard queue*/
			tmp = m_hazards_check(miniat, 2, true);
			CuAssert(tc, "r2 should be in queue", tmp == true);
			break;

		case 2:

			/*The decode and fetch stages should be in a stall*/
			CuAssert(tc, "Should be datat hazard on fetch and decode",
				(miniat->pipeline.decode.state == m_pipeline_stage_state_data_hazard) &&
				(miniat->pipeline.fetch.state == m_pipeline_stage_state_stalled));
			/*Check PCs and NPCs this time the PC should freeze since decode is stalled waiting
			 * on the data hazard*/
			m_registers_read(miniat, M_REGISTER_PC, &data);

			/*Since were stalled PC and fetch output latches should not change from previous cycle*/
			CuAssert(tc, "PC should NOT increment last PC 0x2004", data == 0x2004);
			CuAssert(tc, "NPC should be 0x2004 since the fetch stage is stalled, it didn't update it's output latch!",
					miniat->pipeline.fetch.out.NPC == 0x2004);

			/*Check fetched data in output latch of fetch should be the same as time 1 since it didn't run
			 * the input to the stage is instruction 2 which it's address is sitting in the PC*/
			CuAssert(tc, "Low word should be 0x22020000", miniat->pipeline.fetch.out.data[M_LOW] == 0x11020000);
			CuAssert(tc, "High word should be 0x200",miniat->pipeline.fetch.out.data[M_HIGH] == 0x200);

			/*Check fetched data in input latch of decode should be the data of the instruction
			 * stalling the pipeline, ie STOR r2 0x200 which will sit in the decode stage until
			 * the hazard is cleared*/
			CuAssert(tc, "Low word should be 0", miniat->pipeline.decode.in.data[M_LOW] == 0x11020000);
			CuAssert(tc, "High word should be 0",miniat->pipeline.decode.in.data[M_HIGH] == 0x200);
			CuAssert(tc, "NPC should be 0x2004 for an instruction at 0x2002",
					miniat->pipeline.decode.in.NPC == 0x2004);

			/*Check decoded data in output latch of decode, should be bubble coming out
			 * because of stall induced by data hazard on r2*/
			CuAssert(tc, "Bubbles should be all 0", miniat->pipeline.decode.out.A == 0);
			CuAssert(tc, "Bubbles should be all 0", miniat->pipeline.decode.out.B == 0);
			CuAssert(tc, "Bubbles should be all 0", miniat->pipeline.decode.out.D == 0);
			CuAssert(tc, "Bubbles should be all 0", miniat->pipeline.decode.out.NPC == 0);
			CuAssert(tc, "Bubbles should be all 0", miniat->pipeline.decode.out.hint == 0);
			CuAssert(tc, "Bubbles should be all 0", miniat->pipeline.decode.out.opcode == M_ADD);
			CuAssert(tc, "Bubbles should be all 0", miniat->pipeline.decode.out.rX == 0);

			/*Check decoded data in input latch of execute,a  bubble out of decode should be a bubble coming into execute*/
			CuAssert(tc, "Should be Register 2", miniat->pipeline.execute.in.A == 0);
			CuAssert(tc, "Should be Register 0", miniat->pipeline.execute.in.B == 0);
			CuAssert(tc, "Should be 0x50", miniat->pipeline.execute.in.D == 0);
			CuAssert(tc, "NPC should not change", miniat->pipeline.execute.in.NPC == 0);
			CuAssert(tc, "It is not hinted", miniat->pipeline.execute.in.hint == 0);
			CuAssert(tc, "It is a STOR instruction", miniat->pipeline.execute.in.opcode == M_ADD);
			CuAssert(tc, "No output reg on a STOR", miniat->pipeline.execute.in.rX == 0);

			/* Execute output latches should be the processed data from instruction 1 ADD r2 5*/
			CuAssert(tc, "The result of rC + I should be 5", miniat->pipeline.execute.out.D == 5);
			CuAssert(tc, "NPC should not change", miniat->pipeline.execute.out.NPC == 0x2002);
			CuAssert(tc, "Not hinted", miniat->pipeline.execute.out.hint == 0);
			CuAssert(tc, "The opcode should still be add", miniat->pipeline.execute.out.opcode == M_ADD);
			CuAssert(tc, "The writeback reg should be 2", miniat->pipeline.execute.out.rX == 2);
			CuAssert(tc, "The result of addition should be 5", miniat->pipeline.execute.out.result == 5);

			/*Register 2 should STILL be in the hazard queue since it has not been written back yet*/
			tmp = m_hazards_check(miniat, 2, true);
			CuAssert(tc, "r2 should be in queue", tmp == true);
			break;

			/*
			 * At the end of cycle three, the add should have written back to r2 and cleared the stall, since
			 * we have a same cycle read after write the pipeline should be free to progress.
			 */
		case 3:

			/*Check pipeline state*/
			CuAssert(tc, "No pipeline hazard", miniat->pipeline.fetch.state == m_pipeline_stage_state_no_hazard);
			CuAssert(tc, "No pipeline hazard", miniat->pipeline.decode.state == m_pipeline_stage_state_no_hazard);
			CuAssert(tc, "No pipeline hazard", miniat->pipeline.execute.state == m_pipeline_stage_state_no_hazard);
			CuAssert(tc, "No pipeline hazard", miniat->pipeline.writeback.state == m_pipeline_stage_state_no_hazard);

			/*Register 2 should NOT be in the hazard queue since it has been written back*/
			tmp = m_hazards_check(miniat, 2, true);
			CuAssert(tc, "r2 should NOT be in hazards queue", tmp == false);

			/*Register 2 should have been written with a 5*/
			m_registers_read(miniat, 2, &data);
			CuAssert(tc, "r2 should be equal to 5", data == 5);

			/*Check PC*/
			m_registers_read(miniat, M_REGISTER_PC, &data);
			CuAssert(tc, "PC should advance from 0x2004 to 0x2006", data == 0x2006);

			/*Check fetch output latches ADD r0 r0 r0 0 is a NOP (should be all 0's)*/
			CuAssert(tc, "NPC should be 0x2008 for an instruction fetched from 0x2006",
					miniat->pipeline.fetch.out.NPC == 0x2006);

			CuAssert(tc, "NOP Low word should be 0's", miniat->pipeline.fetch.out.data[M_LOW] == 0);
			CuAssert(tc, "NOP High word should be 0's", miniat->pipeline.fetch.out.data[M_HIGH] == 0);

			/*The input latches on decode should match the fetch output latches*/
			CuAssert(tc, "NOP Low word should be 0's", miniat->pipeline.decode.in.data[M_LOW] == 0);
			CuAssert(tc, "NOP High word should be 0's", miniat->pipeline.decode.in.data[M_HIGH] == 0);

			/*Decode should have gotten done decoding STOR r2 0x200*/
			CuAssert(tc, "The contents of rA (r2) should be a 5", miniat->pipeline.decode.out.A == 5); /*This is the write we were waiting on*/
			CuAssert(tc, "The contents of rB (r0) should be 0", miniat->pipeline.decode.out.B == 0);
			CuAssert(tc, "The contents of rC + I (r0 + 0x200) should be 0x200", miniat->pipeline.decode.out.D == 0x200);
			CuAssert(tc, "The NPC for this instruction should be 0x2004", miniat->pipeline.decode.out.NPC == 0x2004);
			CuAssert(tc, "This instruction is not hinted", miniat->pipeline.decode.out.hint == 0);
			CuAssert(tc, "The opcode should be for STOR", miniat->pipeline.decode.out.opcode == M_STORE);
			CuAssert(tc, "The writeback register, unused for STOR, should be r2", miniat->pipeline.decode.out.rX  == 2); /*Unsued on STOR, but should be r2*/

			/*Execute in should match decode out*/
			CuAssert(tc, "The contents of rA (r2) should be a 5", miniat->pipeline.execute.in.A == 5);
			CuAssert(tc, "The contents of rB (r0) should be 0", miniat->pipeline.execute.in.B == 0);
			CuAssert(tc, "The contents of rC + I (r0 + 0x200) should be 0x200", miniat->pipeline.execute.in.D == 0x200);
			CuAssert(tc, "The NPC for this instruction should be 0x2004", miniat->pipeline.execute.in.NPC == 0x2004);
			CuAssert(tc, "This instruction is not hinted", miniat->pipeline.execute.in.hint == 0);
			CuAssert(tc, "The opcode should be for STOR", miniat->pipeline.execute.in.opcode == M_STORE);
			CuAssert(tc, "The writeback register, unused for STOR, should be r2", miniat->pipeline.execute.in.rX  == 2);

			/*Execute should have pushed a NOP for*/
			CuAssert(tc, "NOPs should be all 0s", miniat->pipeline.execute.out.D == 0);
			CuAssert(tc, "NOPs should be all 0s", miniat->pipeline.execute.out.NPC == 0);
			CuAssert(tc, "NOPs should be all 0s", miniat->pipeline.execute.out.hint == 0);
			CuAssert(tc, "NOPs should be all 0s", miniat->pipeline.execute.out.opcode == M_ADD);
			CuAssert(tc, "NOPs should be all 0s", miniat->pipeline.execute.out.rX == 0);
			CuAssert(tc, "NOPs should be all 0s", miniat->pipeline.execute.out.result == 0);

			/*Writeback should have accepted a bubble*/
			CuAssert(tc, "NOPs should be all 0s", miniat->pipeline.writeback.in.D == 0);
			CuAssert(tc, "NOPs should be all 0s", miniat->pipeline.writeback.in.NPC == 0);
			CuAssert(tc, "NOPs should be all 0s", miniat->pipeline.writeback.in.hint == 0);
			CuAssert(tc, "NOPs should be all 0s", miniat->pipeline.writeback.in.opcode == M_ADD);
			CuAssert(tc, "NOPs should be all 0s", miniat->pipeline.writeback.in.rX == 0);
			CuAssert(tc, "NOPs should be all 0s", miniat->pipeline.writeback.in.result == 0);

			/*In cycle 2, decode was in a stall, now that it has run
			 Decode should have added 0x200 to the hazards queue since a write is occurring there*/
			tmp = m_hazards_check(miniat, 0x200, false);
			CuAssert(tc, "Memory address 0x200 should be in the queue", tmp == true);
			break;

		case 4:

			/*Their should be no stall in the pipeline*/
			CuAssert(tc, "", miniat->pipeline.fetch.state == m_pipeline_stage_state_no_hazard);
			CuAssert(tc, "", miniat->pipeline.decode.state == m_pipeline_stage_state_no_hazard);
			CuAssert(tc, "", miniat->pipeline.execute.state == m_pipeline_stage_state_no_hazard);
			CuAssert(tc, "", miniat->pipeline.writeback.state == m_pipeline_stage_state_no_hazard);

			/*Register 2 should still contain a 5*/
			m_registers_read(miniat, 2, &data);
			CuAssert(tc, "r2 should be equal to 5", data == 5);

			/*PC Should be on 0x2008*/
			m_registers_read(miniat, M_REGISTER_PC, &data);
			CuAssert(tc, "PC should advance from 0x2006 to 0x2008", data == 0x2008);

			/*Memory address 0x200 should still be in hazard queue since STORE has not completed*/
			tmp = m_hazards_check(miniat, 0x200, false);
			CuAssert(tc, "Memory address 0x200 should be in the queue", tmp == true);

			/*Check fetch output latches LOAD r1 0x50*/
			CuAssert(tc, "NPC should be 0x2008 for an instruction fetched from 0x2006",
					miniat->pipeline.fetch.out.NPC == 0x2008);

			CuAssert(tc, "Low word for LOAD r1 0x200", miniat->pipeline.fetch.out.data[M_LOW] == 0x10010000);
			CuAssert(tc, "High word for LOAD r1 0x200", miniat->pipeline.fetch.out.data[M_HIGH] == 0x00000200);

			/*The input latches on decode should match the fetch output latches*/
			CuAssert(tc, "Low word for LOAD r1 0x200", miniat->pipeline.decode.in.data[M_LOW] == 0x10010000);
			CuAssert(tc, "High word for LOAD r1 0x200", miniat->pipeline.decode.in.data[M_HIGH] == 0x00000200);

			/*Decode should have gotten done decoding a ADD r0 r0 r0 0 (NOP)*/
			CuAssert(tc, "NOP", miniat->pipeline.decode.out.A == 0); /*This is the write we were waiting on*/
			CuAssert(tc, "NOP", miniat->pipeline.decode.out.B == 0);
			CuAssert(tc, "NOP", miniat->pipeline.decode.out.D == 0);
			CuAssert(tc, "NOP", miniat->pipeline.decode.out.NPC == 0x2006); /*This NOP was not generated, but fetched so it has a NPC*/
			CuAssert(tc, "NOP", miniat->pipeline.decode.out.hint == 0);
			CuAssert(tc, "NOP", miniat->pipeline.decode.out.opcode == M_ADD);
			CuAssert(tc, "NOP", miniat->pipeline.decode.out.rX  == 0);

			/*Execute in should match decode out*/
			CuAssert(tc, "NOP", miniat->pipeline.execute.in.A == 0);
			CuAssert(tc, "NOP", miniat->pipeline.execute.in.B == 0);
			CuAssert(tc, "NOP", miniat->pipeline.execute.in.D == 0);
			CuAssert(tc, "NOP", miniat->pipeline.execute.in.NPC == 0x2006);
			CuAssert(tc, "NOP", miniat->pipeline.execute.in.hint == 0);
			CuAssert(tc, "NOP", miniat->pipeline.execute.in.opcode == M_ADD);
			CuAssert(tc, "NOP", miniat->pipeline.execute.in.rX  == 0);

			/*Execute should have STOR r2 0x200*/
			CuAssert(tc, "Store to address should be 0x200", miniat->pipeline.execute.out.D == 0x200);
			CuAssert(tc, "NPC should be 0x2004", miniat->pipeline.execute.out.NPC == 0x2004);
			CuAssert(tc, "Not hinted", miniat->pipeline.execute.out.hint == 0);
			CuAssert(tc, "STORE instruction", miniat->pipeline.execute.out.opcode == M_STORE);
			CuAssert(tc, "Should r2 (Unused)", miniat->pipeline.execute.out.rX == 2);
			CuAssert(tc, "The value to write should be a 5", miniat->pipeline.execute.out.result == 5);

			/*Writeback should have accepted a STOR r2 0x200*/
			CuAssert(tc, "Store to address should be 0x200", miniat->pipeline.writeback.in.D == 0x200);
			CuAssert(tc, "NPC should be 0x2004", miniat->pipeline.writeback.in.NPC == 0x2004);
			CuAssert(tc, "Not hinted", miniat->pipeline.writeback.in.hint == 0);
			CuAssert(tc, "STORE instruction", miniat->pipeline.writeback.in.opcode == M_STORE);
			CuAssert(tc, "Should r2 (Unused)", miniat->pipeline.writeback.in.rX == 2);
			CuAssert(tc, "The value to write should be a 5", miniat->pipeline.writeback.in.result == 5);
			break;

			/*Stall on STOR */
		case 5:

			/*While we wait on the STOR to complete, the pipeline should be in a data structural hazard*/
			CuAssert(tc, "Stalled waiting on slow memory write", miniat->pipeline.fetch.state == m_pipeline_stage_state_stalled);
			CuAssert(tc, "Stalled waiting on slow memory write", miniat->pipeline.decode.state == m_pipeline_stage_state_stalled);
			CuAssert(tc, "Stalled waiting on slow memory write", miniat->pipeline.execute.state == m_pipeline_stage_state_stalled);
			CuAssert(tc, "Stalled waiting on slow memory write", miniat->pipeline.writeback.state == m_pipeline_stage_state_structural_memory_hazard);

			/*Register 2 should still contain a 5*/
			m_registers_read(miniat, 2, &data);
			CuAssert(tc, "r2 should be equal to 5", data == 5);

			/*Ahh were stalled, no PC advance!*/
			m_registers_read(miniat, M_REGISTER_PC, &data);
			CuAssert(tc, "PC should advance from 0x2006 to 0x2008", data == 0x2008);

			/*Check fetch output latches for NOP*/
			CuAssert(tc, "NPC should be 0x2008 since were stalled",
					miniat->pipeline.fetch.out.NPC == 0x2008);

			/*0x200 still chilling the hazard queue*/
			tmp = m_hazards_check(miniat, 0x200, false);
			CuAssert(tc, "Memory address 0x200 should be in the queue", tmp == true);

			/* The PC is still on the old cycle and fetch did not run since their is a stall, so all the data in the
			 * latches should not change
			 */
			CuAssert(tc, "Low word for LOAD r1 0x200", miniat->pipeline.fetch.out.data[M_LOW] == 0x10010000);
			CuAssert(tc, "High word for LOAD r1 0x200", miniat->pipeline.fetch.out.data[M_HIGH] == 0x00000200);

			/*The input latches on decode should still be a LOAD r1 0x200 since were stalled*/
			CuAssert(tc, "Low word for LOAD r1 0x200", miniat->pipeline.decode.in.data[M_LOW] == 0x10010000);
			CuAssert(tc, "High word for LOAD r1 0x200", miniat->pipeline.decode.in.data[M_HIGH] == 0x00000200);

			/*Decode should have gotten done decoding a ADD r0 r0 r0 0 (NOP)*/
			CuAssert(tc, "NOP", miniat->pipeline.decode.out.A == 0); /*This is the write we were waiting on*/
			CuAssert(tc, "NOP", miniat->pipeline.decode.out.B == 0);
			CuAssert(tc, "NOP", miniat->pipeline.decode.out.D == 0);
			CuAssert(tc, "NOP", miniat->pipeline.decode.out.NPC == 0x2006); /*This NOP was not generated, but fetched so it has a NPC*/
			CuAssert(tc, "NOP", miniat->pipeline.decode.out.hint == 0);
			CuAssert(tc, "NOP", miniat->pipeline.decode.out.opcode == M_ADD);
			CuAssert(tc, "NOP", miniat->pipeline.decode.out.rX  == 0);

			/*Execute in should match decode out*/
			CuAssert(tc, "NOP", miniat->pipeline.execute.in.A == 0);
			CuAssert(tc, "NOP", miniat->pipeline.execute.in.B == 0);
			CuAssert(tc, "NOP", miniat->pipeline.execute.in.D == 0);
			CuAssert(tc, "NOP", miniat->pipeline.execute.in.NPC == 0x2006);
			CuAssert(tc, "NOP", miniat->pipeline.execute.in.hint == 0);
			CuAssert(tc, "NOP", miniat->pipeline.execute.in.opcode == M_ADD);
			CuAssert(tc, "NOP", miniat->pipeline.execute.in.rX  == 0);

			/*Execute should have STOR r2 0x200*/
			CuAssert(tc, "STOR r2 0x200", miniat->pipeline.execute.out.D == 0x200);
			CuAssert(tc, "STOR r2 0x200", miniat->pipeline.execute.out.NPC == 0x2004);
			CuAssert(tc, "STOR r2 0x200", miniat->pipeline.execute.out.hint == 0);
			CuAssert(tc, "STOR r2 0x200", miniat->pipeline.execute.out.opcode == M_STORE);
			CuAssert(tc, "STOR r2 0x200", miniat->pipeline.execute.out.rX == 2);
			CuAssert(tc, "STOR r2 0x200", miniat->pipeline.execute.out.result == 5);

			/*Writeback should have accepted a STOR r2 0x200*/
			CuAssert(tc, "STOR r2 0x200", miniat->pipeline.writeback.in.D == 0x200);
			CuAssert(tc, "STOR r2 0x200", miniat->pipeline.writeback.in.NPC == 0x2004);
			CuAssert(tc, "STOR r2 0x200", miniat->pipeline.writeback.in.hint == 0);
			CuAssert(tc, "STOR r2 0x200", miniat->pipeline.writeback.in.opcode == M_STORE);
			CuAssert(tc, "STOR r2 0x200", miniat->pipeline.writeback.in.rX == 2);
			CuAssert(tc, "STOR r2 0x200", miniat->pipeline.writeback.in.result == 5);
			break;


			/*
			 * In this cycle, the memory STOR is completed, and the hazard is removed, all the stages execute their input latches
			 * and the latch shuffle occurs (Pushing the NOP into WB).
			 */
		case 6:

			/*Hazards should cleared out*/
			CuAssert(tc, "Pipeline should be hazard free", miniat->pipeline.fetch.state == m_pipeline_stage_state_no_hazard);
			CuAssert(tc, "Pipeline should be hazard free", miniat->pipeline.decode.state == m_pipeline_stage_state_no_hazard);
			CuAssert(tc, "Pipeline should be hazard free", miniat->pipeline.execute.state == m_pipeline_stage_state_no_hazard);
			CuAssert(tc, "Pipeline should be hazard free", miniat->pipeline.writeback.state == m_pipeline_stage_state_no_hazard);

			/*Register 2 should still contain a 5*/
			m_registers_read(miniat, 2, &data);
			CuAssert(tc, "r2 should be equal to 5", data == 5);

			/*PC should be advancing to 0x200A*/
			m_registers_read(miniat, M_REGISTER_PC, &data);
			CuAssert(tc, "PC should advance from 0x2008 to 0x200A", data == 0x200A);

			/*Check fetch output latches for NOP*/
			CuAssert(tc, "NPC should be 0x200A",
					miniat->pipeline.fetch.out.NPC == 0x200A);

			/*0x200 should no longer be chilling in the hazard queue*/
			tmp = m_hazards_check(miniat, 0x200, false);
			CuAssert(tc, "Memory address 0x200 should NOT be in the queue, the STOR is done!", tmp == false);

			/*r1 should have been added to the hazard queue LOAD r1 0x200 has gone through the decode stage*/
			tmp = m_hazards_check(miniat, 1, true);
			CuAssert(tc, "r1 should be in the queue!", tmp == true);


			/*Fetch should have grabbed a NOP since everything is initialized to 0s*/
			CuAssert(tc, "Low word for NOP", miniat->pipeline.fetch.out.data[M_LOW] == 0);
			CuAssert(tc, "High word for NOP", miniat->pipeline.fetch.out.data[M_HIGH] == 0);

			/*The input latches on decode should made fetch's output latches*/
			CuAssert(tc, "Low word for a NOP", miniat->pipeline.decode.in.data[M_LOW] == 0);
			CuAssert(tc, "High word for a NOP", miniat->pipeline.decode.in.data[M_HIGH] == 0);

			/*Decode should have processed that LOAD r1 0x200 and set it to it's output latches*/
			CuAssert(tc, "Decode out", miniat->pipeline.decode.out.A == 0);
			CuAssert(tc, "Decode out", miniat->pipeline.decode.out.B == 0);
			CuAssert(tc, "Decode out", miniat->pipeline.decode.out.D == 0x200);
			CuAssert(tc, "Decode out", miniat->pipeline.decode.out.NPC == 0x2008);
			CuAssert(tc, "Decode out", miniat->pipeline.decode.out.hint == 0);
			CuAssert(tc, "Decode out", miniat->pipeline.decode.out.opcode == M_LOAD);
			CuAssert(tc, "Decode out", miniat->pipeline.decode.out.rX  == 1);

			/*Execute in should match decode out*/
			CuAssert(tc, "Execute in", miniat->pipeline.execute.in.A == 0);
			CuAssert(tc, "Execute in", miniat->pipeline.execute.in.B == 0);
			CuAssert(tc, "Execute in", miniat->pipeline.execute.in.D == 0x200);
			CuAssert(tc, "Execute in", miniat->pipeline.execute.in.NPC == 0x2008);
			CuAssert(tc, "Execute in", miniat->pipeline.execute.in.hint == 0);
			CuAssert(tc, "Execute in", miniat->pipeline.execute.in.opcode == M_LOAD);
			CuAssert(tc, "Execute in", miniat->pipeline.execute.in.rX  == 1);

			/*Execute should have ran a NOP*/
			CuAssert(tc, "Execute out", miniat->pipeline.execute.out.D == 0);
			CuAssert(tc, "Execute out", miniat->pipeline.execute.out.NPC == 0x2006);
			CuAssert(tc, "Execute out", miniat->pipeline.execute.out.hint == 0);
			CuAssert(tc, "Execute out", miniat->pipeline.execute.out.opcode == M_ADD);
			CuAssert(tc, "Execute out", miniat->pipeline.execute.out.rX == 0);
			CuAssert(tc, "Execute out", miniat->pipeline.execute.out.result == 0);

			/*Writeback should have ran the STOR r2*/
			CuAssert(tc, "Writeback in", miniat->pipeline.writeback.in.D == 0);
			CuAssert(tc, "Writeback in", miniat->pipeline.writeback.in.NPC == 0x2006);
			CuAssert(tc, "Writeback in", miniat->pipeline.writeback.in.hint == 0);
			CuAssert(tc, "Writeback in", miniat->pipeline.writeback.in.opcode == M_ADD);
			CuAssert(tc, "Writeback in", miniat->pipeline.writeback.in.rX == 0);
			CuAssert(tc, "Writeback in", miniat->pipeline.writeback.in.result == 0);

			break;

			/*Stall on LOAD*/
		case 7:

			/*Their should be a structural hazard in execute for the LOAD instruction*/
			CuAssert(tc, "Pipeline should be hazard free", miniat->pipeline.fetch.state == m_pipeline_stage_state_stalled);
			CuAssert(tc, "Pipeline should be hazard free", miniat->pipeline.decode.state == m_pipeline_stage_state_stalled);
			CuAssert(tc, "Pipeline should be hazard free", miniat->pipeline.execute.state == m_pipeline_stage_state_structural_memory_hazard);
			CuAssert(tc, "Pipeline should be hazard free", miniat->pipeline.writeback.state == m_pipeline_stage_state_no_hazard);

			/*Register 2 should still contain a 5*/
			m_registers_read(miniat, 2, &data);
			CuAssert(tc, "r2 should be equal to 5", data == 5);

			/*PC should be stalled on 0x200A*/
			m_registers_read(miniat, M_REGISTER_PC, &data);
			CuAssert(tc, "PC should be stalled on LOAD structural data hazard at 0x200A", data == 0x200A);

			/*0x200 should no longer be chilling in the hazard queue*/
			tmp = m_hazards_check(miniat, 0x200, false);
			CuAssert(tc, "Memory address 0x200 should NOT be in the queue, the STOR is done!", tmp == false);

			/*r1 should be in the hazard queue*/
			tmp = m_hazards_check(miniat, 1, true);
			CuAssert(tc, "r1 is in the hazard queue!", tmp == true);

			/*Check fetch output latches for NOP*/
			CuAssert(tc, "NPC should be 0x200A",
					miniat->pipeline.fetch.out.NPC == 0x200A);

			/*Fetch should have grabbed a NOP since everything is initialized to 0s*/
			CuAssert(tc, "Low word for NOP", miniat->pipeline.fetch.out.data[M_LOW] == 0);
			CuAssert(tc, "High word for NOP", miniat->pipeline.fetch.out.data[M_HIGH] == 0);

			/*Decode should be hanging onto the previous fetched NOP from uninitialized memory*/
			CuAssert(tc, "Low word for a NOP", miniat->pipeline.decode.in.data[M_LOW] == 0);
			CuAssert(tc, "High word for a NOP", miniat->pipeline.decode.in.data[M_HIGH] == 0);

			/*
			 * Decode should have the LOAD still sitting in the output latches since were stalled and decode
			 * did not run
			 */
			CuAssert(tc, "decode out", miniat->pipeline.decode.out.A == 0);
			CuAssert(tc, "decode out", miniat->pipeline.decode.out.B == 0);
			CuAssert(tc, "decode out", miniat->pipeline.decode.out.D == 0x200);
			CuAssert(tc, "decode out", miniat->pipeline.decode.out.NPC == 0x2008);
			CuAssert(tc, "decode out", miniat->pipeline.decode.out.hint == 0);
			CuAssert(tc, "decode out", miniat->pipeline.decode.out.opcode == M_LOAD);
			CuAssert(tc, "decode out", miniat->pipeline.decode.out.rX  == 1);

			/*Execute in should match decode out*/
			CuAssert(tc, "execute in", miniat->pipeline.execute.in.A == 0);
			CuAssert(tc, "execute in", miniat->pipeline.execute.in.B == 0);
			CuAssert(tc, "execute in", miniat->pipeline.execute.in.D == 0x200);
			CuAssert(tc, "execute in", miniat->pipeline.execute.in.NPC == 0x2008);
			CuAssert(tc, "execute in", miniat->pipeline.execute.in.hint == 0);
			CuAssert(tc, "execute in", miniat->pipeline.execute.in.opcode == M_LOAD);
			CuAssert(tc, "execute in", miniat->pipeline.execute.in.rX  == 1);

			/*Execute should have pushed a bubble (NOP with garbage NPC)*/
			CuAssert(tc, "execute out", miniat->pipeline.execute.out.D == 0);
			CuAssert(tc, "execute out", miniat->pipeline.execute.out.NPC == 0);
			CuAssert(tc, "execute out", miniat->pipeline.execute.out.hint == 0);
			CuAssert(tc, "execute out", miniat->pipeline.execute.out.opcode == M_ADD);
			CuAssert(tc, "execute out", miniat->pipeline.execute.out.rX == 0);
			CuAssert(tc, "execute out", miniat->pipeline.execute.out.result == 0);

			/*Writeback should have the bubble from execute in it's input latches, ready to run!*/
			CuAssert(tc, "writeback in", miniat->pipeline.writeback.in.D == 0);
			CuAssert(tc, "writeback in", miniat->pipeline.writeback.in.NPC == 0);
			CuAssert(tc, "writeback in", miniat->pipeline.writeback.in.hint == 0);
			CuAssert(tc, "writeback in", miniat->pipeline.writeback.in.opcode == M_ADD);
			CuAssert(tc, "writeback in", miniat->pipeline.writeback.in.rX == 0);
			CuAssert(tc, "writeback in", miniat->pipeline.writeback.in.result == 0);
			break;

			/*Load is finished in this cycle and all the stages execute their input latches*/
		case 8:

			/*The pipeline should be clear*/
			CuAssert(tc, "Pipeline should be hazard free", miniat->pipeline.fetch.state == m_pipeline_stage_state_no_hazard);
			CuAssert(tc, "Pipeline should be hazard free", miniat->pipeline.decode.state == m_pipeline_stage_state_no_hazard);
			CuAssert(tc, "Pipeline should be hazard free", miniat->pipeline.execute.state == m_pipeline_stage_state_no_hazard);
			CuAssert(tc, "Pipeline should be hazard free", miniat->pipeline.writeback.state == m_pipeline_stage_state_no_hazard);

			/*Register 2 should still contain a 5*/
			m_registers_read(miniat, 2, &data);
			CuAssert(tc, "r2 should be equal to 5", data == 5);

			/*PC should have advanced to 0x200C*/
			m_registers_read(miniat, M_REGISTER_PC, &data);
			CuAssert(tc, "PC should be stalled on LOAD structural data hazard at 0x200A", data == 0x200C);

			/*r1 should still be in the hazard queue*/
			tmp = m_hazards_check(miniat, 1, true);
			CuAssert(tc, "r1 is in the hazard queue!", tmp == true);

			/*Check fetch output latches for NOP*/
			CuAssert(tc, "NPC should be 0x200C for an instruction fetched from 0x200A",
					miniat->pipeline.fetch.out.NPC == 0x200C);

			/*Fetch should have grabbed a NOP since everything is initialized to 0s*/
			CuAssert(tc, "Low word for NOP", miniat->pipeline.fetch.out.data[M_LOW] == 0);
			CuAssert(tc, "High word for NOP", miniat->pipeline.fetch.out.data[M_HIGH] == 0);

			/*Decode's in latches should have been pused the NOP fetched*/
			CuAssert(tc, "Low word for a NOP", miniat->pipeline.decode.in.data[M_LOW] == 0);
			CuAssert(tc, "High word for a NOP", miniat->pipeline.decode.in.data[M_HIGH] == 0);

			/* Should have ran a fetched NOP */
			CuAssert(tc, "", miniat->pipeline.decode.out.A == 0);
			CuAssert(tc, "", miniat->pipeline.decode.out.B == 0);
			CuAssert(tc, "", miniat->pipeline.decode.out.D == 0);
			CuAssert(tc, "", miniat->pipeline.decode.out.NPC == 0x200A);
			CuAssert(tc, "", miniat->pipeline.decode.out.hint == 0);
			CuAssert(tc, "", miniat->pipeline.decode.out.opcode == M_ADD);
			CuAssert(tc, "", miniat->pipeline.decode.out.rX  == 0);

			/*Execute in should match decode out*/
			CuAssert(tc, "execute in", miniat->pipeline.execute.in.A == 0);
			CuAssert(tc, "execute in", miniat->pipeline.execute.in.B == 0);
			CuAssert(tc, "execute in", miniat->pipeline.execute.in.D == 0);
			CuAssert(tc, "execute in", miniat->pipeline.execute.in.NPC == 0x200A);
			CuAssert(tc, "execute in", miniat->pipeline.execute.in.hint == 0);
			CuAssert(tc, "execute in", miniat->pipeline.execute.in.opcode == M_ADD);
			CuAssert(tc, "execute in", miniat->pipeline.execute.in.rX  == 0);

			/*Execute out should be a load*/
			CuAssert(tc, "execute out", miniat->pipeline.execute.out.D == 0x200);
			CuAssert(tc, "execute out", miniat->pipeline.execute.out.NPC == 0x2008);
			CuAssert(tc, "execute out", miniat->pipeline.execute.out.hint == 0);
			CuAssert(tc, "execute out", miniat->pipeline.execute.out.opcode == M_LOAD);
			CuAssert(tc, "execute out", miniat->pipeline.execute.out.rX == 1);
			CuAssert(tc, "execute out", miniat->pipeline.execute.out.result == 5); /*Their should be a 5 in mem[0x200]*/

			/*Writeback should have the bubble pushed from execute in it's input latches, ready to run!*/
			CuAssert(tc, "writeback in", miniat->pipeline.writeback.in.D == 0x200);
			CuAssert(tc, "writeback in", miniat->pipeline.writeback.in.NPC == 0x2008);
			CuAssert(tc, "writeback in", miniat->pipeline.writeback.in.hint == 0);
			CuAssert(tc, "writeback in", miniat->pipeline.writeback.in.opcode == M_LOAD);
			CuAssert(tc, "writeback in", miniat->pipeline.writeback.in.rX == 1);
			CuAssert(tc, "writeback in", miniat->pipeline.writeback.in.result == 5);
			break;

		case 9:

			/*The pipeline should be clear*/
			CuAssert(tc, "Pipeline should be hazard free", miniat->pipeline.fetch.state == m_pipeline_stage_state_no_hazard);
			CuAssert(tc, "Pipeline should be hazard free", miniat->pipeline.decode.state == m_pipeline_stage_state_no_hazard);
			CuAssert(tc, "Pipeline should be hazard free", miniat->pipeline.execute.state == m_pipeline_stage_state_no_hazard);
			CuAssert(tc, "Pipeline should be hazard free", miniat->pipeline.writeback.state == m_pipeline_stage_state_no_hazard);

			/*Register 2 should still contain a 5*/
			m_registers_read(miniat, 2, &data);
			CuAssert(tc, "r2 should be equal to 5", data == 5);

			/*PC should have advanced to 0x200E*/
			m_registers_read(miniat, M_REGISTER_PC, &data);
			CuAssert(tc, "PC should advance to 0x200E", data == 0x200E);

			/*r1 should NOT be in the hazard queue*/
			tmp = m_hazards_check(miniat, 1, true);
			CuAssert(tc, "r1 is in the hazard queue!", tmp == false);

			/* r1 should have a 5 in it*/
			m_registers_read(miniat, 1, &data);
			CuAssert(tc, "r1 should have a 5 in it", data == 5);
			break;
		}
	}
#endif
}
