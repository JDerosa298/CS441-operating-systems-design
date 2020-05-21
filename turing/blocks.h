/*
 * Name        : blocks.h
 * Author      : William "Amos" Confer
 *
 * License     : Copyright (C) 2019 All rights reserved
 */

#ifndef _BLOCKS_H_
#define _BLOCKS_H_

#include "encodings.h"
#include "list.h"

typedef struct block block;
typedef struct block_word block_word;

struct block {
	tm_uword address;
	list *words;
};

struct block_word {
	int line_number;
	tm_uword data;
	tm_uword address;
};

extern list *blocks;
extern list *flat_block_words;

extern void blocks_init();
extern void block_new(tm_uword address);
extern void block_new_word(int line_number, tm_uword data);
extern void cleanup_blocks();

extern void blocks_print();

#endif /* _BLOCKS_H_ */
