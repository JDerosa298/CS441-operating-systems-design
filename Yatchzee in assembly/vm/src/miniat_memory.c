#include <errno.h>
#include <string.h>
#include <stdbool.h>

#include "miniat/miniat.h"
#include "miniat_memory.h"
#include "miniat/miniat_error.h"
#include "miniat_cache.h"
#include "miniat_defines.h"
#include "miniat_enums.h"
#include "miniat_structures.h"
#include "miniat_typedefs.h"
#include "miniat_unions.h"

/*
 * We include the current cycle in
 * stall times, ie the current cycle
 * makes progress. So a stall of 1
 * happens in the current cycle.
 */
#define M_RAM_STALL_DURATION              2 //!< Number of minimum cycles ram accesses take
#define M_IVT_STALL_DURATION              1 //!< Number of minimum cycles interrupt vector table accesses take
#define M_GPIO_STALL_DURATION             1 //!< Number of minimum cycles gpio accesses take
#define M_CSR_STALL_DURATION              1 //!< Number of minimum cycles control status register accesses take
#define M_ID_STALL_DURATION               2 //!< Number of minimum cycles id accesses take
#define M_FLASH_STALL_DURATION            4 //!< Number of minimum cycles flash accesses take
#define M_PERIPHERAL_STALL_DURATION       1 //!< Number of minimum cycles peripheral accesses take on top of peripheral access time

static m_uword m_get_stall_time(m_memory_type type) {

	switch (type) {
	case m_memory_type_ram:
		return M_RAM_STALL_DURATION;
		break;
	case m_memory_type_ivt:
		return M_IVT_STALL_DURATION;
		break;
	case m_memory_type_gpio:
		return M_GPIO_STALL_DURATION;
		break;
	case m_memory_type_csr:
		return M_CSR_STALL_DURATION;
		break;
	case m_memory_type_id:
		return M_ID_STALL_DURATION;
		break;
	case m_memory_type_flash:
		return M_FLASH_STALL_DURATION;
		break;
	default:
		THROW(EINVAL, "Got invalid memory type: %d", type);
		/* no break */
	}

	/* Impossible keeps compiler happy */
	return ~0;
}

static void _m_memory_write(miniat *m, m_memory_characteristics *mem_info,
		m_uword address, m_uword data) {

	if (mem_info->permissions == m_memory_permissions_write_access
			|| mem_info->permissions == m_memory_permissions_full_access) {

		if (mem_info->type == m_memory_type_flash) {
			m->mem.is_flash_dirty = true;
		}

		m->mem.data[address] = data;
	} else {
		/* writes to invalid addresses go to the bit bucket*/
	}

	return;
}

void m_memory_write(miniat *m, m_uword address, m_uword data) {

	m_memory_characteristics mem_info;

	mem_info = m_memory_address_lookup(address);
	_m_memory_write(m, &mem_info, address, data);

	return;
}

m_uword m_memory_write_or_get_stall_time(miniat *m, m_uword address,
		m_uword data, bool is_ins) {

	m_memory_characteristics mem_info;
	mem_info = m_memory_address_lookup(address);

	m_uword stall = m_get_stall_time(mem_info.type);

	int cache_stall = m_cache_write(m, is_ins, address);

	stall = cache_stall > 0 ? cache_stall : stall;

	/* decrement, this stage counts towards progress */
	stall--;

	if (stall == M_NO_STALL_DURATION) {
		_m_memory_write(m, &mem_info, address, data);
	}

	return stall;
}

static void _m_memory_read(miniat *m, m_memory_characteristics *mem_info,
		m_uword address, m_uword *data) {

	if (mem_info->permissions == m_memory_permissions_read_access
			|| mem_info->permissions == m_memory_permissions_full_access) {

		*data = m->mem.data[address];
	} else {
		/*Reads at invalid addresses return all Fs*/
		*data = ~0;
	}
}

m_uword m_memory_read_or_get_stall_time(miniat *m, m_uword address,
		m_uword *data, bool is_ins) {

	m_memory_characteristics mem_info;
	mem_info = m_memory_address_lookup(address);

	m_uword stall = m_get_stall_time(mem_info.type);

	int cache_stall = m_cache_read(m, is_ins, address);

	stall = cache_stall > 0 ? cache_stall : stall;

	/* decrement, this stage counts towards progress */
	stall--;

	if (stall == M_NO_STALL_DURATION) {
		_m_memory_read(m, &mem_info, address, data);
	}

	return stall;
}

void m_memory_read(miniat *m, m_uword address, m_uword *data) {

	m_memory_characteristics mem_info;
	mem_info = m_memory_address_lookup(address);

	_m_memory_read(m, &mem_info, address, data);
}

m_memory_characteristics m_memory_address_lookup(m_uword address) {

	m_memory_characteristics identifier;

	if (address >= M_RAM_START && address <= M_RAM_END) {
		identifier.type = m_memory_type_ram;
		identifier.permissions = m_memory_permissions_full_access;
	} else if (address >= M_IVT_START && address <= M_IVT_END) {
		if (address == M_IVT_INT_RESET) {
			identifier.permissions = m_memory_permissions_read_access;
		} else {
			identifier.permissions = m_memory_permissions_full_access;
		}
		identifier.type = m_memory_type_ivt;
	} else if (address >= M_GPIO_START && address <= M_GPIO_END) {

		identifier.type = m_memory_type_gpio;
		identifier.permissions = m_memory_permissions_full_access;
	} else if (address >= M_CSR_START && address <= M_CSR_END) {

		identifier.type = m_memory_type_csr;
		identifier.permissions = m_memory_permissions_full_access;
	} else if (address >= M_ID_START && address <= M_ID_END) {

		identifier.type = m_memory_type_id;
		identifier.permissions = m_memory_permissions_read_access;
	} else if (address >= M_FLASH_START && address <= M_FLASH_END) {

		identifier.type = m_memory_type_flash;
		identifier.permissions = m_memory_permissions_full_access;
	} else if (address >= M_PERIPHERAL_START && address <= M_PERIPHERAL_END) {

		identifier.type = m_memory_type_peripheral;
		identifier.permissions = m_memory_permissions_full_access;
	} else {
		identifier.type = m_memory_type_invalid;
		identifier.permissions = m_memory_permissions_no_access;
	}
	return identifier;
}

void m_memory_csr_init(miniat *m) {

	m->csr = (m_csr *) &(m->mem.data[M_CSR_SYSTEM_REGISTER]);
}
