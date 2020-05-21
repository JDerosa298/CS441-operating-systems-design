#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <miniat/miniat.h>
#include "simple_term.h"

struct p_simple_term {
	int connected;
	m_uword address;
	m_bus *bus;
};

p_simple_term *p_simple_term_new(m_uword address) {

	p_simple_term *t = malloc(sizeof(p_simple_term));
	if (t) {
		t->bus = (m_bus *)malloc(sizeof(m_bus));
		if (!t->bus) {
			free(t);
			t = NULL;
		}
		else {
			t->connected = 0;
			t->address = address;
		}
	}
	return t;
}

void p_simple_term_bus_connector_set(p_simple_term *t, m_bus *bus) {

	if (t && bus) {
		if (!t->connected) {
			free(t->bus);
		}
		t->bus = bus;
		t->connected = 1;
	}
	return;
}

void p_simple_term_free(p_simple_term *t) {

	if (t) {
		if (!t->connected) {
			free(t->bus);
		}
		free(t);
	}
	return;
}

void p_simple_term_clock(p_simple_term *t) {

	if (!t) {
		return;
	}

	/*
	 * If the miniat is making a request, has the address set and is writing
	 * we ack that we have received the request and take the data off the bus
	 */
	if (t->bus->req && (t->bus->address == t->address) && t->bus->rW && !t->bus->ack) {
		t->bus->ack = M_HIGH;
		printf("%c", (char)t->bus->data);
		fflush(stdout);
	}
	/* If we ack'd bring it back low */
	else if ((t->bus->address == t->address) && t->bus->ack) {
		t->bus->ack = M_LOW;
	}
	return;
}

m_bus p_simple_term_get_bus(p_simple_term *t) {
	m_bus b = { 0 };
	if (!t) {
		return b;
	}
	return *(t->bus);
}

void p_simple_term_set_bus(p_simple_term * t, m_bus bus) {

	if (t) {
		memcpy(t->bus, &bus, sizeof(bus));
	}
}
