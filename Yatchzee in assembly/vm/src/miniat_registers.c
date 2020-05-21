#include "miniat_registers.h"

#include "miniat/miniat.h"
#include "miniat/miniat_error.h"
#include "miniat_defines.h"
#include "miniat_enums.h"
#include "miniat_pc.h"
#include "miniat_pipeline.h"
#include "miniat_structures.h"
#include "miniat_typedefs.h"
#include "miniat_unions.h"

void m_registers_read(miniat *m, m_ubyte address, m_uword *data) {

	*data = m->reg[address];
}

void m_registers_write(miniat *m, m_ubyte address, m_uword data) {

	if (address == M_REGISTER_PC) {
		m_pc_add_last(m, data);
	}
	/* writes to register 0 go to the bit bucket */
	else if (address != M_REGISTER_ZERO) {
		m->reg[address] = data;
	}
}
