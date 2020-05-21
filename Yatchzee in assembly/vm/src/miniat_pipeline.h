#ifndef _MINIAT_PRIV_PIPELINE_H_
#define _MINIAT_PRIV_PIPELINE_H_

#include "miniat/miniat.h"
#include "miniat_typedefs.h"

/**
 * Intializes the pipeline
 * @param m
 *  The miniat whose pipeline to initialzie
 */
extern void m_pipeline_init(miniat *m);

/**
 * Provide a "clock" to the pipeline, ie cuase the pipeline to try and make
 * forward progress.
 * @param m
 *  The miniat whose pipeline to provide the clock too
 */
extern void m_pipeline_clock(miniat *m);

/**
 * Set a stall on a particular stage of the pipeline. This causes all previous stages
 * to become in a passive stall state.
 * @param m
 *  The miniat whose pipeline to stall
 * @param stage
 *  The stage starting the stall
 * @param type
 *  The type of the stall, such as structural memory hazard
 * @param duration
 *  The duration of the stall, used for memory accesses
 */
extern void m_pipeline_stalls_set(miniat *m, m_pipeline_stage stage,
		m_pipeline_stage_state type, m_uword duration);

/**
 * Clear the stall on a stage in the pipeline and causes all stalls before it to be cleared as
 * well.
 * @param m
 *  The miniat whose stall to clear
 * @param stage
 *  The stage to clear the hazard on.
 */
extern void m_pipeline_stalls_clear(miniat *m, m_pipeline_stage stage);

/**
 * Clear ALL the stalls on the pipeline
 * @param m
 *  The miniat whose pipeline stalls to clear
 */
extern void m_pipeline_stalls_clear_all(miniat *m);

/**
 * Set a particular stages latch to that of a No Operation (NOP)
 * @param m
 *  The miniat whose latch to set
 * @param stage
 *  The stages output latch to set
 */
extern void m_pipeline_set_latch_to_NOP(miniat *m, m_pipeline_stage stage);

/**
 * Morph a stages output latch to that of an interrupt intruction. Used for handling
 * internal pipeline interrupts.
 * @param m
 *  The miniat whose latch to morph
 * @param stage
 *  The stage to morph
 * @param interrupt
 *  The interrupt to morph the latch too.
 */
extern void m_pipeline_morph_to_INT(miniat *m, m_pipeline_stage stage,
		m_uword interrupt);

/**
 * Flush the pipeline
 * @param m
 *  The miniat whose pipeline to flush
 */
extern void m_pipeline_flush(miniat *m);

#endif
