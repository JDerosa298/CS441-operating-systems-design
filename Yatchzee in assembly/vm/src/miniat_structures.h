#ifndef _MINIAT_PRIV_STRUCTURES_H_
#define _MINIAT_PRIV_STRUCTURES_H_

#include <stdio.h>
#include <time.h>

#include "lua.h"

#include "miniat/miniat.h"
#include "miniat_defines.h"
#include "miniat_enums.h"

#include "miniat_typedefs.h"
#include "miniat_unions.h"

/**
 * A stage can be in a particular m_pipeline_stage_state, which is set in the state variable. e.g. a data hazard.
 * Some states last for a particular duration, such as an access to RAM or FLASH (structural hazard), these timed
 * events place how many ticks it takes for the hazard to clear in the duration variable.
 */
struct m_pipeline_memory_hazards {
	m_ubyte size;         //!< Current count of slots filled in the hazard queue
	m_uword queue[M_PIPELINE_SIZE - 1];                    //!< The queue itself
};

/**
 * Used to mark the registers that are pending a write. The decode stage adds to this queue
 * and when the write completes the writeback removes it.
 */
struct m_pipeline_register_hazards {
	m_ubyte size;         //!< Current count of slots filled in the hazard queue
	m_uword queue[M_PIPELINE_SIZE - 1];                    //!< The queue itself
};

/**
 * Fetch stages output latch.
 */
struct m_pipeline_fetch_latch_out {

	m_uword PC;                   //!< This instruction's address... for tracing
	m_uword NPC;                            //!< Next Program Counter
	m_uword data[M_INSTRUCTION_SIZE_WORDS]; //!< The instruction fetched from memory
};

/**
 * This structure wraps up the elements that must be tracked for
 * the pipeline stage fetch.
 */
struct m_pipeline_fetch {

	m_pipeline_fetch_latch_out out; //!< The output from its execution
	m_pipeline_stage_state state;   //!< The state of the stage, such as stalled
	m_uword duration;               //TODO is this used?
};

/**
 * The output latch to the decode stage. After the decode executes it's results
 * are stored in this structure.
 */
struct m_pipeline_decode_latch_out {

	m_uword PC;     //!< This instruction's address... for tracing
	m_uword NPC;    //!< Next program counter
	m_ubyte hint;   //!< Hint bit from the instruction string
	m_uword opcode; //!< Opcode from the instruction string
	m_uword A;      //!< The value stored in register A (A = reg(rA))
	m_uword B;      //!< The value stored in register B (B = reg(rB))
	m_uword D;      //!< The results from computing reg(rC) + Immediate
	m_ubyte rX; //!< The writeback register, used to index into the register file on writes
};

/**
 * Wraps up all the data needed for the decode stage
 */
struct m_pipeline_decode {

	m_pipeline_decode_latch_in in;   //!< The input from the fetch stage
	m_pipeline_decode_latch_out out; //!< The output from executing this stage
	m_pipeline_stage_state state;   //!< The state of the stage, such as stalled
	m_uword duration;                //TODO: Do we need this
};

/**
 * Output from executing the output stage is stored in this structure
 */
struct m_pipeline_execute_latch_out {

	m_uword PC;     //!< This instruction's address... for tracing
	m_uword NPC;    //!< The Next Program Counter (NPC)
	m_ubyte hint;   //!< The hint bit from the instruction string
	m_uword opcode; //!< The opcode from the instruction string
	m_uword D;      //!< The results of reg(rC) + I
	m_uword result; //!< The result of the operation (such as ADD)
	m_ubyte rX;     //!< The writeback destination for registers
};

/**
 * Wraps up all the data needed for the execute stage
 */
struct m_pipeline_execute {

	m_pipeline_execute_latch_in in;   //!< The input latch from the decode stage
	m_pipeline_execute_latch_out out; //!< The output latch, results are stored here
	m_pipeline_stage_state state;  //!< The state of this stage, such as stalled
	m_uword duration; //!< The duration this stage needs to be stalled for, ie for accesses to memory
};

/**
 * Wraps up all the data needed for the writeback stage
 */
struct m_pipeline_writeback {

	m_pipeline_writeback_latch_in in; //!< The input from the execute stage
	m_pipeline_stage_state state;  //!< The state of this stage, such as stalled
	m_uword duration; //!< The duration this stage needs to be stalled for, ie for accesses to memory
};

/**
 * Represents a MiniAT pipeline
 */
struct m_pipeline {

	m_pipeline_fetch fetch;                       //!< The fetch stage
	m_pipeline_decode decode;                     //!< The decode stage
	m_pipeline_execute execute;                   //!< The execute stage
	m_pipeline_writeback writeback;               //!< The writeback stage
	m_pipeline_register_hazards register_hazards; //!< The register hazard queue
	m_pipeline_memory_hazards memory_hazards;     //!< The memory hazard queue
	bool is_interrupt_return; //!< This indicates whether the pipeline was just restored from the IVBT.
};

/**
 * General purpose timer structure
 */
struct m_timer {
	m_timer_compare *compare; //!< Pointer back into memory to the compare mem/reg
	m_timer_control *control; //!< Pointer back into memory to the control mem/reg
	m_uword *count;           //!< Pointer back into memory to the count mem/reg
	m_timers_io *io;          //!< Pointer back into memory to the io mem/reg
	m_uword internal_count;   //!< Internal count of the modulus counter
	m_ubyte old_in; //!< State of the last cycle's timers input pin, used for detecting rising transitions
	m_uword low_length; //!< Used by PWM for how many cycles to have the output low for
	m_uword high_length; //!< Used by PWM for how many cycles to have the output high for
};

