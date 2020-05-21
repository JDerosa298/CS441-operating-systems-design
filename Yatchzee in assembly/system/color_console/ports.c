
#include "miniat/miniat.h"
#include "ports.h"
#include "term.h"

typedef union cursor_position cursor_position;
typedef union color_port color_port;


union cursor_position {
	struct {
		m_uwyde x: 8;
		m_uwyde y: 8;
	};
	m_wyde all;
};

union color_port {
	struct {
		m_uwyde fg: 3;
		m_uwyde bg: 3;
		m_uwyde reserved: 9;
		m_uwyde trigger: 1;
	};
	m_wyde all;
};

void ports_clock(miniat *m) {

	cursor_position port_a;
	m_wyde port_b;
	static color_port port_c = {.all = { .u = 0 }};
	static color_port old_port_c = { .all = { .u = 0}};

	port_a.x = (unsigned)cursor_col;
	port_a.y = (unsigned)cursor_row;
	miniat_pins_set_gpio_port(m, m_gpio_id_A, port_a.all);

	port_b.s = terminal[cursor_row][cursor_col];
	miniat_pins_set_gpio_port(m, m_gpio_id_B, port_b);

	port_c.all = miniat_pins_get_gpio_port(m, m_gpio_id_C);
	if (port_c.trigger != old_port_c.trigger) {
		terminal_set_fg_color(port_c.fg);
		terminal_set_bg_color(port_c.bg);

		old_port_c.all.u = port_c.all.u;
	}

	return;
}
