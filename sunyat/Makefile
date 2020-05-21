# Name        : Makefile
# Author      : William "Amos" Confer
# 
# License     : Copyright (c) 2008--2014 William "Amos" Confer
#               
#    Permission is hereby granted, free of charge, to any person obtaining a 
#    copy of this software and associated documentation files (the "Software"),
#    to deal in the Software without restriction, including without limitation
#    the rights to use, copy, modify, merge, publish, distribute, sublicense, 
#    and/or sell copies of the Software, and to permit persons to whom the
#    Software is furnished to do so, subject to the following conditions:
#
#    The above copyright notice and this permission notice shall be included in
#    all copies or substantial portions of the Software.;  
#
#    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
#    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
#    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
#    DEALINGS IN THE SOFTWARE.

ifeq (Windows_NT,$(OS))
NCURSES_LFLAGS = -lpdcurses
else
NCURSES_LFLAGS = -lncurses
endif

all:	sunyat sunyat-asm example

#----------------------------------------
# sunyat
#----------------------------------------
sunyat:	sunyat.c sunyat.h build_number.h
	gcc -o sunyat sunyat.c $(NCURSES_LFLAGS)

#----------------------------------------
# sunyat-asm
#----------------------------------------
sunyat-asm:	lemon lex.yy.c sunyat-asm.c token.h sunyat.h build_number.h
	gcc -o sunyat-asm lex.yy.c sunyat-asm.c

lemon:	lemon.c
	gcc -o lemon lemon.c

lex.yy.c:	sunyat-asm.lex
	flex sunyat-asm.lex

sunyat-asm.h:
sunyat-asm.c:	sunyat-asm.y sunyat-asm.h lemon
	./lemon sunyat-asm.y

build_number.h:	next_build_number

next_build_number:	
	gcc -o build_number build_number.c
	./build_number


#----------------------------------------
# example
#----------------------------------------
example: sunyat example.rom build_number.h
	./sunyat example.rom

example.rom: sunyat-asm example.asm build_number.h
	./sunyat-asm example.asm	

#----------------------------------------
# clean
#----------------------------------------
clean:
	rm -f sunyat sunyat-asm lemon lex.yy.c sunyat-asm.c sunyat-asm.h sunyat-asm.out example.rom build_number

