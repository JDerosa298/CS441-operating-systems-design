/*
 * Name        : token.c
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
#include <stdlib.h>
#include <string.h>

#include "token.h"
#include "util.h"
#include "list.h"
 #include "mash.h"

token_list tokens_list;
token_list *tokens = &tokens_list;
//token_list *extra_tokens;

void token_free(void *item) {

	Token *t = (Token *)item;
	if(t) {
		if(t->token_str) {
			free(t->token_str);
		}
		free(t);
	}

	return;
}

int token_compare(void *item1, void *item2) {

	ERROR("There shold be no comparisons on the token list");

	return 1;
}

void token_list_init() {

	list_init(tokens, token_free, token_compare);

	return;
}

Token *token_list_append(Token *tok) {

	return (Token *)list_append((list *)tokens, (void *)tok);
}
