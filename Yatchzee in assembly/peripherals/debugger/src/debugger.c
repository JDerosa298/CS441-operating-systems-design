#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "debugger.h"

#include "../../../vm/src/miniat_defines.h"
#include "../../../vm/src/miniat_structures.h"
#include "../../../vm/src/miniat_structures.h"
#include "../../../vm/src/miniat_typedefs.h"
#include "../../../vm/src/miniat_typedefs.h"
#include "../../../vm/src/miniat_unions.h"

enum debugger_state {
	d_main_state = 0,
	d_memory_state,
	d_register_state,
	d_latch_state,
	d_bus_state,
	d_timer_state,
	d_help_state
};

struct debugger {
	int skip;
	int reg_addr;
	int reg_i;
	int mem_addr;
	int mem_i;
	debugger_state state;
	m_pipeline_writeback_latch_in writeback;
	miniat m;
};

static int debugger_print(debugger *d, miniat *m);
static int debugger_scan(debugger *d, miniat *m);

static int debugger_main_print(debugger *d, miniat *m);
static int debugger_memory_print(debugger *d, miniat *m);
static int debugger_register_print(debugger *d, miniat *m);
static int debugger_latch_state(debugger *d, miniat *m);
static int debugger_bus_state(debugger *d, miniat *m);
static int debugger_timer_state(debugger *d, miniat *m);
static int debugger_help_print(debugger *d, miniat *m);

static void debugger_changed_clear(debugger *d, miniat *m);
static void debugger_reg_changed(debugger *d, miniat *m);
static void debugger_mem_changed(debugger *d, miniat *m);

debugger* debugger_new(miniat *m) {

	debugger *d = NULL;

	if (m) {
		d = malloc(sizeof(debugger));
		if (d) {
			memset(d, 0, sizeof(debugger));
			memcpy(&(d->m), m, sizeof(miniat));
		}
	}
	return d;
}

void debugger_free(debugger *d) {

	free(d);

	return;
}
void debugger_changed_clear(debugger *d, miniat *m) {
	d->reg_i = 0;
	d->mem_i = 0;
}
void debugger_reg_changed(debugger *d, miniat *m) {

	for (; d->reg_i < M_NUM_REGISTERS; d->reg_i++) {
		if (d->m.reg[d->reg_i] != m->reg[d->reg_i]) {
			return;
		}
	}
	return;
}
void debugger_mem_changed(debugger *d, miniat *m) {

	for (; d->mem_i < M_MEMORY_SIZE; d->mem_i++) {
		if (d->m.mem.data[d->mem_i] != m->mem.data[d->mem_i]) {
			return;
		}
	}
	return;
}
void debugger_clock(debugger *d, miniat *m) {

	if (!d->skip) {
		do {
			system("clear");
			debugger_print(d, m);

		} while (!debugger_scan(d, m));
		memcpy(&(d->m), m, sizeof(miniat));
	}
	else {
		d->skip--;
	}

	memcpy(&(d->writeback), &(m->pipeline.writeback.in), sizeof(m_pipeline_writeback_latch_in));

	return;
}

int debugger_scan(debugger *d, miniat *m) {

	char string[100];

	/* Exploits are fun, try to get around stack smashing detected */

	scanf("%s", string);
	if (!strcmp(string, "quit") || !strcmp(string, "q")) {
		exit(EXIT_SUCCESS);
	}
	else if (!strcmp(string, "home") || !strcmp(string, "h")) {
		d->state = d_main_state;
		return false;
	}
	else if (!strcmp(string, "memory") || !strcmp(string, "mem") | !strcmp(string, "m")) {
		if (scanf("%x", &(d->mem_addr))) {
			d->state = d_memory_state;
		}
		return false;
	}
	else if (!strcmp(string, "register") || !strcmp(string, "reg") | !strcmp(string, "r")) {
		if (scanf("%d", &(d->reg_addr))) {
			d->state = d_register_state;
		}
		return false;
	}
	else if (!strcmp(string, "latch") || !strcmp(string, "lat") | !strcmp(string, "l")) {
		d->state = d_latch_state;
		return false;
	}
	else if (!strcmp(string, "bus") || !strcmp(string, "b")) {
		d->state = d_bus_state;
		return false;
	}
	else if (!strcmp(string, "timer") || !strcmp(string, "t")) {
		d->state = d_timer_state;
		return false;
	}
	else if (!strcmp(string, "skip") || !strcmp(string, "s")) {
		//d->state = d_main_state;
		scanf("%d", &(d->skip));
		return true;
	}
	else if (!strcmp(string, "next") || !strcmp(string, "n")) {
		//d->state = d_main_state;
		return true;
	}

	d->state = d_help_state;
	return false;
}
int debugger_print(debugger *d, miniat *m) {

	switch (d->state) {
		case d_main_state:
			debugger_main_print(d, m);
			break;
		case d_memory_state:
			debugger_memory_print(d, m);
			break;
		case d_register_state:
			debugger_register_print(d, m);
			break;
		case d_latch_state:
			debugger_latch_state(d, m);
			break;
		case d_bus_state:
			debugger_bus_state(d, m);
			break;
		case d_timer_state:
			debugger_timer_state(d, m);
			break;
		case d_help_state:
			debugger_help_print(d, m);
			break;
	}
	return EXIT_SUCCESS;
}


