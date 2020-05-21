#include <ctype.h>
#include <stdarg.h>

#include "util.h"

void util_print(util_print_level lvl, FILE *out, const char *filename, int lineno, const char *fmt, ...) {

	va_list args;
	va_start(args, fmt);
	fflush(NULL);
	switch (lvl) {
	case util_print_level_error:
		fprintf(out, "Error: ");
		break;
	case util_print_level_impossible:
		fprintf(out, "Impossible, %s:%d - This should never happen...\n\n", filename, lineno);
		break;
	}
	vfprintf(out, fmt, args);
	va_end(args);
	fprintf(out, "\n");
	fflush(out);

	return;
}
