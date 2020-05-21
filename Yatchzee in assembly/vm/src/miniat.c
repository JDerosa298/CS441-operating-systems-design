#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "miniat/miniat.h"

#include "miniat/miniat_error.h"
#include "miniat_bus.h"
#include "miniat_cache.h"
#include "miniat_defines.h"
#include "miniat_dso.h"
#include "miniat_enums.h"
#include "miniat_file.h"
#include "miniat_interrupts.h"
#include "miniat_memory.h"
#include "miniat_pc.h"
#include "miniat_pipeline.h"
#include "miniat_structures.h"
#include "miniat_timers.h"
#include "miniat_typedefs.h"
#include "miniat_unions.h"

PUBLIC miniat *miniat_file_new(const char *bin_file_path,
		const char *lua_cache_file_path) {

	/* If bin_file_path is NULL, valgrind reports:
	 * Syscall param open(filename) points to unaddressable byte(s)
	 *
	 * So check for NULL upfront.
	 */
	if (!bin_file_path) {
		THROW(EINVAL, "bin_file_path cannot be NULL");
	}

	FILE *f = fopen(bin_file_path, "r+b");
	return miniat_new(f, lua_cache_file_path);
}

PUBLIC miniat *miniat_new(FILE *binary_file, const char *lua_cache_file_path) {

	miniat *m = (miniat *) calloc(sizeof(miniat), 1);
	if (!m) {
		THROW(errno, "MiniAT allocation failure, error: \"%s\"", strerror(errno));
	}

	TRY {
		m_interrupts_init(m);
		m_pipeline_init(m);
		m_pc_init(m);
		m_timers_init(m);
		m_memory_csr_init(m);
		m_load_file(m, binary_file);
		m_cache_init(m, lua_cache_file_path);
		m_pc_add_last(m, M_FLASH_START);
	}
	CATCHALL {
		miniat_dump_error(m);
		miniat_free(m);
		CTHROW(EIO, NULL);
	}
	FINALLY {}
	ETRY;

	return m;
}

PUBLIC void miniat_free(miniat *m) {

	if (!m) {
		THROW(EINVAL, "MiniAT cannot be NULL");
	}

	m_write_file(m);
	m_cache_destroy(m);
	free(m);
}

PUBLIC void miniat_clock(miniat *m) {

	if (!m) {
		THROW(EINVAL, "MiniAT cannot be NULL");
	}

	TRY	{
		m_timers_clock(m);
		m_interrupts_handle_interrupt(m);
		m_bus_shuffle(m);
		m_pipeline_clock(m);
		m_bus_shuffle(m);
		m_interrupts_handle_interrupt(m);

		m->interrupts.oifr_l = *(m->interrupts.ifr_l);
		m->interrupts.oifr_h = *(m->interrupts.ifr_h);
		m->ticks++;
	}
	CATCHALL {
		miniat_panic(_e_);
	}
	FINALLY {}
	ETRY;
}

PUBLIC void miniat_reset(miniat *m) {

	m_uword flash[M_FLASH_SIZE];

	if (!m) {
		THROW(EINVAL, "MiniAT cannot be NULL");
	}

	TRY {
		m_uword ticks = m->ticks;

		memcpy(flash, &(m->mem.data[M_FLASH_START]), sizeof(m_uword) * M_FLASH_SIZE);

		memset(m, 0, sizeof(miniat));

		m_interrupts_init(m);
		m_pipeline_init(m);
		m_pc_init(m);
		m_timers_init(m);
		m_memory_csr_init(m);
		m_pc_add_last(m, M_FLASH_START);

		m->ticks = ticks;
		memcpy(&(m->mem.data[M_FLASH_START]), flash, sizeof(m_uword) * M_FLASH_SIZE);
	}
	CATCHALL {
		miniat_panic(_e_);
	}
	FINALLY {}
	ETRY;
}
