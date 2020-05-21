# peripheral constants

.const PERIPH_KEYB           0x4000
.const PERIPH_TERM           0x4001
.const PERIPH_TERM_FIXED     0x4002
.const PERIPH_RAW_TERM       0x4003
.const PERIPH_RAW_TERM_FIXED 0x4004
.const PERIPH_REFRESH        0x4005

.const PERIPH_CURSOR_X       0x4010
.const PERIPH_CURSOR_Y       0x4011
.const PERIPH_CURSOR_Y_X     0x4012
.const PERIPH_FG_COLOR       0x4013
.const PERIPH_BG_COLOR       0x4014
.const PERIPH_ATTR_UNDERLINE 0x4015
.const PERIPH_ATTR_REVERSE   0x4016
.const PERIPH_ATTR_BLINK     0x4017
.const PERIPH_ATTR_BOLD      0x4018
.const PERIPH_ATTR_INVIS     0x4019
.const PERIPH_ALL_ATTRIBUTES 0x401A
.const PERIPH_ATTR_STACK     0x401B
.const PERIPH_COORD_STACK    0x401C

.const PERIPH_FLUSH_BUFFER   0x4020
.const PERIPH_SHOW_CURSOR    0x4021
.const PERIPH_BEEP           0x4022
.const PERIPH_ERASE          0x4023
.const PERIPH_TABSIZE        0x4024
.const PERIPH_AUTO_REFRESH   0x4025

.const PERIPH_EXIT           0x4500


# port constants

.const PORT_CURSOR    0x1B1A
.const PORT_CHARACTER 0x1B1B
.const PORT_COLOR     0x1B1C

# "Visual" / drawing characters

.const DRAW_ULCORNER  -2
.const DRAW_LLCORNER  -3
.const DRAW_URCORNER  -4
.const DRAW_LRCORNER  -5
.const DRAW_LTEE      -6
.const DRAW_RTEE      -7
.const DRAW_BTEE      -8
.const DRAW_TTEE      -9
.const DRAW_HLINE    -10
.const DRAW_VLINE    -11
.const DRAW_PLUS     -12
.const DRAW_S1       -13
.const DRAW_S9       -14
.const DRAW_DIAMOND  -15
.const DRAW_CKBOARD  -16
.const DRAW_DEGREE   -17
.const DRAW_PLMINUS  -18
.const DRAW_BULLET   -19
.const DRAW_LARROW   -20
.const DRAW_RARROW   -21
.const DRAW_DARROW   -22
.const DRAW_UARROW   -23
.const DRAW_BOARD    -24
.const DRAW_LANTERN  -25
.const DRAW_BLOCK    -26
.const DRAW_S3       -27
.const DRAW_S7       -28
.const DRAW_LEQUAL   -29
.const DRAW_GEQUAL   -30
.const DRAW_PI       -31
.const DRAW_NEQUAL   -32
.const DRAW_STERLING -33

# Default color constants
.const COLOR_BLACK   0
.const COLOR_RED     1
.const COLOR_GREEN   2
.const COLOR_YELLOW  3
.const COLOR_BLUE    4
.const COLOR_MAGENTA 5
.const COLOR_CYAN    6
.const COLOR_WHITE   7

.const DELAY 30_000

<<<
	Part 1:

	Display the default color table
>>>

	movi  r1 = -1
	stor  [PERIPH_SHOW_CURSOR] = r0
	movi  r105 = 0xDEADC33D
	movi  r106 = 0xBADCC0DE

!part_1

# set FG and BG to white on black
	movi  r100 = COLOR_BLACK
	stor  [PERIPH_BG_COLOR] = r100
	movi  r100 = COLOR_WHITE
	stor  [PERIPH_FG_COLOR] = r100

	movi  r5 = 13
	movi  r6 = 7
	stor  [PERIPH_CURSOR_X] = r5
	stor  [PERIPH_CURSOR_Y] = r6
	movi  r211 = !p1_str_default_colors
	add   r210 = PC + 2
	bra   [!print_packed_string]

	movi  r9 = 'A'   # the character to print in the table
	movi  r6 = 9     # Y
	movi  r7 = 8     # row color
!draw_color_row
	dec   r7
	stor  [PERIPH_BG_COLOR] = r7
	movi  r5 = 36   # X
	movi  r8 = 8    # col color
!draw_color_col
	dec   r8
	stor  [PERIPH_CURSOR_X] = r5
	stor  [PERIPH_CURSOR_Y] = r6
	stor  [PERIPH_FG_COLOR] = r8
	stor  [PERIPH_TERM] = r9
	inc   r5
	brag [!draw_color_col], r8 > r0

	inc   r6
	brag [!draw_color_row], r7 > r0

	add   r200 = PC + 2
	bra   [!press_any_key]

	bra   [!part_2]

