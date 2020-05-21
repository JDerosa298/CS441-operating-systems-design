#ifndef _MINIAT_PRIV_FILE_H_
#define _MINIAT_PRIV_FILE_H_

#include <stdio.h>
#include <stdbool.h>

#include "miniat/miniat.h"
#include "miniat_defines.h"
#include "miniat_enums.h"

#include "miniat_structures.h"
#include "miniat_typedefs.h"
#include "miniat_unions.h"

/**
 * Initialize the contents of flash from the contents of a file.
 * @param m
 *  The miniat whose flash contents to initialize.
 * @param file_stream
 *  The file whose contents are used to populate flash.
 */
void m_load_file(miniat *m, FILE *file_stream);

/**
 * Write the contents of flash to the file used in m_load_file(), and closes
 * the open file.
 * @param m
 *  The miniat whose flash contents to save.
 *
 * @warning
 * On Big-endian systems, the contents of flash will be converted to little
 * endian format.
 */
void m_write_file(miniat *m);

#endif /* _MINIAT_PRIV_FILE_H_ */

