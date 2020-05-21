#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../src/miniat_defines.h"
#include "../../src/miniat_hazards.h"
#include "../../src/miniat_registers.h"
#include "../../src/miniat_structures.h"
#include "miniat/miniat.h"
#include "CuTest.h"

void t_error___exec_bus_write(CuTest *tc) {

	int i;
	FILE *bin_file;
	miniat *miniat;
	m_uword data;
	m_pipeline_decode_latch_out decode_out = {0};
	m_pipeline_execute_latch_out execute_out = {0};

	char *path = BIN_OUT_DIR"/bus_write.bin";
	bin_file = fopen(path, "r+b");
	CuAssert(tc, "Should not be NULL", bin_file != NULL);

	miniat = miniat_new (bin_file, CACHE_DIR"/hit.lua");
	CuAssert(tc, "Should not be NULL", miniat != NULL);

	for (i=0; i <= 8; i++) {

		miniat_clock(miniat);

		switch (i) {
		case 0:
			CuAssert(tc, "First instruction word not fetched properly", miniat->pipeline.fetch.out.data[M_LOW] == 0x10000);
			CuAssert(tc, "Second instruction word not fetched properly", miniat->pipeline.fetch.out.data[M_HIGH] == 0xBADCC0DE);

			CuAssert(tc, "NPC incorrect", miniat->pipeline.decode.in.NPC == 0x2002);
			CuAssert(tc, "First instruction word incorrect", miniat->pipeline.decode.in.data[M_LOW] == 0x10000);
			CuAssert(tc, "High instruction word incorrect", miniat->pipeline.decode.in.data[M_HIGH] == 0xBADCC0DE);

			break;

		case 1:
			CuAssert(tc, "NPC incorrect", miniat->pipeline.decode.out.NPC == 0x2002);
			CuAssert(tc, "Hint incorrect", miniat->pipeline.decode.out.hint == 0);
			CuAssert(tc, "A incorrect", miniat->pipeline.decode.out.A == 0);
			CuAssert(tc, "B incorrect", miniat->pipeline.decode.out.B == 0);
			CuAssert(tc, "D incorrect", miniat->pipeline.decode.out.D == 0xBADCC0DE);

			CuAssert(tc, "Instruction should be load", miniat->pipeline.decode.out.opcode == M_ADD);
			CuAssert(tc, "rX incorrect", miniat->pipeline.decode.out.rX == 1);

			CuAssert(tc, "First instruction word not fetched properly", miniat->pipeline.fetch.out.data[M_LOW] == 0x11010000);
			CuAssert(tc, "Second instruction word not fetched properly", miniat->pipeline.fetch.out.data[M_HIGH] == 0x4004);

			break;

		case 2:
			CuAssert(tc, "Decode Out incorrect", memcmp(&(miniat->pipeline.decode.out), &decode_out, sizeof(miniat->pipeline.decode.out)) == 0);
			CuAssert(tc, "State incorrect", miniat->pipeline.decode.state == m_pipeline_stage_state_data_hazard);

			CuAssert(tc, "NPC incorrect", miniat->pipeline.execute.out.NPC == 0x2002);
			CuAssert(tc, "Hint incorrect", miniat->pipeline.execute.out.hint == 0);
			CuAssert(tc, "B incorrect", miniat->pipeline.execute.out.result == 0xBADCC0DE);
			CuAssert(tc, "D incorrect", miniat->pipeline.execute.out.D == 0xBADCC0DE);

			CuAssert(tc, "Instruction should be load", miniat->pipeline.execute.out.opcode == M_ADD);
			CuAssert(tc, "rX incorrect", miniat->pipeline.execute.out.rX == 1);

			break;

		case 3:

			CuAssert(tc, "State incorrect", miniat->pipeline.decode.state == m_pipeline_stage_state_no_hazard);
			CuAssert(tc, "NPC incorrect", miniat->pipeline.decode.out.NPC == 0x2004);
			CuAssert(tc, "Hint incorrect", miniat->pipeline.decode.out.hint == 0);
			CuAssert(tc, "A incorrect", miniat->pipeline.decode.out.A == 0xBADCC0DE);
			CuAssert(tc, "B incorrect", miniat->pipeline.decode.out.B == 0);
			CuAssert(tc, "D incorrect", miniat->pipeline.decode.out.D == 0x4004);

			CuAssert(tc, "Instruction should be load", miniat->pipeline.decode.out.opcode == M_STORE);
			CuAssert(tc, "rX incorrect", miniat->pipeline.decode.out.rX == 1);


			CuAssert(tc, "Execute Out incorrect", memcmp(&(miniat->pipeline.execute.out), &execute_out, sizeof(miniat->pipeline.execute.out)) == 0);

			m_registers_read(miniat, 1, &data);
			CuAssert(tc, "Register 1 should be 0xBADCC0DE", data == 0xBADCC0DE);

			break;

		case 4:
			CuAssert(tc, "NPC incorrect", miniat->pipeline.execute.out.NPC == 0x2004);
			CuAssert(tc, "Hint incorrect", miniat->pipeline.execute.out.hint == 0);
			CuAssert(tc, "B incorrect", miniat->pipeline.execute.out.result == 0xBADCC0DE);
			CuAssert(tc, "D incorrect", miniat->pipeline.execute.out.D == 0x4004);

			CuAssert(tc, "Instruction should be load", miniat->pipeline.execute.out.opcode == M_STORE);
			CuAssert(tc, "rX incorrect", miniat->pipeline.execute.out.rX == 1);

			CuAssert(tc, "NPC incorrect", miniat->pipeline.writeback.in.NPC == 0x2004);
			CuAssert(tc, "Hint incorrect", miniat->pipeline.writeback.in.hint == 0);
			CuAssert(tc, "B incorrect", miniat->pipeline.writeback.in.result == 0xBADCC0DE);
			CuAssert(tc, "D incorrect", miniat->pipeline.writeback.in.D == 0x4004);

			CuAssert(tc, "Instruction should be load", miniat->pipeline.writeback.in.opcode == M_STORE);
			CuAssert(tc, "rX incorrect", miniat->pipeline.writeback.in.rX == 1);

			CuAssert(tc, "Bus state not as expected", miniat->bus.state == m_bus_state_initial);
			CuAssert(tc, "Bus state not as expected", miniat->bus.pub.ack == M_LOW);
			CuAssert(tc, "Bus state not as expected", miniat->bus.pub.req == M_LOW);

			break;

		case 5:
		case 6:
			CuAssert(tc, "State incorrect", miniat->pipeline.writeback.state == m_pipeline_stage_state_structural_peripheral_hazard);

			CuAssert(tc, "NPC incorrect", miniat->pipeline.writeback.in.NPC == 0x2004);
			CuAssert(tc, "Hint incorrect", miniat->pipeline.writeback.in.hint == 0);
			CuAssert(tc, "B incorrect", miniat->pipeline.writeback.in.result == 0xBADCC0DE);
			CuAssert(tc, "D incorrect", miniat->pipeline.writeback.in.D == 0x4004);

			CuAssert(tc, "Instruction should be load", miniat->pipeline.writeback.in.opcode == M_STORE);
			CuAssert(tc, "rX incorrect", miniat->pipeline.writeback.in.rX == 1);

			CuAssert(tc, "Bus state not as expected", miniat->bus.state == m_bus_state_transfer);
			CuAssert(tc, "Bus state not as expected", miniat->bus.pub.ack == M_LOW);
			CuAssert(tc, "Bus state not as expected", miniat->bus.pub.req == M_HIGH);
			CuAssert(tc, "Bus state not as expected", miniat->bus.pub.address == 0x4004);
			CuAssert(tc, "Bus state not as expected", miniat->bus.pub.data == 0xBADCC0DE);

			if (i == 6) {
				miniat->bus.pub.ack = M_HIGH;
			}
			break;

		case 7:
			CuAssert(tc, "State incorrect", miniat->pipeline.writeback.state == m_pipeline_stage_state_structural_peripheral_hazard);

			CuAssert(tc, "NPC incorrect", miniat->pipeline.writeback.in.NPC == 0x2004);
			CuAssert(tc, "Hint incorrect", miniat->pipeline.writeback.in.hint == 0);
			CuAssert(tc, "B incorrect", miniat->pipeline.writeback.in.result == 0xBADCC0DE);
			CuAssert(tc, "D incorrect", miniat->pipeline.writeback.in.D == 0x4004);

			CuAssert(tc, "Instruction should be load", miniat->pipeline.writeback.in.opcode == M_STORE);
			CuAssert(tc, "rX incorrect", miniat->pipeline.writeback.in.rX == 1);

			CuAssert(tc, "Bus state not as expected", miniat->bus.state == m_bus_state_final);
			CuAssert(tc, "Bus state not as expected", miniat->bus.pub.ack == M_HIGH);
			CuAssert(tc, "Bus state not as expected", miniat->bus.pub.req == M_LOW);
			CuAssert(tc, "Bus state not as expected", miniat->bus.pub.address == 0x4004);
			CuAssert(tc, "Bus state not as expected", miniat->bus.pub.data == 0xBADCC0DE);

			miniat->bus.pub.ack = M_LOW;

			break;

		case 8:
			CuAssert(tc, "State incorrect", miniat->pipeline.writeback.state == m_pipeline_stage_state_no_hazard);
			break;
		}
	}

	miniat_free(miniat);
	return;
}