!p1_str_default_colors
	'Shown below are all the default FG and BG color pairs:'


!p2_str_attributes
	'There are other visual attributes that can be applied:'

!part_2

	stor  [PERIPH_ERASE] = r0

# set FG and BG to white on black
	movi  r100 = COLOR_BLACK
	stor  [PERIPH_BG_COLOR] = r100
	movi  r100 = COLOR_WHITE
	stor  [PERIPH_FG_COLOR] = r100

	movi  r5 = 13
	movi  r6 = 7
	stor  [PERIPH_CURSOR_X] = r5
	stor  [PERIPH_CURSOR_Y] = r6
	movi  r211 = !p2_str_attributes
	add   r210 = PC + 2
	bra   [!print_packed_string]


	movi  r6 = 9		# Y
	movi  r120 = 8		# lines left
!p2_next_line

	movi  r5 = 20		# X
	stor  [PERIPH_CURSOR_Y] = r6
	stor  [PERIPH_CURSOR_X] = r5

	movi  r121 = 40
!p2_next_char

# get random attributes
	add   r205 = PC + 2
	bra   [!rand]
	and   r122 = r206 & 0b_0111_0111_000_11111_00000000_00000000

# get a random character
	add   r205 = PC + 2
	bra   [!rand]
	and   r206 = r206 & 0x7FFF_FFFF
	mod   r206 = r206 % 26
	add   r206 = r206 + (r122 + 'A')

	stor  [PERIPH_RAW_TERM] = r206

	movi  r123 = 100
!p2_delay
	dec   r123
	brag  [!p2_delay], r123 > r0

	dec   r121
	brag  [!p2_next_char], r121 > r0

	inc   r6
	dec   r120
	brag  [!p2_next_line], r120 > r0



	add   r200 = PC + 2
	bra   [!press_any_key]

	bra   [!part_3]



!part_3

	movi  r100 = COLOR_BLACK
	stor  [PERIPH_BG_COLOR] = r100
	stor  [PERIPH_ERASE] = r0

	movi  r125 = !p3_logo
	load  r126 = [r125]
!p3_next_char
	stor  [PERIPH_RAW_TERM] = r126

	inc   r125
	load  r126 = [r125]
	brane  [!p3_next_char], r126 != r0

	add   r200 = PC + 2
	bra   [!press_any_key]

	stor  [PERIPH_EXIT] = r0

<<<
	!press_any_key

	Prompts for a key on the bottom of the screen and exits with white on black.
	This draws a box around the screen and may cover something drawn, so it
	delays a while first.

	Uses registers 100...101
	Expects return address in r200.
>>>

!str_press_any_key
	'Press any key to continue'

!press_any_key

	movi  r100 = DELAY
!pak_delay
	dec   r100
	brag [!pak_delay], r100 > r0

################################
# draw box around edge of screen
#

	stor  [PERIPH_ATTR_STACK] = r0
	stor  [PERIPH_COORD_STACK] = r0

# set FG and BG to yellow on blue
	movi  r100 = COLOR_BLUE
	stor  [PERIPH_BG_COLOR] = r100
	movi  r100 = COLOR_YELLOW
	stor  [PERIPH_FG_COLOR] = r100

# top edge
	stor  [PERIPH_CURSOR_X] = r0
	stor  [PERIPH_CURSOR_Y] = r0
	movi  r101 = DRAW_ULCORNER
	stor  [PERIPH_TERM] = r101

	movi  r101 = DRAW_HLINE
	movi  r100 = 78
!pak_top_bar
	stor  [PERIPH_TERM] = r101
	dec   r100
	brag [!pak_top_bar], r100 > r0

	movi  r101 = DRAW_URCORNER
	stor  [PERIPH_TERM] = r101

# bottom edge
	stor  [PERIPH_CURSOR_X] = r0
	movi  r101 = 23
	stor  [PERIPH_CURSOR_Y] = r101
	movi  r101 = DRAW_LLCORNER
	stor  [PERIPH_TERM] = r101

	movi  r101 = DRAW_HLINE
	movi  r100 = 78
!pak_bottom_bar
	stor  [PERIPH_TERM] = r101
	dec   r100
	brag [!pak_bottom_bar], r100 > r0

	movi  r101 = DRAW_LRCORNER
	stor  [PERIPH_TERM] = r101

# left edge
	movi  r101 = DRAW_VLINE
	movi  r102 = 1   # this is the Y value
	movi  r100 = 22
!pak_left_bar
	stor  [PERIPH_CURSOR_X] = r0
	stor  [PERIPH_CURSOR_Y] = r102
	stor  [PERIPH_TERM] = r101
	inc   r102
	dec   r100
	brag  [!pak_left_bar], r100 > r0

