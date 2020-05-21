#include <stdio.h>
#include <stdlib.h>

#include "../../src/miniat_defines.h"
#include "../../src/miniat_hazards.h"
#include "../../src/miniat_registers.h"
#include "../../src/miniat_structures.h"
#include "miniat/miniat.h"
#include "CuTest.h"

void t_error___exec_bus_read(CuTest *tc) {

	int i;
	FILE *bin_file;
	miniat *miniat;
	m_uword data;

	char *path = BIN_OUT_DIR"/bus_read.bin";
	bin_file = fopen(path, "r+b");
	CuAssert(tc, "Should not be NULL", bin_file != NULL);

	miniat = miniat_new (bin_file, CACHE_DIR"/hit.lua");
	CuAssert(tc, "Should not be NULL", miniat != NULL);

	for (i=0; i <= 8; i++) {

		miniat_clock(miniat);

		switch (i) {
		case 0:
			CuAssert(tc, "First instruction word not fetched properly", miniat->pipeline.fetch.out.data[M_LOW] == 0x10010000);
			CuAssert(tc, "Second instruction word not fetched properly", miniat->pipeline.fetch.out.data[M_HIGH] == 0x4004);

			CuAssert(tc, "NPC incorrect", miniat->pipeline.decode.in.NPC == 0x2002);
			CuAssert(tc, "First instruction word incorrect", miniat->pipeline.decode.in.data[M_LOW] == 0x10010000);
			CuAssert(tc, "High instruction word incorrect", miniat->pipeline.decode.in.data[M_HIGH] == 0x4004);

			break;

		case 1:
			CuAssert(tc, "NPC incorrect", miniat->pipeline.decode.out.NPC == 0x2002);
			CuAssert(tc, "Hint incorrect", miniat->pipeline.decode.out.hint == 0);
			CuAssert(tc, "A incorrect", miniat->pipeline.decode.out.A == 0);
			CuAssert(tc, "B incorrect", miniat->pipeline.decode.out.B == 0);
			CuAssert(tc, "D incorrect", miniat->pipeline.decode.out.D = 0x4004);

			CuAssert(tc, "Instruction should be load", miniat->pipeline.decode.out.opcode == M_LOAD);
			CuAssert(tc, "rX incorrect", miniat->pipeline.decode.out.rX == 1);
			CuAssert(tc, "Bus state incorrect", miniat->bus.state == m_bus_state_initial);
			break;

		/* Don't delay for more than M_BUS_STROBE_DURATION */
		case 2:
		case 3:
			CuAssert(tc, "Pipeline state not as expected", miniat->pipeline.execute.state == m_pipeline_stage_state_structural_peripheral_hazard);
			CuAssert(tc, "Bus state not as expected", miniat->bus.state == m_bus_state_transfer);
			CuAssert(tc, "Bus state not as expected", miniat->bus.pub.ack == M_LOW);
			CuAssert(tc, "Bus state not as expected", miniat->bus.pub.req == M_HIGH);
			CuAssert(tc, "Bus state not as expected", miniat->bus.pub.rW == M_LOW);
			CuAssert(tc, "Bus state not as expected", miniat->bus.pub.address == 0x4004);

			if (i == 3) {
				miniat->bus.pub.ack = M_HIGH;
				miniat->bus.pub.data = 0x55AA;
			}
			break;

		case 4:
		case 5:
		case 6:
			CuAssert(tc, "Pipeline state not as expected", miniat->pipeline.execute.state == m_pipeline_stage_state_structural_peripheral_hazard);
			CuAssert(tc, "Bus state not as expected", miniat->bus.state = m_bus_state_final);
			CuAssert(tc, "Bus state not as expected", miniat->bus.pub.ack == M_HIGH);
			CuAssert(tc, "Bus state not as expected", miniat->bus.pub.req == M_LOW);

			if (i == 6) {
				miniat->bus.pub.ack = M_LOW;
			}
			break;

		case 7:
			CuAssert(tc, "Pipeline state not as expected", miniat->pipeline.execute.state == m_pipeline_stage_state_no_hazard);
			CuAssert(tc, "Bus state not as expected", miniat->bus.state == m_bus_state_initial);
			CuAssert(tc, "Bus state not as expected", miniat->bus.pub.ack == M_LOW);
			CuAssert(tc, "Bus state not as expected", miniat->bus.pub.req == M_LOW);

			CuAssert(tc, "NPC not as expected", miniat->pipeline.execute.out.NPC == 0x2002);
			CuAssert(tc, "Hint not as expected", miniat->pipeline.execute.out.hint == 0);
			CuAssert(tc, "opcode incorrect", miniat->pipeline.execute.out.opcode == M_LOAD);
			CuAssert(tc, "rX incorrect", miniat->pipeline.execute.out.rX == 1);
			CuAssert(tc, "Result incorrect", miniat->pipeline.execute.out.result == 0x55AA);
			break;

		case 8:
			m_registers_read(miniat, 1, &data);
			CuAssert(tc, "Register 1 should be 0x55AA", data == 0x55AA);
			break;
		}
	}

	miniat_free(miniat);
	return;
}

