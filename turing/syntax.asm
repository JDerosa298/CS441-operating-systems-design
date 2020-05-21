; This is an example assembly file for the TMasm
; assembler.  Semicolons start comments running to
; the end of the line.


; Labels are equivalent to the immediate address of whatever the next
; line of code would be, if one existed in the same address block.

!label  ; same as address 0x0000

	alpha 'a'       ; adds 'a' to alphabet
	alpha "bAB"     ; adds 'b', 'A', and 'B', also
!label#2  ; same as address 0x0004
	alpha off 'A'   ; removes 'A' from the alphabet
	alpha off "ba"  ; removes 'b' and 'a' from the alphabet

	; The CMP instructions will cause failure in the machine if the
	; head has a character not in the current alphabet

	cmp 'B'     ; set EQ flag if head has 'B', clear otherwise
	cmp blank   ; set EQ flag if head has blank, clear otherwise

	draw 'E'    ; draw an 'E' at the head
	draw blank  ; erase the tape at the head

	brane !label
	brae !label#2
	bra !3_is_the_label_4_me

!3_is_the_label_4_me  ; same as address 0x000E
	left   ; go left 1
	right  ; go right 1

	fail
	halt
