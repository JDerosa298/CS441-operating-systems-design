/*
 * Name        : encodings.h
 * Author      : William "Amos" Confer
 *
 * License     : Copyright (C) 2019 All rights reserved
 */

#ifndef __ENCODINGS_H__
#define __ENCODINGS_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint16_t tm_uword;
typedef int16_t tm_sword;

typedef union tm_word tm_word;
typedef union tm_encoding tm_encoding;
typedef struct tm_instruction tm_instruction;

union tm_word {
     struct {
          uint16_t byte0 : 8;
          uint16_t byte1 : 8;
     } bytes;
     tm_uword u;
     tm_sword s;
};


#define TM_OPCODE_LEFT   0
#define TM_OPCODE_RIGHT  1
#define TM_OPCODE_HALT   2
#define TM_OPCODE_FAIL   3
#define TM_OPCODE_DRAW   4
#define TM_OPCODE_ALPHA  5
#define TM_OPCODE_BRAE   6
#define TM_OPCODE_BRANE  7
#define TM_OPCODE_BRA    8
#define TM_OPCODE_CMP    9

union tm_encoding {
	struct {
          uint16_t rsvd   : 12;
          uint16_t opcode : 4;
     } generic;
	struct {
          uint16_t rsvd   : 12;
          uint16_t opcode : 4;
     } left;
     struct {
          uint16_t rsvd   : 12;
          uint16_t opcode : 4;
     } right;
     struct {
          uint16_t rsvd   : 12;
          uint16_t opcode : 4;
     } halt;
     struct {
          uint16_t rsvd   : 12;
          uint16_t opcode : 4;
     } fail;
     struct {
          uint16_t letter : 8;
          uint16_t rsvd   : 3;
          uint16_t blank  : 1;
          uint16_t opcode : 4;
     } draw;
     struct {
          uint16_t letter : 8;
          uint16_t rsvd   : 3;
          uint16_t on     : 1;
          uint16_t opcode : 4;
     } alpha;
     struct {
          uint16_t addr   : 12;
          uint16_t opcode : 4;
     } brae;
     struct {
          uint16_t addr   : 12;
          uint16_t opcode : 4;
     } brane;
     struct {
          uint16_t addr   : 12;
          uint16_t opcode : 4;
     } bra;
     struct {
          uint16_t letter : 8;
          uint16_t rsvd   : 3;
          uint16_t blank  : 1;
          uint16_t opcode : 4;
     } cmp;
     tm_word word;
};

struct tm_instruction {
     int line_num;
     tm_encoding encoding;
};

tm_encoding BAD_ENCODING_SIZE[((sizeof(tm_encoding) != 2) * -2) + 1];

#ifdef __cplusplus
}
#endif

#endif /* __ENCODINGS_H__ */