/**
 * Represents the watchdog timer
 */
struct m_watchdog {
	m_watchdog_compare *compare; //!< Pointer back into memory for the compare mem/reg
	m_watchdog_control *control; //!< Pointer back into memory for the control mem/reg
	m_uword internal_count;      //!< Internal count of the modulus counter
};

/**
 * Wraps all the timers into this struct.
 */
struct m_timers {
	m_watchdog watchdog;                               //!< Watchdog timer
	m_timer general_purpose_timer[m_timer_id_cnt]; //!< General purpose timers
};

/**
 * The program counter, which is a queue.
 */
struct m_pc {

	m_ubyte size;                  //!< The current number of items in the queue
	/* Union'ing this onto itself made the code much easier to write */
	union {
		m_uword *queue;                         //!< Base pointer to the queue
		m_uword ivbt_queue[M_PC_QUEUE_SIZE]; //!< Other fields of space to the queue
	};
};

/**
 * This structure hides the state variable from public accessible bus structure
 * as well as keeps two separate versions of the bus. The "priv" version of the
 * bus is for internal use by the MiniAT. The pub version is what is provided by a
 * miniat_conector_bus_get or is the one set by miniat_pins_bus_set/get.
 * each clock cycle should then copy the public structure fields to the private version.
 * This prevents peripherals from driving master only control lines.
 *
 * Just tracking the bits of req and ack is not enough for determining if your in the
 * initial state or just finished waiting on the peripheral to bring it's ack line low.
 * Ie initial state versus final state of the bus both have the req and ack low...
 */
struct m_priv_bus {

	m_bus_state state; //!< Used to distinguish the initial state from the final state
	m_uword storage; //!< Used to store the data from when it was read from the bus
	m_uword strobe_duration; //!< Used to maintain the strobe duration in cycles, prevents bus hangs
	m_bus priv; //!< Internal copy of the miniat bus for use by the bus mechanism
	m_bus pub;               //!< Public copy provided by bus connector

};

struct m_ivbt_pc {
	m_ubyte size;     //!< The current number of items in the backed up pc queue
	m_uword queue[M_PC_QUEUE_SIZE]; //!< The backed up pc queue itself.
};

/**
 * The interrupt back up table. Used as a private scratch-pad for managing interrupt
 * execution internally to the microcontroller. On interrupt context switches, the
 * state of the pipeline stages are backed up into the IVBT on a per interrupt bases.
 */
struct m_ivbt {

	m_pc pc;
	m_pipeline_register_hazards register_hazards;
	m_pipeline_memory_hazards memory_hazards;
	m_pipeline_decode decode;
	m_pipeline_execute execute;
	m_pipeline_writeback writeback;

};

/**
 * Used to contain the characteristics of memory that you are checking.
 */
struct m_memory_characteristics {
	m_memory_type type;               //!< Type of memory, such as ram
	m_memory_permissions permissions; //!< Permissions associated with that memory, such as read only
};

/**
 * This structure wraps up all the things required for the interrupt subsystem
 */
struct m_interrupts {
	m_uword *sys_reg;             //!< A pointer to the system register
	m_uword *ier_l; //!< Currently enabled interrupts register low, this is a pointer back into mem
	m_uword *ier_h; //!< Currently enabled interrupts register high, this is a pointer back into mem
	m_uword *ifr_l; //!< Currently flagged interrupts for execution register low, this is a pointer back into mem
	m_uword *ifr_h; //!< Currently flagged interrupts for execution register high, this is a pointer back into mem
	m_uword iqr_l;  //!< Currently queued interrupts for execution register high
	m_uword iqr_h;  //!< Currently queued interrupts for execution register high
	m_uword irr_l;                //!< Currently running interrupt register low
	m_uword irr_h;                //!< Currently running interrupt register high
	m_uword oifr_l;        //!< The previous cycles interrupt flags register low
	m_uword oifr_h;       //!< The previous cycles interrupt flags register high
	m_ivbt ivbt[M_IVBT_SIZE]; //!< The interrupt back up table
};

/**
 * This is the structure to hold each block to be read or written to
 */
struct m_file_block {
	m_uword number;	       //!< number from 0 - n(number of blocks)
	m_uword id;	           //!< any element of the m_block_type enum
	m_uword start_address; //!< address that the blocks data should be written to in the miniat's address space
	m_uword length;       //!< number of uwords in the data section of the block
	m_uword *data;         //!< the actual data of the block

};

struct m_memory {
	m_uword data[M_MEMORY_SIZE]; //!< Memory
	bool is_flash_dirty;           //!< True if flash was ever written.
};

struct m_cache {
	lua_State *state;
};

/**
 * Introducing the MiniAT structure. This contains all the structure above and represents
 * the micro-controller itself.m
 */
struct miniat {

	m_memory mem;
	m_uword reg[M_NUM_REGISTERS + M_PC_QUEUE_SIZE - 1]; //!< Register file, Note that the number of registers and PC queue size overlap by 1, since r255 is shared */
	m_pc pc;                 //!< Program counter queue
	m_interrupts interrupts; //!< Interrupt subsystem
	m_timers timers;         //!< Timer subsystem
	m_pipeline pipeline;     //!< The pipeline state
	m_priv_bus bus;          //!< The bus state
	m_csr *csr;              //!< The csr's. This is a pointer back into mem
	m_cache cache;
	m_uword ticks;           //!< Current tick of the miniat
	FILE *bin_file;          //!< Current loaded file
};

#endif
