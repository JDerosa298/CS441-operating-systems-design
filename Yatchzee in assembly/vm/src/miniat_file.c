#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <unistd.h>

#include "miniat_file.h"
#include "miniat/miniat.h"
#include "miniat/miniat_error.h"
#include "miniat_defines.h"
#include "miniat_enums.h"
#include "miniat_structures.h"
#include "miniat_typedefs.h"
#include "miniat_unions.h"

#ifndef PRIz
	#ifdef _WIN32
		#define PRIz "u"
	#else
		#define PRIz "zu"
	#endif
#endif

static bool is_big_endian() {

	uint32_t test_word;
	uint8_t *test_byte;

	test_word = 0xFF000000;
	test_byte = (uint8_t *) (&test_word);

	return test_byte[0] == 0xFF;
}

static inline void endian_convert(m_uword *word) {

	uint8_t * byte;
	m_uword temp_word;
	uint8_t * temp_byte;

	byte = (uint8_t *) word;
	temp_word = *word;
	temp_byte = (uint8_t *) &temp_word;

	byte[0] = temp_byte[3];
	byte[1] = temp_byte[2];
	byte[2] = temp_byte[1];
	byte[3] = temp_byte[0];

	return;
}

static void big_endian_convert(miniat *m) {

	m_uword i;

	if (!is_big_endian()) {
		return;
	}

	for (i = M_FLASH_START; i <= M_FLASH_END; i++) {
		m_uword *word = &m->mem.data[i];
		endian_convert(word);
	}

	return;
}

void m_load_file(miniat *m, FILE *file_stream) {

	int error;
	long size;
	m_uword *flash;
	size_t bytes_read;

	m->bin_file = file_stream;

	error = fseek(file_stream, 0, SEEK_END);
	if (error < 0) {
		THROW(errno, "Could not fseek binary file due to error: \"%s\"", strerror(errno));
	}

	/* size is in bytes */
	size = ftell(file_stream);
	if (size != M_FLASH_SIZE_IN_BYTES) {
		THROW(EINVAL, "Invalid binary size, expected %ld got: %"PRIz":", size, M_FLASH_SIZE_IN_BYTES);
	}

	rewind(file_stream);

	flash = &m->mem.data[M_FLASH_START];

	bytes_read = fread(flash, sizeof(*flash), M_FLASH_SIZE, file_stream);
	if (bytes_read != M_FLASH_SIZE) {
		if (feof(file_stream)) {
			THROW(EINVAL, "Unexpected end of file\n");
		}
		else {
			THROW(errno, "Could not read binary file due to error: \"%s\"", strerror(errno));
		}
	}

	/* After reading from disk, we in place convert
	 * from little-endian to big-endian. Flash
	 * is good after this.
	 */
	big_endian_convert(m);

	return;
}

void m_write_file(miniat *m) {

	m_uword *flash;
	size_t bytes_written;

	/*
	 * This may be called on an incomplete initialization, if no
	 * initialization is done, ignore.
	 */
	if (!m->bin_file) {
		return;
	}

	if (!m->mem.is_flash_dirty) {
		goto close_file;
	}

	rewind(m->bin_file);

	/* before writing to disk, we in place convert
	 * from big-ending to little-endian. Flash
	 * is hosed after this.
	 */
	big_endian_convert(m);

	flash = &m->mem.data[M_FLASH_START];

	bytes_written = fwrite(flash, sizeof(*flash), M_FLASH_SIZE, m->bin_file);
	if (bytes_written != M_FLASH_SIZE) {
		THROW(errno, "Could not write binary file due to error: \"%s\"", strerror(errno));
	}

	close_file: fclose(m->bin_file);
	m->bin_file = NULL;

	return;
}
