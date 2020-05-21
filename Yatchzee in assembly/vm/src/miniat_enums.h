#ifndef _MINIAT_PRIV_ENUMERATIONS_H_
#define _MINIAT_PRIV_ENUMERATIONS_H_

#include "miniat/miniat.h"
#include "miniat_defines.h"

#include "miniat_typedefs.h"

/**
 * Used for indicating which stage of the pipe
 * line your referring too.
 */
enum m_pipeline_stage {

	m_pipeline_stage_fetch = 0, //!< Fetch stage of the pipeline
	m_pipeline_stage_decode,   //!< Decode stage of the pipeline
	m_pipeline_stage_execute,  //!< Execute stage of the pipeline
	m_pipeline_stage_writeback,  //!< Writeback stage of the pipeline
	/*
	 * The size variable must go last
	 */
	m_pipeline_stage_size      //!< Number of pipeline stages
};

/**
 * Indicates the current state of a stage
 * in the pipeline. Their are two classes of stalls.
 * Active stalls are ones in which the stage marking the stall
 * has encountered the stalling condition.
 * Passive stalls are caused by previous stages being stalled due to
 * a forward stage actively stalling.
 */
enum m_pipeline_stage_state {

	m_pipeline_stage_state_no_hazard = 0, //!< No hazards, pipeline state executes normally
	m_pipeline_stage_state_stalled, //!< The pipeline stage is passively being stalled.
	m_pipeline_stage_state_data_hazard, //!< The pipeline stage is actively stalled waiting on a data hazard to be cleared.
	m_pipeline_stage_state_structural_memory_hazard, //!< The pipeline stage is actively stalled waiting on a memory hazard to be cleared.
	m_pipeline_stage_state_structural_peripheral_hazard, //!< The pipeline stage is actively stalled waiting on a peripheral hazard to be cleared.
	/*
	 * The size variable must go last
	 */
	m_pipeline_stage_state_size                //!< Number of hazard conditions.
};

/**
 * The type of memory. Typically used to tag memory with a type
 * so permissions on that memory can be determined.
 */
enum m_memory_type {
	m_memory_type_ram = 0,   //!< The memory is within ram
	m_memory_type_ivt,      //!< The memory is within the interrupt vector table
	m_memory_type_gpio,      //!< The memory is within gpio
	m_memory_type_csr,    //!< The memory is within the control status registers
	m_memory_type_id,        //!< The memory is within the id section
	m_memory_type_flash,     //!< The memory is within flash
	m_memory_type_peripheral, //!< The memory is within peripheral address space
	m_memory_type_invalid,   //!< The memory is invalid
	/*
	 * The size variable must go last
	 */
	m_memory_type_size       //!< Number of memory types
};

/**
 * Tags memory types with permissions.
 */
enum m_memory_permissions {
	m_memory_permissions_no_access = 0,    //!< No access allowed on that memory
	m_memory_permissions_read_access,  //!< Memory is read only
	m_memory_permissions_write_access, //!< Memory is write only
	m_memory_permissions_full_access,  //!< Memory is both read and write
	/*
	 * the size variable must go last
	 */
	m_memory_permissions_size          //!< Number of permissions
};

/**
 * The different modes the general purpose timers can be in.
 */
enum m_timer_modes {

	m_timer_time = 0,          //!< The timing mode
	m_timer_count,   //!< The counting mode
	m_timer_toggle,  //!< The toggle mode
	m_timer_pwm,     //!< The Pulse Width Modulation (PWM) mode
	/*
	 * The size variable must go last
	 */
	m_timer_mode_size     //!< Number of timer modes
};

/**
 * These are all the known valid types of blocks that can be specified in the binary file.
 * TODO: Are these used?
 */
enum m_block_type {
	m_block_type_ram = 0,   //!< Block is for ram
	m_block_type_flash,     //!< Block is for flash
	m_block_type_pipeline,  //!< Block is for pipeline
	m_block_type_peripheral,  //!< Block is for peripheral
	m_block_type_state,     //!< Block is for state
	/*
	 * This must go last
	 */
	m_block_type_cnt,       //!< m_block_type_cnt
};

/**
 * Different states the bus can be in.
 */
enum m_bus_state {
	m_bus_state_initial = 0,       //!< Initial state
	m_bus_state_transfer,   //!< The state during the data transfer
	m_bus_state_final       //!< The final state
};

#endif
