#include <errno.h>
#include <string.h>

#include "miniat/miniat.h"
#include "miniat_pc.h"
#include "miniat/miniat_error.h"
#include "miniat_defines.h"
#include "miniat_enums.h"
#include "miniat_structures.h"
#include "miniat_typedefs.h"
#include "miniat_unions.h"


void m_pc_init(miniat *m) {

	m->pc.queue = &(m->reg[M_REGISTER_PC]);
	m->pc.size = 0;
}

void m_pc_add_last(miniat *m, m_uword address) {

	m_uword *pc_queue = m->pc.queue;
	m_ubyte *size = &(m->pc.size);

	if (*size >= M_PC_QUEUE_SIZE) {
		THROW(ERANGE, "PC Queue is full");
	}

	pc_queue[*size] = address;
	(*size)++;
}

m_uword m_pc_peek(miniat *m) {
	return m->pc.queue[0];
}

m_uword m_pc_remove_first(miniat *m) {

	m_uword *pc_queue = m->pc.queue;
	m_ubyte *size = &(m->pc.size);

	if (*size == 0) {
		THROW(ERANGE, "PC Queue is empty");

	}

	/*Grab the current head of the queue*/
	m_uword address = pc_queue[0];

	/*Decrement the size of the PCQ, since an element has been removed */
	(*size)--;

	/*
	 * Since the head has been removed, slide the data up in the queue
	 * towards the head
	 */
	pc_queue[0] = pc_queue[1];
	pc_queue[1] = pc_queue[2];
	pc_queue[2] = 0;
	return address;
}

void m_pc_clear(miniat *m) {

	memset(m->pc.queue, 0, M_PC_QUEUE_SIZE);
	m->pc.size = 0;
}
