/*
 * Name        : list.c
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

#include <stdlib.h>
#include <string.h>

#include "list.h"

void list_init(list *x, /* void *(*constructor)(void *),*/ void (*destructor)(void *), int (*compare)(void *, void *)) {

	if(!x) ERROR("Cannot initialize unallocated list");
	if(!destructor) ERROR(" required to initialize list");
	if(!compare) ERROR(" required to initialize list");

	if(x->size > 0) {
		list_delete(x);
	}

	x->size = 0;
	x->head = NULL;
	x->tail = NULL;
	x->destructor = destructor;
	x->compare = compare;

	return;
}

void list_delete(list *x) {

	if(!x) ERROR("Cannot delete unallocated list");

	while(x->head) {
		list_node *next = x->head->next;
		x->destructor(x->head->item);
		free(x->head);
		x->head = next;
	}
	x->size = 0;
	x->head = NULL;
	x->tail = NULL;

	return;
}

list_node *list_append(list *x, void *item) {

	list_node *node;

	if(!x) ERROR("Cannot append to an unallocated list");

	SAFE_MALLOC(node, list_node *, sizeof(list_node));
	node->item = item;
	node->next = NULL;

	if(x->size == 0) {
		x->head = node;
	}
	else {
		x->tail->next = node;
	}
	x->tail = node;
	x->size++;

	return node;
}

void *list_search(list *x, void *item) {

	list_node *current;
    list_node *result = NULL;

	if(!x) ERROR("Cannot append to an unallocated list");
	if(!item) ERROR("Cannot search list for nothing");

	current = x->head;
	while(current != NULL && (x->compare(current->item, item) != 0)) {
		current = current->next;
	}

    if(current != NULL) {
        result = current->item;
    }

    return result;
}
