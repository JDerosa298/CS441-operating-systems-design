#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <miniat/miniat.h>
#include "simple_kb.h"

struct p_simple_kb {
	int connected;
	m_uword address;
	m_bus *bus;
};

p_simple_kb *p_simple_kb_new(m_uword address) {

	p_simple_kb *k = (p_simple_kb *)malloc(sizeof(p_simple_kb));
	if (k) {
		k->bus = (m_bus *)malloc(sizeof(m_bus));
		if (!k->bus) {
			free(k);
			k = NULL;
		}
		else {
			k->address = address;
			k->connected = 0;
		}
	}
	return k;
}

void p_simple_kb_bus_connector_set(p_simple_kb *k, m_bus *bus) {

	if (k && bus) {
		if (!k->connected) {
			free(k->bus);
		}
		k->bus = bus;
		k->connected = 1;
	}
	return;
}

void p_simple_kb_free(p_simple_kb *k) {

	if (k) {
		if (!k->connected) {
			free(k->bus);
		}
		free(k);
	}
	return;
}

void p_simple_kb_clock(p_simple_kb *k) {

	int rc;
	char data;
	if (!k) {
		return;
	}

	/*
	 * If the miniat is making a request, has the address set and is reading
	 * we ack that we have received the request and take the data off the bus
	 */
	if (k->bus->req && (k->bus->address == k->address) && !k->bus->rW && !k->bus->ack) {
		k->bus->ack = M_HIGH;
		rc = scanf("%c", &data);
		if (rc == EOF) {
			data = 0;
		}
		k->bus->data = data;
	}
	/* If we ack'd bring it back low */
	else if (k->bus->ack && (k->bus->address == k->address)) {
		k->bus->ack = M_LOW;
	}
	return;
}

m_bus p_simple_kb_get_bus(p_simple_kb *k) {
	m_bus b = { 0 };
	if (!k) {
		return b;
	}
	return *(k->bus);
}

void p_simple_kb_set_bus(p_simple_kb * k, m_bus bus) {

	if (k) {
		memcpy(k->bus, &bus, sizeof(bus));
	}
}
