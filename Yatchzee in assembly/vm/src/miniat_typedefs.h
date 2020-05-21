#ifndef _MINIAT_PRIV_TYPEDEFS_H_
#define _MINIAT_PRIV_TYPEDEFS_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct m_pc m_pc;
typedef struct m_ivbt_pc m_ivbt_pc;
typedef struct m_interrupts m_interrupts;
typedef struct m_timer m_timer;
typedef struct m_pipeline m_pipeline;

typedef union m_csr m_csr;

typedef struct m_ivbt m_ivbt;

typedef struct m_priv_bus m_priv_bus;
typedef enum m_bus_state m_bus_state;

typedef enum m_timer_modes m_timer_modes;
typedef union m_timer_compare m_timer_compare;
typedef union m_timer_control m_timer_control;

typedef struct m_timers m_timers;

typedef struct m_watchdog m_watchdog;
typedef union m_watchdog_control m_watchdog_control;
typedef union m_watchdog_compare m_watchdog_compare;

typedef enum m_pipeline_stage m_pipeline_stage;
typedef enum m_pipeline_stage_state m_pipeline_stage_state;

typedef struct m_pipeline_fetch m_pipeline_fetch;
typedef struct m_pipeline_fetch_latch_out m_pipeline_fetch_latch_out;

typedef struct m_pipeline_decode m_pipeline_decode;
typedef struct m_pipeline_fetch_latch_out m_pipeline_decode_latch_in;
typedef struct m_pipeline_decode_latch_out m_pipeline_decode_latch_out;

typedef struct m_pipeline_execute m_pipeline_execute;
typedef struct m_pipeline_decode_latch_out m_pipeline_execute_latch_in;
typedef struct m_pipeline_execute_latch_out m_pipeline_execute_latch_out;

typedef struct m_pipeline_writeback m_pipeline_writeback;
typedef struct m_pipeline_execute_latch_out m_pipeline_writeback_latch_in;

typedef struct m_pipeline_register_hazards m_pipeline_register_hazards;
typedef struct m_pipeline_memory_hazards m_pipeline_memory_hazards;

typedef struct m_memory_characteristics m_memory_characteristics;
typedef enum m_memory_type m_memory_type;
typedef enum m_memory_permissions m_memory_permissions;

typedef struct m_memory m_memory;

typedef enum m_block_type m_block_type;
typedef struct m_file_block m_file_block;

typedef union m_low_word_layout m_low_word_layout;

typedef struct m_cache m_cache;

#endif
