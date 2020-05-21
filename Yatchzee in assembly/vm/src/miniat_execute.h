#ifndef _MINIAT_PRIV_EXECUTE_H_
#define _MINIAT_PRIV_EXECUTE_H_

#include <stdbool.h>

#include "miniat/miniat.h"

/**
 * Causes the execute stage of the miniat to run.
 * @param m
 *  The miniat whose execute stage to run
 */
void m_execute_stage(miniat *m);

#endif

