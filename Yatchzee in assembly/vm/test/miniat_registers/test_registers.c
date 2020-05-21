#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../src/miniat_defines.h"
#include "../../src/miniat_pc.h"
#include "../../src/miniat_registers.h"
#include "../../src/miniat_structures.h"
#include "../../src/miniat_typedefs.h"
#include "miniat/miniat.h"

#include "CuTest.h"

void t_registers___read(CuTest *tc) {

	miniat m;
	m_ubyte address;
	m_uword data;

	memset(&m, 0, sizeof(miniat));

	m.reg[100] = 0xDEADC0DE;

	address = 100;
	data = 0;
	m_registers_read(&m, address, &data);

	CuAssert(tc, "Register 100 equals 0xDEADC0DE", m.reg[100] == data);

	m.reg[254] = 0xDEADC0DE;

	address = 254;
	data = 0;
	m_registers_read(&m, address, &data);

	CuAssert(tc, "Register 254 equals 0xDEADC0DE", m.reg[254] == data);

	m.reg[1] = 0xDEADC0DE;

	address = 1;
	data = 0;
	m_registers_read(&m, address, &data);

	CuAssert(tc, "Register 1 equals 0xDEADC0DE", m.reg[1] == data);

	return;
}
void t_registers___write(CuTest *tc) {

	miniat m;
	m_ubyte address;
	m_uword data;

	memset(&m, 0, sizeof(miniat));

	address = 100;
	data = 0xDEADC0DE;
	m_registers_write(&m, address, data);

	CuAssert(tc, "Register 100 equals 0xDEADC0DE", m.reg[100] == data);

	address = 254;
	data = 0xDEADC0DE;
	m_registers_write(&m, address, data);

	CuAssert(tc, "Register 254 equals 0xDEADC0DE", m.reg[254] == data);

	address = 1;
	data = 0xDEADC0DE;
	m_registers_write(&m, address, data);

	CuAssert(tc, "Register 1 equals 0xDEADC0DE", m.reg[1] == data);

	address = M_REGISTER_ZERO;
	data = 0xDEADC0DE;
	m_registers_write(&m, address, data);

	CuAssert(tc, "Register 0 equals 0", m.reg[M_REGISTER_ZERO] == 0);

	return;
}

void t_registers___pc(CuTest *tc) {

	miniat m;
	m_ubyte address;
	m_uword data;

	memset(&m, 0, sizeof(miniat));

	m_pc_init(&m);

	address = M_REGISTER_PC;
	data = 1;
	m_registers_write(&m, address, data);

	CuAssert(tc, "Register M_REGISTER_PC_ONE equals 1", m.reg[M_REGISTER_PC_ONE] == data);

	address = M_REGISTER_PC;
	data = 2;
	m_registers_write(&m, address, data);

	CuAssert(tc, "Register M_REGISTER_PC_TWO equals 2", m.reg[M_REGISTER_PC_TWO] == data);

	address = M_REGISTER_PC;
	data = 3;
	m_registers_write(&m, address, data);

	CuAssert(tc, "Register M_REGISTER_PC_THREE equals 3", m.reg[M_REGISTER_PC_THREE] == data);

	return;

}

