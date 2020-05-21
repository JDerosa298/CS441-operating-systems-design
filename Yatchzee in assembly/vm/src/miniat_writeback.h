#ifndef _MINIAT_PRIV_WRITEBACK_H_
#define _MINIAT_PRIV_WRITEBACK_H_

#include <stdbool.h>

#include "miniat/miniat.h"

/**
 * Cause the writeback stage to execute
 * @param m
 *  The miniat whose writeback stage needs execution
 */
extern void m_writeback_stage(miniat *m);

#endif
