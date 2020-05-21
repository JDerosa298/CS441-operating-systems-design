;-----------------------------------------------
; Name        : example.asm
; Author      : William "Amos" Confer
; 
; License     : Copyright (c) 2008--2014 William "Amos" Confer
;               
;    Permission is hereby granted, free of charge, to any person obtaining a 
;    copy of this software and associated documentation files (the "Software"),
;    to deal in the Software without restriction, including without limitation
;    the rights to use, copy, modify, merge, publish, distribute, sublicense, 
;    and/or sell copies of the Software, and to permit persons to whom the
;    Software is furnished to do so, subject to the following conditions:
;
;    The above copyright notice and this permission notice shall be included in
;    all copies or substantial portions of the Software.;  
;
;    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
;    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
;    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
;    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
;    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
;    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
;    DEALINGS IN THE SOFTWARE.
;
; This application prints "Hello, world" and prompts for two lowercase letters.
; It then prints the lowercase alphabet, but draws the two user letters in
; their uppercase forms.
;
; -Amos
;-----------------------------------------------


.message	"Hello, world" and nifty alphabet

.constant	TERM	0xFF

.constant	CR	0xD
.constant	LF	0xA

	jmp	!main

.variable	key

!crlf
.variable	crlf0	CR
.variable	crlf1	LF
.variable	crlf2	0

!hello
.variable	hello0	'H'
.variable	hello1	'e'
.variable	hello2	'l'
.variable	hello3	'l'
.variable	hello4	'o'
.variable	hello5	','
.variable	hello6	' '
.variable	hello7	'w'
.variable	hello8	'o'
.variable	hello9	'r'
.variable	hello10	'l'
.variable	hello11	'd'
.variable	hello12	'.'
.variable	hello13	CR
.variable	hello14	LF
.variable	hello15	0

!one
.variable	one0	'1'
.variable	one1	')'
.variable	one2	' '
.variable	one3	0

!two
.variable	two0	'2'
.variable	two1	')'
.variable	two2	' '
.variable	two3	0

!prompt
.variable	propmt0  'E'
.variable	propmt1  'n'
.variable	propmt2  't'
.variable	propmt3  'e'
.variable	propmt4  'r'
.variable	propmt5  ' '
.variable	propmt6  'a'
.variable	propmt7  ' '
.variable	propmt8  'l'
.variable	propmt9  'o'
.variable	propmt10 'w'
.variable	propmt11 'e'
.variable	propmt12 'r'
.variable	propmt13 'c'
.variable	propmt14 'a'
.variable	propmt15 's'
.variable	propmt16 'e'
.variable	propmt17 ' '
.variable	propmt18 'l'
.variable	propmt19 'e'
.variable	propmt20 't'
.variable	propmt21 't'
.variable	propmt22 'e'
.variable	propmt23 'r'
.variable	propmt24 ':'
.variable	propmt25 ' '
.variable	propmt26 0

!main
	mov	R7	!hello
	call	!print_string
	mov	R7	!crlf
	call	!print_string
	
	mov	R7	!one
	call	!print_string
	mov	R7	!prompt
	call	!print_string

	call	!read_letter
	load	R0	key
	stor	TERM	R0
	mov	R7	!crlf
	call	!print_string

	mov	R7	!two
	call	!print_string
	mov	R7	!prompt
	call	!print_string

	call	!read_letter
	load	R1	key
	stor	TERM	R1
	mov	R7	!crlf
	call	!print_string

	mov	R7	!crlf
	call !print_string

	mov	R2	'a'
!ascii		
	cmp	R2	R0
	jne	!lower_R0
	mov	R7	R2
	call	!draw_uppercase
	jmp	!ascii_test
!lower_R0
	cmp	R2	R1
	jne	!lower_R1
	mov	R7	R1
	call	!draw_uppercase
	jmp	!ascii_test

!lower_R1	
	stor	TERM	R2	

!ascii_test
	add	R2	1
	cmp	R2	'z'
	jeq	!ascii
	jls	!ascii
!acsii_end

	ret	;returning beyond the stack ends the application
!main_end

;------------------------------------------------------------------------------
; print the character string pointed to by R7 (register 7) until the NULL 
; character is found.
;------------------------------------------------------------------------------
!print_string	; (PS)
	push	R6	
	push	R7
	;while (mem[R7] != 0) { print(mem[R7]); R7++ }
!while_PS	
	loadp	R6	R7
	cmp	R6	0
	jeq	!while_PS_end
	stor	TERM	R6
	add	R7	1
	jmp	!while_PS
!while_PS_end
	pop	R7
	pop	R6
	ret
!print_string_end


;------------------------------------------------------------------------------
; swallow keystrokes until a lowercase letter is found.  Returns key in 
; the variable 'key'.
;------------------------------------------------------------------------------
!read_letter	; (RL)
	push	R0
!do_RL
	load	R0	TERM
!do_RL_while
	cmp	R0	'a'
	jls	!do_RL
	cmp	R0	'z'
	jgr	!do_RL
!do_RL_end
	stor	key	R0

	pop	R0
	ret
!read_letter_end

;------------------------------------------------------------------------------
; draws the lowercase character in R7 as uppercase;
;------------------------------------------------------------------------------
!draw_uppercase
	push	R6
	push	R7
		
	mov	R6	' '
	stor	TERM	R6
	sub	R7	'a'
	add	R7	'A'
	stor	TERM	R7
	stor	TERM	R6

	pop	R7
	pop	R6
	ret
!draw_uppercase_end

