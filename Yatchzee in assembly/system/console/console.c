#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#include "miniat/miniat.h"
#include "simple_term.h"
#include "simple_kb.h"

#define M_SIMPLE_KB_ADDRESS   0x4002
#define M_SIMPLE_TERM_ADDRESS 0x4000

miniat *m = NULL;
p_simple_term *t = NULL;
p_simple_kb *k = NULL;

typedef struct miniat_files miniat_files;

struct miniat_files {
	const char *input_filename;
	const char *cache_filename;
};

static void print_usage(const char *name) {
	printf("usage: %s binfile [cache script]\n", name);
}

static miniat_files get_arguments(int argc, char *argv[]) {

	const char *name = argv[0];
	int arg_count = argc -1;

	/* Boiler plate code for arguments */
	if (arg_count <  1 || arg_count > 2) {
		print_usage(name);
		THROW(EINVAL, "Expecting 1 or 2 arguments, got: %d", arg_count);
	}

	miniat_files files = { 0 };

	switch (arg_count) {
	case 2:
		files.cache_filename = argv[arg_count];
		arg_count--;
		/* no break */
	case 1:
		files.input_filename = argv[arg_count];
		break;
	default:
		print_usage(name);
		exit(EXIT_FAILURE);
	}

	return files;
}

static void miniat_start(int argc, char *argv[]) {

	m_bus bus_state; /* bus state from miniat */
	m_bus peripheral_bus_state; /* bus state from peripherals */

	miniat_files files = get_arguments(argc, argv);

	/*
	 * We need instances of the peripherals and a miniat that we want to use. Since this
	 * system uses a keyboard and terminal, we create those objects to use in our system.
	 */
	m = miniat_file_new(files.input_filename, files.cache_filename);

	t = p_simple_term_new(M_SIMPLE_TERM_ADDRESS);
	k = p_simple_kb_new(M_SIMPLE_KB_ADDRESS);

	/* Just do it for ever, the miniat handles the ctr^c keyboard interrupt, so you can always
	   stop it by pressing ctr^c on the command line */
	for (;;) {

		/* Give the miniat a clock cycle */
		miniat_clock(m);

		/*
		 * After the miniat does something, we need to get any data that could
		 * be waiting for peripherals on the bus, or other pin sources. Were using
		 * the bus in these examples.
		 */
		bus_state = miniat_pins_bus_get(m);

		/*
		 * Transfer the data to the terminal.
		 * it only accepts data if the address is appropriate as per the comment
		 * above this function declaration.
		 */
		p_simple_term_set_bus(t, bus_state);

		/*
		 * Transfer the data to the Keyboard.
		 * it only accepts data if the address is appropriate as per the comment
		 * above this function declaration.
		 */
		p_simple_kb_set_bus(k, bus_state);

		/*
		 * Give the terminal and kb a clock so it can do something with the data we got
		 * from the miniat and set to the peripherals.
		 */
		p_simple_term_clock(t);
		p_simple_kb_clock(k);

		/*
		 * Transfer the data from the peripheral back to the miniat, we need to do
		 * transfer these by current address on the miniat "bus" since the miniat
		 * listens for everything. We wouldn't want an inactive peripherals data
		 * to clobber the active peripherals data on the wire.
		 */
		peripheral_bus_state = p_simple_term_get_bus(t);
		if (peripheral_bus_state.address == M_SIMPLE_TERM_ADDRESS) {
			goto set_address;
		}

		peripheral_bus_state = p_simple_kb_get_bus(k);
		if (peripheral_bus_state.address == M_SIMPLE_KB_ADDRESS) {
			goto set_address;
		}

		/* Ignore address 0, as it is a 'return to 0' bus design, 0 may be on the bus */
		if (peripheral_bus_state.address == 0) {
			continue;
		}

		/* Error if we got here log it, and keep going! The ugly cast keeps the compiler happy*/
		fprintf(stderr, "Could not find peripheral mapped to address: %lu\n",
		        (long unsigned int)peripheral_bus_state.address);
		continue;

		/*
		 * If we got here, we just need to transfer the active peripherals data back to the miniat
		 * so it can process it.
		 */
set_address:
		miniat_pins_bus_set(m, peripheral_bus_state);
	}

	/* Free the data we use before exiting */
	p_simple_term_free(t);
	p_simple_kb_free(k);
	miniat_free(m);
}

static void cleanup(int signum) {
	(void)signum;

	if (m) {
		miniat_free(m);
		m = NULL;
	}
	if (t) {
		p_simple_term_free(t);
		t = NULL;
	}
	if (k) {
		p_simple_kb_free(k);
		k = NULL;
	}
	exit(1);
}

int main(int argc, char *argv[]) {
	TRY {
		signal(SIGINT, cleanup);
		miniat_start(argc, argv);
	}
	CATCHALL {
		miniat_dump_error();
	}
	FINALLY {
		cleanup(0);
	}
	ETRY;
	return 0;
}
