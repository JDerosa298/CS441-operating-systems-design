; demo.asm
;
; redeces runs of 'a's or 'b's to a single 'a' or 'b'

!main
	alpha "ab"

	cmp 'a'
	brae !new_a
	cmp 'b'
	brae !new_b
	halt

!new_a
	alpha 'A'
	alpha 'B'
	draw blank
	right
	cmp 'a'
	brae !new_a

!to_end_with_a
	cmp blank
	brae !right_of_possible_blank_with_a
	right
!right_of_possible_blank_with_a
	brane !to_end_with_a

	draw 'A'
	left
	bra !to_first_blank

!new_b
	alpha 'A'
	alpha 'B'
	draw blank
	right
	cmp 'b'
	brae !new_b

!to_end_with_b
	cmp blank
	brae !right_of_possible_blank_with_b
	right
!right_of_possible_blank_with_b
	brane !to_end_with_b

	draw 'B'
	left
	bra !to_first_blank

!to_first_blank
	cmp blank
	brae !left_of_possible_blank
	left
!left_of_possible_blank
	brane !to_first_blank

	right

	cmp 'a'
	brae !new_a
	cmp 'b'
	brae !new_b

!go_right
	cmp blank
	brae !going_right_past_possible_blank
	right
!going_right_past_possible_blank
	brane !go_right

	left

!finalize
	cmp 'A'
	brae !lower_A
	cmp 'B'
	brae !lower_B

	right
	halt

!finale
	cmp blank
	brae !finale_left_of_possible_blank
	left
!finale_left_of_possible_blank
	brane !finale

	right
	halt

!lower_A
	draw 'a'
	left
	bra !finalize

!lower_B
	draw 'b'
	left
	bra !finalize
