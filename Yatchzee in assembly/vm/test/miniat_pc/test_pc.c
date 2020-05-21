#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../src/miniat_defines.h"
#include "../../src/miniat_pc.h"
#include "../../src/miniat_structures.h"
#include "../../src/miniat_typedefs.h"
#include "miniat/miniat.h"

#include "CuTest.h"


void t_pc___remove_first(CuTest *tc) {

	miniat m;
	m_uword data;

	memset(&m, 0, sizeof(miniat));

	m_pc_init(&m);

	data = 1;
	m_pc_add_last(&m, data);
	
	data = 2;
	m_pc_add_last(&m, data);

	data = 3;
	m_pc_add_last(&m, data);


	CuAssert(tc, "PC queue position 1 equals 1", m.reg[M_REGISTER_PC_ONE] == 1);
	CuAssert(tc, "PC queue position 2 equals 2", m.reg[M_REGISTER_PC_TWO] == 2);
	CuAssert(tc, "PC queue position 3 equals 3", m.reg[M_REGISTER_PC_THREE] == 3);
	CuAssert(tc, "PC queue size equals 3", m.pc.size == 3);

	m_pc_remove_first(&m);

	CuAssert(tc, "PC queue position 1 equals 1", m.reg[M_REGISTER_PC_ONE] == 2);
	CuAssert(tc, "PC queue position 2 equals 2", m.reg[M_REGISTER_PC_TWO] == 3);
	CuAssert(tc, "PC queue size equals 2", m.pc.size == 2);

	m_pc_remove_first(&m);

	CuAssert(tc, "PC queue position 1 equals 1", m.reg[M_REGISTER_PC_ONE] == 3);
	CuAssert(tc, "PC queue size equals 1", m.pc.size == 1);

	m_pc_remove_first(&m);

	CuAssert(tc, "PC queue size equals 0", m.pc.size == 0);

	return;
}

void t_pc___add_last(CuTest *tc) {

	miniat m;
	m_uword data;

	memset(&m, 0, sizeof(miniat));

	m_pc_init(&m);

	data = 1;
	m_pc_add_last(&m, data);

	CuAssert(tc, "PC queue 1 equals 1", m.reg[M_REGISTER_PC_ONE] == 1);
	CuAssert(tc, "PC queue size equals 1", m.pc.size == 1);

	data = 2;
	m_pc_add_last(&m, data);

	CuAssert(tc, "PC queue position 1 equals 1", m.reg[M_REGISTER_PC_ONE] == 1);
	CuAssert(tc, "PC queue position 2 equals 2", m.reg[M_REGISTER_PC_TWO] == 2);
	CuAssert(tc, "PC queue size equals 2", m.pc.size == 2);

	data = 3;
	m_pc_add_last(&m, data);

	CuAssert(tc, "PC queue position 1 equals 1", m.reg[M_REGISTER_PC_ONE] == 1);
	CuAssert(tc, "PC queue position 2 equals 2", m.reg[M_REGISTER_PC_TWO] == 2);
	CuAssert(tc, "PC queue position 3 equals 3", m.reg[M_REGISTER_PC_THREE] == 3);
	CuAssert(tc, "PC queue size equals 2", m.pc.size == 3);

	return;
}
void t_pc___clear(CuTest *tc) {

	miniat m;
	m_uword data;

	memset(&m, 0, sizeof(miniat));

	m_pc_init(&m);

	data = 1;
	m_pc_add_last(&m, data);
	
	data = 2;
	m_pc_add_last(&m, data);

	data = 3;
	m_pc_add_last(&m, data);

	m_pc_clear(&m);
	
	CuAssert(tc, "PC queue size equals 0", m.pc.size == 0);

	return;
}
