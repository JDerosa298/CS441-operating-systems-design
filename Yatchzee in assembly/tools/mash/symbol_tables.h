/*
 * Name        : symbol_tables.h
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
#ifndef _SYMBOL_TABLES_H_
#define _SYMBOL_TABLES_H_

#include <stdlib.h>
#include <stdbool.h>

#include "util.h"
#include "list.h"

typedef struct symbol symbol;
typedef struct list symbol_table;

struct symbol {
	char *str;
	m_word value;
};

extern symbol_table *labels;
extern symbol_table *constants;
extern symbol_table *variables;

/*
 * symbol_table_init()
 *
 * Initializes a symbol table to an empty state.  The argument should already
 * be allocated... perhaps the address of an static/global instance.
 */
extern void symbol_table_init(symbol_table *t);

/*
 * symbol_table_add()
 *
 * Adds the string::value pair to the symbol table if it isn't there already.
 * Returns the address of the symbol in the table if added, NULL otherwise.
 */
extern symbol *symbol_table_add(symbol_table *t, char *str, m_word value);

/*
 * symbol_table_search()
 *
 * searches for a symbol table entry with the same str.  Returns the address of
 * the matched symbol; NULL if no match found.
 */
extern symbol *symbol_table_search(symbol_table *t, char *str);


/*
 * symbol_table_print()
 *
 * prints the string:value pairs maintained in the symbol table.
 */
extern void symbol_table_print(symbol_table *t, char *header);

#endif /* _SYMBOL_TABLES_H_ */
