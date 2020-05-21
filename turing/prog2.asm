!1
    alpha "01"
    cmp '0'
    brae !go_right_on_0
    cmp '1'
    brae !go_right_on_1
    cmp blank
    brae !1_blank

!1_blank
    left
    bra !11

!go_right_on_0
    right
    bra !1

!go_right_on_1
    right

!2
    cmp blank
    right
    brane !2

!3
    alpha 'a'
    cmp 'a'
    brae !3_a
    draw 'a'
    left
    bra !4

!3_a
    right
    bra !3

!4
    alpha 'a'
    cmp 'a'
    brae !4_a
    cmp blank
    brae !4_blank

!4_a
    left
    bra !4

!4_blank
    left

!5
    cmp '0'
    brae !5_0
    cmp '1'
    brae !5_1

!5_0
    draw '1'
    left
    bra !5

!5_1
    draw '0'
    left

!6
    cmp '0'
    brae !6_0
    cmp '1'
    brae !6_1
    cmp blank;
    brae !6_b

!6_0
    draw '0'
    left
    bra !6

!6_1
    draw '1'
    left
    bra !6

!6_b   ;not really needed
    right

!7
    cmp '0'
    brae !7_0
    cmp '1'
    brae !7_1
    cmp blank ; can use bra instead
    brae !7_b 

!7_0
    right
    bra !7

!7_1
    right
    bra !2

!7_b
    left

!8
    cmp blank
    brane !8_not_b
    right
    brae !9



!8_not_b
    draw blank
    left
    bra !8

!9
    cmp blank
    brae !9_b
    brane !9_a

!9_b
    right
    bra !9

!9_a
    left

!10
    right
    halt



!11
    cmp '0'
    brae !11_0
    cmp '1'
    brae !11_1
    left
    halt

!11_0
    draw blank
    left
    bra !11

!11_1
    draw blank
    left
    bra !11



