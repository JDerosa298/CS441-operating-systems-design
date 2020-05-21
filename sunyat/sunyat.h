/*
 * Name        : sunyat.h
 * Author      : William "Amos" Confer
 * 
 * License     : Copyright (c) 2008--2014 William "Amos" Confer
 *              
 *    Permission is hereby granted, free of charge, to any person obtaining a 
 *    copy of this software and associated documentation files (the "Software"),
 *    to deal in the Software without restriction, including without limitation
 *    the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 *    and/or sell copies of the Software, and to permit persons to whom the
 *    Software is furnished to do so, subject to the following conditions:
 *
 *    The above copyright notice and this permission notice shall be included in
 *    all copies or substantial portions of the Software.;  
 *
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 *    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 *    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 *    DEALINGS IN THE SOFTWARE.
 */
#ifndef _SUNYAT_H_
#define _SUNYAT_H_

/*
 * On read:  0 if no key, key code otherwise
 * On write: character output w/ cursor adjust
 */
#define IO_TERMINAL     0xFF

#define APP_RAM_SIZE    0xFF /* same as the lowest device address */
#define APP_MSG_SIZE    70
#define APP_ROM_SIZE    (APP_MSG_SIZE + APP_RAM_SIZE)

#define OPCODE_MOV_RR   0
#define OPCODE_MOV_RI   1
#define OPCODE_ADD_RR   2
#define OPCODE_ADD_RI   3
#define OPCODE_SUB_RR   4
#define OPCODE_SUB_RI   5
#define OPCODE_MUL_RR   6
#define OPCODE_MUL_RI   7
#define OPCODE_DIV_RR   8
#define OPCODE_DIV_RI   9
#define OPCODE_CMP_RR   10
#define OPCODE_CMP_RI   11
#define OPCODE_JMP_M    12
#define OPCODE_JEQ_M    13
#define OPCODE_JNE_M    14
#define OPCODE_JGR_M    15
#define OPCODE_JLS_M    16
#define OPCODE_CALL_M   17
#define OPCODE_RET      18
#define OPCODE_AND_RR   19
#define OPCODE_AND_RI   20
#define OPCODE_OR_RR    21
#define OPCODE_OR_RI    22
#define OPCODE_XOR_RR   23
#define OPCODE_XOR_RI   24
#define OPCODE_NEG_R    25
#define OPCODE_LOAD_RM  26
#define OPCODE_LOADP_RR 27
#define OPCODE_STOR_MR  28
#define OPCODE_STORP_RR 29
#define OPCODE_PUSH_R   30
#define OPCODE_POP_R    31

#endif /* _SUNYAT_H_ */