# right edge
	movi  r101 = DRAW_VLINE
	movi  r103 = 79  # this is the X value
	movi  r102 = 1   # this is the Y value
	movi  r100 = 22
!pak_right_bar
	stor  [PERIPH_CURSOR_X] = r103
	stor  [PERIPH_CURSOR_Y] = r102
	stor  [PERIPH_TERM] = r101
	inc   r102
	dec   r100
	brag  [!pak_right_bar], r100 > r0

# prompt for key
	movi  r100 = 28
	stor  [PERIPH_CURSOR_X] = r100
	movi  r100 = 23
	stor  [PERIPH_CURSOR_Y] = r100
	movi  r211 = !str_press_any_key
	add   r210 = PC + 2
	bra   [!print_packed_string]

!key_wait
	load  r100 = [PERIPH_KEYB]
	brae  [!key_wait], r100 == r1

	load  r0 = [PERIPH_ATTR_STACK]
	load  r0 = [PERIPH_COORD_STACK]

	bra   [r200]

<<<
	!rand

	random bit pattern returned in r206

	uses registers 105...107
	return addres expected in r205
>>>


!rand
	movr  r107 = r105
	shl   r105 = r105 << 7
	shr   r107 = r107 >> 25
	exor  r105 = r105 ^ r107
	add   r105 = r105 + r106
	movr  r206 = r105

	movr  r107 = r106
	shl   r106 = r106 << 7
	shr   r107 = r107 >> 25
	exor  r106 = r106 ^ r107
	add   r106 = r106 + 0xBADCC0DE

	bra   [r205]


<<<
	!print_packed_string

	Prints packed bytes starting at the address in r211

	uses registers 110...113
	return address expected in r210
>>>

!print_packed_string
	movi  r110 = 4
	movr  r111 = r211
	load  r112 = [r111]
!pps_next
	brag  [!pps_print_char], r110 > r0
	# move on to next word
	inc   r111
	movi  r110 = 4
	load  r112 = [r111]
!pps_print_char
	brae  [!pps_done], r112 == r0
	and   r113 = r112 & 0xFF
	stor  [PERIPH_TERM] = r113
	shr   r112 = r112 >> 8
	dec   r110
	bra   [!pps_next]
!pps_done
	bra   [r210]