int debugger_timer_state(debugger *d, miniat *m) {

	m_timer *t;
	m_timer_id i;

	printf("|==============================================================================|\n");
	printf("|                                MiniAT Debugger                               |\n");
	printf("|==============================================================================|\n");
	printf("\n");

	printf("                 Watchdog     Timer 0     Timer 1     Timer 2     Timer 3       \n");
	printf("\n");

	printf(" Enable          %08x", m->timers.watchdog.control->enable);
	for (i=0; i < m_timer_id_cnt ; i++) {
		t = &(m->timers.general_purpose_timer[i]);
		        printf("    %08x", t->control->enable);
	}
	printf("\n");

        printf(" Mode                     ");
        for (i=0; i < m_timer_id_cnt ; i++) {
                t = &(m->timers.general_purpose_timer[i]);
                        printf("    %08x", t->control->mode);
        }
        printf("\n");

        printf(" Reset                    ");
        for (i=0; i < m_timer_id_cnt ; i++) {
                t = &(m->timers.general_purpose_timer[i]);
                        printf("    %08x", t->control->reset);
        }
        printf("\n");

        printf(" CPS             %08x", m->timers.watchdog.compare->CPS);
        for (i=0; i < m_timer_id_cnt ; i++) {
                t = &(m->timers.general_purpose_timer[i]);
                        printf("    %08x", t->control->mode);
        }
        printf("\n");

        printf(" Count           %08x", m->timers.watchdog.control->count);
        for (i=0; i < m_timer_id_cnt ; i++) {
                t = &(m->timers.general_purpose_timer[i]);
                        printf("    %08x", *(t->count));
        }
        printf("\n");


	printf(" Compare         %08x\n", m->timers.watchdog.compare->value);

        printf(" Wavelength                ");
	for (i=0; i < m_timer_id_cnt ; i++) {
                t = &(m->timers.general_purpose_timer[i]);
                        printf("    %08x", t->compare->wavelength);
        }
        printf("\n");

        printf(" Duty                     ");
        for (i=0; i < m_timer_id_cnt ; i++) {
                t = &(m->timers.general_purpose_timer[i]);
                        printf("    %08x", t->compare->duty);
        }
	printf("\n");

        printf(" Out                      ");
        for (i=0; i < m_timer_id_cnt ; i++) {
                t = &(m->timers.general_purpose_timer[i]);
                        printf("    %08x", t->io->line.output);
        }
	printf("\n");

        printf(" In                       ");
        for (i=0; i < m_timer_id_cnt ; i++) {
                t = &(m->timers.general_purpose_timer[i]);
                        printf("    %08x", t->io->line.input);
        }
        printf("\n");

	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("|==============================================================================|\n");
	printf(": ");
	fflush(stdout);


	return EXIT_SUCCESS;
}

int debugger_bus_state(debugger *d, miniat *m) {

	printf("|==============================================================================|\n");
	printf("|                                MiniAT Debugger                               |\n");
	printf("|==============================================================================|\n");
	printf("\n");

	printf(" State: ");

	if (m->bus.state == m_bus_state_initial) {
		printf("Initial ");
	}
	else if (m->bus.state == m_bus_state_transfer) {
		printf("Transfer");
	}
	else if (m->bus.state == m_bus_state_final) {
		printf("Final   ");
	}

	printf("                Address: %08x                Data: %08x\n", m->bus.pub.address, m->bus.pub.data);
	printf("\n");
	printf(" Req: %08x                    Ack: %08x                  rW: %08x\n", m->bus.pub.req, m->bus.pub.ack, m->bus.pub.rW);
	printf("\n");

	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("|==============================================================================|\n");
	printf(": ");
	fflush(stdout);

	return EXIT_SUCCESS;

}

