/*
 * Name        : token.h
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

#ifndef _TOKEN_H_
#define _TOKEN_H_

#include "list.h"
#include "miniat/miniat.h"


typedef struct Token Token;
typedef list token_list;

struct Token {
	char *token_str;
	int line_num;
	int type;
	m_word data;
};

extern token_list *tokens;
//extern token_list *extra_tokens;

/*
 * token_list_init()
 *
 * Initializes the "tokens" list to an empty state.
 */
extern void token_list_init();

/*
 * token_list_add()
 *
 * Adds the token to the tail of the "tokens" list .
 * Returns the address of the token in the table if added, NULL otherwise.
 */
extern Token *token_list_append(Token *tok);

#endif /* _TOKEN_H_ */