!p3_logo

	10
	10
	10
	0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x0208002C 0x0208002E 0x0208002E 0x0208002E 0x0208002E 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x0208002C 0x0208002E 0x0208002E 0x0208002E 0x0208002E 10
	0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x0208005F 0x20090064 0x2009005E 0x2009005E 0x2009005E 0x2009005E 0x2009005E 0x2009005E 0x2009005E 0x2009005E 0x2009005E 0x20090062 0x0208005F 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x0208005F 0x20090064 0x2009005E 0x2009005E 0x2009005E 0x2009005E 0x2009005E 0x2009005E 0x2009005E 0x2009005E 0x2009005E 0x20090062 0x0208005F 10
	0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x0208002E 0x20090064 0x20090027 0x20090027 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x20090060 0x20090060 0x20090062 0x0208002E 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x0208002E 0x20090064 0x20090027 0x20090027 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x20090060 0x20090060 0x20090062 0x0208002E 10
	0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x0208002E 0x20090070 0x20090027 0x21000020 0x2100002D 0x2100002D 0x2100002D 0x2100002D 0x2100002D 0x2100002D 0x2100002D 0x2100002D 0x2100002D 0x2100002D 0x2100002D 0x2100002D 0x2100002D 0x2100002D 0x2100002D 0x21000020 0x20090060 0x20090071 0x0208002E 0x00000020 0x00000020 0x00000020 0x00000020 0x0208002E 0x20090070 0x20090027 0x21000020 0x2100002D 0x2100002D 0x2100002D 0x2100002D 0x2100002D 0x2100002D 0x2100002D 0x2100002D 0x2100002D 0x2100002D 0x2100002D 0x2100002D 0x2100002D 0x2100002D 0x2100002D 0x21000020 0x20090060 0x20090071 0x0208002E 10
	0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x0208002E 0x20090064 0x20090027 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x20090060 0x20090062 0x0208002E 0x00000020 0x00000020 0x0208002E 0x20090064 0x20090027 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x20090060 0x20090062 0x0208002E 10
	0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x0208002E 0x20090064 0x20090027 0x31000020 0x3100005F 0x3100005F 0x3100005F 0x3100005F 0x3100005F 0x3100005F 0x3100005F 0x3100005F 0x3100005F 0x3100005F 0x3100005F 0x3100005F 0x3100005F 0x3100005F 0x3100005F 0x3100005F 0x3100005F 0x3100005F 0x3100005F 0x31000020 0x20090060 0x02080062 0x00000020 0x00000020 0x02080064 0x20090027 0x31000020 0x3100005F 0x3100005F 0x3100005F 0x3100005F 0x3100005F 0x3100005F 0x3100005F 0x3100005F 0x3100005F 0x3100005F 0x3100005F 0x3100005F 0x3100005F 0x3100005F 0x3100005F 0x3100005F 0x3100005F 0x3100005F 0x3100005F 0x31000020 0x20090060 0x20090062 0x0208002E 10
	0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x0208002E 0x20090048 0x20090048 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x2009005C 0x2009002F 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x20090048 0x20090048 0x0208002E 10
	0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x02080064 0x20090048 0x20090048 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x20090048 0x20090048 0x02080062 10
	0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x02080039 0x20090048 0x20090048 0x31000020 0x3100002D 0x3100002D 0x3100002D 0x3100002D 0x3100002D 0x3100002D 0x3100002D 0x3100002D 0x3100002D 0x3100002D 0x3100002D 0x31000020 0x2009002E 0x31000020 0x3100002D 0x3100002D 0x3100002D 0x3100002D 0x3100002D 0x3100002D 0x3100002D 0x3100002D 0x3100002D 0x3100002D 0x3100002D 0x3100002D 0x3100002D 0x3100002D 0x3100002D 0x3100002D 0x3100002D 0x3100002D 0x3100002D 0x3100002D 0x31000020 0x2009002E 0x31000020 0x3100002D 0x3100002D 0x3100002D 0x3100002D 0x3100002D 0x3100002D 0x3100002D 0x3100002D 0x3100002D 0x3100002D 0x3100002D 0x31000020 0x20090048 0x20090048 0x02080050 10
	0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x02080060 0x20090070 0x20090070 0x2009002E 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x7308007C 0x20090070 0x2009002E 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x2009002E 0x20090071 0x7308007C 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x31000020 0x2009002E 0x20090071 0x20090071 0x02080027 10
	0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x02080060 0x20090070 0x20090070 0x2009002E 0x21000020 0x2100005F 0x2100005F 0x2100005F 0x2100005F 0x2100005F 0x2100005F 0x2100005F 0x2100005F 0x21000020 0x7308007C 0x02080060 0x20090070 0x2009002E 0x21000020 0x2100005F 0x2100005F 0x2100005F 0x2100005F 0x2100005F 0x2100005F 0x2100005F 0x2100005F 0x2100005F 0x2100005F 0x2100005F 0x2100005F 0x2100005F 0x2100005F 0x2100005F 0x2100005F 0x21000020 0x2009002E 0x20090071 0x02080027 0x7308007C 0x21000020 0x2100005F 0x2100005F 0x2100005F 0x2100005F 0x2100005F 0x2100005F 0x2100005F 0x2100005F 0x21000020 0x2009002E 0x20090071 0x20090071 0x02080027 10
	0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x02080060 0x20090062 0x20090062 0x2009002E 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x7308007C 0x00000020 0x02080060 0x20090062 0x2009002E 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x2009002E 0x20090064 0x02080027 0x00000020 0x7308007C 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x2009002E 0x20090064 0x20090064 0x02080027 10
	0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x02080060 0x20090071 0x20090071 0x20090062 0x2009006F 0x2009002E 0x2009002E 0x2009002E 0x21000020 0x21000020 0x21000020 0x7308007C 0x00000020 0x00000020 0x00000020 0x02080060 0x20090071 0x2009002E 0x2009002E 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x21000020 0x2009002E 0x2009002E 0x20090070 0x02080027 0x00000020 0x00000020 0x00000020 0x7308007C 0x21000020 0x21000020 0x21000020 0x2009002C 0x2009002E 0x2009002E 0x2009006F 0x20090064 0x20090070 0x20090070 0x02080027 10
	0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x02080060 0x20090071 0x20090071 0x20090071 0x20090071 0x20090071 0x2009003A 0x7308002B 0x7308002D 0x7308002B 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x0208005E 0x20090071 0x2009002E 0x2009002E 0x2009002E 0x2009002E 0x2009002E 0x2009002E 0x2009002E 0x2009002E 0x20090070 0x0208005E 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x7308002B 0x7308002D 0x7308002B 0x2009003A 0x20090070 0x20090070 0x20090070 0x20090070 0x20090070 0x0208005E 10
	0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x02080060 0x02080027 0x02080071 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x02080027 0x02080027 0x02080027 0x02080027 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x02080070 0x02080027 0x0208005E 10
	10
	10
	0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x00000020 0x0108004D 0x00000020 0x00000020 0x00000020 0x00000020 0x01080049 0x00000020 0x00000020 0x00000020 0x00000020 0x0108004E 0x00000020 0x00000020 0x00000020 0x00000020 0x01080049 0x00000020 0x00000020 0x00000020 0x00000020 0x01080041 0x00000020 0x00000020 0x00000020 0x00000020 0x01080054 10
	0