int debugger_latch_state(debugger *d, miniat *m) {

	printf("|==============================================================================|\n");
	printf("|                                MiniAT Debugger                               |\n");
	printf("|==============================================================================|\n");
	printf("\n");

	printf("                Fetch          Decode          Execute          Writeback       \n");
	printf(" NPC:           %08x       %08x        %08x         %08x                       \n", m->pipeline.fetch.out.NPC,
	                                                                                            m->pipeline.decode.out.NPC,
	                                                                                            m->pipeline.execute.out.NPC,
	                                                                                            d->writeback.NPC);

	printf(" Low Word:      %08x                                                           \n", m->pipeline.fetch.out.data[0]);

	printf(" High Word:     %08x                                                           \n", m->pipeline.fetch.out.data[1]);

	printf(" Hint:                         %08x        %08x         %08x                       \n", m->pipeline.decode.out.hint,
			                                                                                    m->pipeline.execute.out.hint,
			                                                                                    d->writeback.hint);

	printf(" Opcode:                       %08x        %08x         %08x                       \n", m->pipeline.decode.out.opcode,
			                                                                                    m->pipeline.execute.out.opcode,
			                                                                                    d->writeback.opcode);

	printf(" A:                            %08x                                                \n", m->pipeline.decode.out.A);

	printf(" B:                            %08x                                                \n", m->pipeline.decode.out.B);

	printf(" D:                            %08x        %08x         %08x                       \n", m->pipeline.decode.out.D,
			                                                                                    m->pipeline.execute.out.D,
			                                                                                    d->writeback.D);
	printf(" Result:                                   %08x         %08x                       \n", m->pipeline.execute.out.result,
			                                                                                    d->writeback.result);
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("|==============================================================================|\n");
	printf(": ");
	fflush(stdout);

	return EXIT_SUCCESS;
}

