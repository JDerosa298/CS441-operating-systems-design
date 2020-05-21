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

	m_bus *bus; /* bus state from miniat, when we use connectors it must be pointer! */

	miniat_files files = get_arguments(argc, argv);

	/*
	 * We need instances of the peripherals and a miniat that we want to use. Since this
	 * system uses a keyboard and mouse, we create those objects to use in our system.
	 *
	 * This version of simple echo terminal uses miniat_file_new, which means you don't
	 * need to write the code that opens a file.
	 */
	m = miniat_file_new(files.input_filename, files.cache_filename);

	bus = miniat_conector_bus_get(m);

	t = p_simple_term_new(M_SIMPLE_TERM_ADDRESS);
	k = p_simple_kb_new(M_SIMPLE_KB_ADDRESS);

	/*
	 * This example uses connectors. Connectors make your life easier. Once you
	 * connect a peripheral to a miniat bus connector, you don't need to manually
	 * transfer the data back and forth.
	 */
	p_simple_kb_bus_connector_set(k, bus);
	p_simple_term_bus_connector_set(t, bus);

	/* Just do it for ever, the miniat handles the ctr^c keyboard interrupt, so you can always
	   stop it by pressing ctr^c on the command line */
	for (;;) {

		/* Give the miniat a clock cycle */
		miniat_clock(m);

		/*
		 * Give the terminal and kb a clock so it can do something with the data we got
		 * from the miniat and set to the peripherals.
		 */
		p_simple_term_clock(t);
		p_simple_kb_clock(k);

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
