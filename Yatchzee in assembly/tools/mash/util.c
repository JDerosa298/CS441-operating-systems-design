/*
 * Name        : util.c
 * Author      : William "Amos" Confer
 *
 * License     : Copyright (C) 2008--2015 MiniAT Project
 *
 * This file is part of MiniAT.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>

#include "util.h"

bool option_dump_hex         = false;
bool option_generate_listing = false;
bool option_parser_info      = false;
bool option_tokenizer_info   = false;
bool option_verbose_info     = false;

bool mode_nop_delay = true;

int stage_error_cnt = 0;

int pass;
m_word address;
bool inline_variables;
m_word variable_address;

static const char *lvl_string[] = {
		"", /* Info messages have no lead */
		"Error: ",
		"Impossible: "
};

void util_print(util_print_level lvl, FILE *out, const char *filename, int lineno, const char *fmt, ...) {

	va_list args;
	va_start(args, fmt);
	fflush(NULL);
	fprintf(out, "%s", lvl_string[lvl]);
	if(filename) {
		fprintf(out, "%s", filename);
		if(lineno > 0) {
			fprintf(out, ":%d", lineno);
		}
		fprintf(out, " - ");
	}
	vfprintf(out, fmt, args);
	va_end(args);
	fflush(out);
}