int debugger_memory_print(debugger *d, miniat *m) {

	int i, j;
	m_ubyte* bmem;

	printf("|==============================================================================|\n");
	printf("|                                MiniAT Debugger                               |\n");
	printf("|==============================================================================|\n");
	printf("\n");

	bmem = (m_ubyte *) &m->mem.data[d->mem_addr];
	for (i = 0; (i < (8*16)) && (i < 0x10000); i+=8)  {
		printf("         %04x:", (m_uwyde)i + d->mem_addr);

		for (j = i; j < i+16; j+=2) {
			printf(" %02x%02x", bmem[j], bmem[j+1]);
		}
		printf(" ");
		for (j = i; j < i+16; j++) {
			if (bmem[j] > 31 && bmem[j] < 128) {
				printf("%c", bmem[j]);
			}
			else {
				printf(".");
			}
		}
		printf("\n");
	}


	printf("\n");
	printf("|==============================================================================|\n");
	printf(": ");
	fflush(stdout);

	return EXIT_SUCCESS;
}
int debugger_register_print(debugger *d, miniat *m) {

	int i;

	printf("|==============================================================================|\n");
	printf("|                                MiniAT Debugger                               |\n");
	printf("|==============================================================================|\n");
	printf("\n");

	for (i = d->reg_addr; i < (d->reg_addr + 17); i++) {
	printf("                  %03d: %08x                 %03d: %08x\n", i, m->reg[i], i+10, m->reg[i + 10]);
	}

	printf("|==============================================================================|\n");
	printf(": ");
	fflush(stdout);

	return EXIT_SUCCESS;
}
int debugger_help_print(debugger *d, miniat *m) {

	printf("|==============================================================================|\n");
	printf("|                                MiniAT Debugger                               |\n");
	printf("|==============================================================================|\n");
	printf("\n");
	printf("     quit       q            quit the debugger.\n");
	printf("     home       h            home screen.\n");
	printf("     memory     m            memory screen.\n");
	printf("     register   r            register screen.\n");
	printf("     latch      l            latch screen.\n");
	printf("     bus        b            bus screen.\n");
	printf("     timer      t           timer screen.\n");
	printf("     skip       s  [cycles]  skip n number of cycles.\n");
	printf("     next       n            continue to the next cycle.\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("|==============================================================================|\n");
	printf(": ");
	fflush(stdout);

	return EXIT_SUCCESS;
}
int debugger_main_print(debugger *d, miniat *m) {

	int i;

	printf("|==============================================================================|\n");
	printf("|                                MiniAT Debugger                               |\n");
	printf("|==============================================================================|\n");

	printf(" Ticks: %06d                                         FI=%01x FS=%01x IE=%01x SE=%01x U=%01x \n",            m->ticks,
                                                                                                                        m->csr->fi,
                                                                                                                        m->csr->fs,
                                                                                                                        m->csr->ie,
                                                                                                                        m->csr->se,
                                                                                                                        m->csr->u);


	printf("\n");
	printf(" PC 1=%04x 2=%04x 3=%04x                 IER Low: %08x  IER High: %08x\n",                                  m->reg[M_REGISTER_PC_ONE],
                                                                                                                        m->reg[M_REGISTER_PC_TWO],
                                                                                                                        m->reg[M_REGISTER_PC_THREE],
                                                                                                                        *m->interrupts.ier_l,
                                                                                                                        *m->interrupts.ier_h);
	printf("                                         IFR Low: %08x  IFR High: %08x\n",                                  *m->interrupts.ifr_l,
                                                                                                                        *m->interrupts.ifr_h);
	printf("\n");
	printf(" Register Hazards: %03d %03d %03d                  Memory Hazards: %04x %04x %04x\n",                       m->pipeline.register_hazards.queue[0],
                                                                                                                        m->pipeline.register_hazards.queue[1],
                                                                                                                        m->pipeline.register_hazards.queue[2],
                                                                                                                        (m_uwyde)m->pipeline.memory_hazards.queue[0],
                                                                                                                        (m_uwyde)m->pipeline.memory_hazards.queue[1],
                                                                                                                        (m_uwyde)m->pipeline.memory_hazards.queue[2]);
	printf("\n");
	printf(" Registers Changed                Pipeline State                Memory Changed\n");

	for (i = 0; i < 10; i++) {
		debugger_reg_changed(d, m);
		if (d->reg_i < 256) {
			printf(" reg[%03d]: %08x        ", d->reg_i, m->reg[d->reg_i]);
			d->reg_i++;
		}
		else {
			printf("                           ");
		}

		switch (i) {
			case 0:
				printf("      Fetch: ");
				switch (m->pipeline.fetch.state) {
					case m_pipeline_stage_state_no_hazard:
						printf("No Hazard              ");
						break;
					case m_pipeline_stage_state_stalled:
						printf("Stalled                ");
						break;
					case m_pipeline_stage_state_data_hazard:
						printf("Data Hazard            ");
						break;
					case m_pipeline_stage_state_structural_memory_hazard:
						printf("Memory Hazard          ");
						break;
					case m_pipeline_stage_state_structural_peripheral_hazard:
						printf("Peripheral Hazard      ");
						break;
					default:
						break;
				};
				break;
			case 1:
				printf("     Decode: ");
				switch (m->pipeline.decode.state) {
				case m_pipeline_stage_state_no_hazard:
					printf("No Hazard              ");
					break;
				case m_pipeline_stage_state_stalled:
					printf("Stalled                ");
					break;
				case m_pipeline_stage_state_data_hazard:
					printf("Data Hazard            ");
					break;
				case m_pipeline_stage_state_structural_memory_hazard:
					printf("Memory Hazard          ");
					break;
				case m_pipeline_stage_state_structural_peripheral_hazard:
					printf("Peripheral Hazard      ");
					break;
				default:
					break;
				};
				break;
			case 2:
				printf("    Execute: ");
				switch (m->pipeline.execute.state) {
				case m_pipeline_stage_state_no_hazard:
					printf("No Hazard              ");
					break;
				case m_pipeline_stage_state_stalled:
					printf("Stalled                ");
					break;
				case m_pipeline_stage_state_data_hazard:
					printf("Data Hazard            ");
					break;
				case m_pipeline_stage_state_structural_memory_hazard:
					printf("Memory Hazard          ");
					break;
				case m_pipeline_stage_state_structural_peripheral_hazard:
					printf("Peripheral Hazard      ");
					break;
				default:
					break;
				};
				break;
			case 3:
				printf("  Writeback: ");
				switch (m->pipeline.writeback.state) {
				case m_pipeline_stage_state_no_hazard:
					printf("No Hazard              ");
					break;
				case m_pipeline_stage_state_stalled:
					printf("Stalled                ");
					break;
				case m_pipeline_stage_state_data_hazard:
					printf("Data Hazard            ");
					break;
				case m_pipeline_stage_state_structural_memory_hazard:
					printf("Memory Hazard          ");
					break;
				case m_pipeline_stage_state_structural_peripheral_hazard:
					printf("Peripheral Hazard      ");
					break;
				default:
					break;
				};
				break;
			default:
				break;
		}
		debugger_mem_changed(d, m);
		if (d->mem_i < 0x10000) {
			printf("mem[%04x]: %04x\n", d->mem_i, (m_uwyde)m->mem.data[d->mem_i]);
			d->mem_i++;
		}
		else {
			printf("\n");
		}
	}
	debugger_changed_clear(d, m);
	printf("|==============================================================================|\n");
	printf(": ");
	fflush(stdout);
	return EXIT_SUCCESS;
}
