!1
    alpha"01"
    cmp '0'
    brae !rightIfZero
    cmp '1'
    brae !rightIfOne
    cmp blank
    brae !1Blank

!1Blank
    left
    bra !11

!rightIfZero
    right
    bra !1

!rightIfOne
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
    brae !4When_a
    cmp blank
    brae !4_blank

!4When_a
    left
    bra !4

!4_blank
    left

!5
  cmp '0'
  brae !5When0
  cmp '1'
  brae !5When1

!5When0
    draw '1'
    left
    bra !5

!5When1
    draw '0'
    left

!6
  cmp '0'
  brae !6When0
  cmp '1'
  brae !6When1
  cmp blank
  brae !6blank

!6When0
    draw '0'
    left
    bra !6


!6When1
    draw '1'
    left
    bra !6

!6blank
    right

!7
  cmp '0'
  brae !7When0
  cmp '1'
  brae !7When1
  cmp blank
  brae !7blank

!7When0
      right
      bra !7

!7When1
      right
      bra !2

!7blank
      left

!8
  cmp blank
  brane !8notBlank
  right
  brae !9

!8notBlank
  draw blank
  left
  bra !8

!9
    cmp blank
    brae !9Whenb
    brane !9Whena

!9Whenb
    right
    bra !9

!9Whena
      left

!10
    right
    halt

!11
    cmp'0'
    brae !11When0
    cmp '1'
    brae !11When1
    left
    halt

!11When0
    draw blank
    left
    bra !11

!11When1
    draw blank
    left
    bra !11
