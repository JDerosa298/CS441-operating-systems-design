<<<
 	This is an example assembly file for the new and improved
 	MiniAT assembler, "mash".  Triple side-carrots are used to
 	form block comments.
>>>



<<<
	".mode" dircetives can appear throughout the source to temporarily change the mode
	used by the assembler for a particular section of code.

	NOP_DELAY on|off
		- default : on
		- pipeline slots following BRAx, IRET, and INT instructions are filled with NOPs
>>>
.mode NOP_DELAY off



<<<
	".const" (or ".constant"s) cannot have the same identifier as any variable.
>>>
.constant BIN_EXAMPLE 0b_1101_0011_0000_1110   # you could use ".const", as well
.constant HEX_EXAMPLE 0x_FACE                  # 0xFACE or 0x_F_ACE, 0xFA___C_E or 0x_F_ACE too
.constant DEC_EXAMPLE -1024                    # -1_024 if you like (sign only supported for decimal)
.constant OCT_EXAMPLE 0c_437



<<<
	Preamble variables declared with ".variable" or ".var" are automatically
	placed starting at the beginning of RAM (0x0200) Variables may not have
	the same identifier as any constant.  Variables declared after the first
	.address directive, label, or line of code are placed relative to the
	address of the directive/label/code and could end up in Flash and have
	slower access times if you're not careful.
>>>
.variable my_var # 0x0200
.variable zero_array[10]     # 0x0201 through 0x020A, zero_array[0] is 0x0200
.variable offset_array[-2,2] # 0x020B through 0x020F, offset_array[0] is 0x020D



##################################
# ...so ends the preamble
# ...thus beginith the meat
##################################



<<<
	".address" directives tell mash the follwing code, labels, and
	variable declarations begin at the given address.  Every program
	starts at 0x2000 by default, so you do NOT need to have any .address
	directives, but it is common for experienced programmers to use
	one even for the default.
>>>
.address 0x2000  # this is the beginning of FLASH... the MiniAT boot address

	###############################
	### ARITHMETIC INSTRUCTIONS ###
	###############################

	ADD r6 = r4 + (r5 + 104)
	ADD r6 = r4 + r5
	ADD r6 = r4 + 104

	SUB r3 = r8 - (r0 + 'A')
	SUB r3 = r8 - r0
	SUB r3 = r8 - 'A'

	MULT r13 = r7 * (r2 + 30)
	MULT r13 = r7 * r2
	MULT r13 = r7 * 30

	DIV r8 = r5 / (r7 + 14)
	DIV r8 = r5 / r7
	DIV r8 = r5 / 14

	MOD r10 = r20 % (r30 - 17)
	MOD r10 = r20 % r30
	MOD r10 = r20 % -17

	##########################
	### LOGIC INSTRUCTIONS ###
	##########################

	AND r2 = r3 & (r4 + 0x98765432)
	AND r2 = r3 & r4
	AND r2 = r3 & 0x98765432

	OR r5 = r6 | (r7 + 0xBADCC0DE)
	OR r5 = r6 | r7
	OR r5 = r6 | 0xBADCC0DE

	EXOR r8 = r9 ^ (r10 + 0xFEEDCAFE)
	EXOR r8 = r9 ^ r10
	EXOR r8 = r9 ^ 0xFEEDCAFE

	SHL r11 = r12 << (r13 + 5)
	SHL r11 = r12 << r13
	SHL r11 = r12 << 5

	SHR r14 = r15 >> (r1 + 2)
	SHR r14 = r15 >> r1
	SHR r14 = r15 >> 2

	#########################
	### DATA INSTRUCTIONS ###
	#########################

	LOAD r2 = [r3 + 0x2040]
	LOAD r2 = [r3]
	LOAD r2 = [0x2040]

	STOR [r4 + 0x123] = r5
	STOR [r4] = r5
	STOR [0x123] = r5

	RLOAD r2 = [r3 + 4]
	RLOAD r2 = [r3]
	RLOAD r2 = [4]

	RSTOR [r4 + 70] = r6
	RSTOR [r4] = r6
	RSTOR [70] = r6

<<<
	The following are special register mneumonics:
	"RSP" == r253, the register-stack pointer
	"SP"  == r254, the stack pointer
	"PC"  == r255, the program counter
>>>

.address 0x2250
<<<
	Labels are equivalent to the immediate address of whatever the next
	line of code would be, if one existed in the same address block.
>>>
!label1 # same as 0x2250

	############################
	### CONTROL INSTRUCTIONS ###
	############################

	BRAE [r2 + 0x1ADA], r3 == r4
	BRAE [r2], r3 == r4
	BRAE [0x1ADA], r3 == r4

	BRANE [r2 + 0x1ADA], r3 != r4
	BRANE [r2], r3 != r4
	BRANE [0x1ADA], r3 != r4

	BRAL [r11 + 0x2000], r12 < r13
	BRAL [r11], r12 < r13
	BRAL [0x2000], r12 < r13

	BRALE [r14 + 0x1BAA], r15 <= r1
	BRALE [r14], r15 <= r1
	BRALE [0x1BAA], r15 <= r1

	BRAG [r11 + 0x2000], r12 > r13
	BRAG [r11], r12 > r13
	BRAG [0x2000], r12 > r13

	BRAGE [r14 + 0x1BAA], r15 >= r1
	BRAGE [r14], r15 >= r1
	BRAGE [0x1BAA], r15 >= r1



<<<
	Each of the previous lines are 2 words w/ 6 words of NOP padding because of the NOP_DELAY
	mode set in the preamble above.

	If NOP_DELAY mode is off, each of the previous lines are only 2 words
>>>
!label2
	INT r9 + 1
	INT r9
	INT 1
	IRET
	BRAE [!label1], r0 == r0
	BRAE [r5], r0 == r0
	BRAE [r3 + !label2], r0 == r0

# The follwing two examples hint the branch predictor as to the expected conditon
	BRAE [!label1 : T], r2 == r5
	BRAE [r8 + 0x4321 : T], r2 == r5
	BRAE [r8 : F], r2 == r5
	BRAE [0x4321 : T], r2 == r5



### PSEUDOINSTRUCTIONS ###
	NEG r4
	INV r7
	INC r9
	DEC r15
	MOVR r5 = r6
	MOVI r3 = 17
	BRA [!label2]
	BRA [r2 + 0x2300]
	BRA [r2]
	BRA [0x2300]
	NOP
	FLUSH



<<<
	Predicating an instruction is done by leading the line with two
	underscores.  This visually calls attention to the line(s) quite
	harshly to ensure they are impossible to mistake for non-
	predicated instructions.  This style also gives the visual
	sensation of code tabbed within a conditional scope in some high
	level language to reinforce (through cognitive reflex) the line
	executes conditionally.
>>>
	SUB r1 = r15 - (r1 + 3)
	__INC r1   # ensure r1 is never 0
	DIV r4 = r5 / r1


<<<
	Raw values allow the writer to drop raw decimal, hex, octal, or binary words into
	an address block along with labels to locate the values.  These could effectively be
	code or (un)initialized data.

	Raw values also support single character data enclosed in 's.  Currently no support exists
	for the inclusion of the sigle quote (or other special characters) as the character data.

	Double-quoted strings may also be used and become NULL terminated.

	Packed strings are quoted (enclosed in 's).  Mash stores each consecutive 4 character
	sequence in a word with the earliest character from the 4 stored in the lowest order
	byte of the word.  Packed strings are NULL terminated w/ the NULL packed as part of the
	string, as well.
>>>

.address 0x2500

!some_characters
	'A'
	'a'
	'@'

!string1
	"abcdefg"

!string2
	"HIJKLMNOP"

!packed_string
	'MiniAT is cool'

!other_raw_stuff
	0b_00000000000000000000000000000001
	0b_00000000000000000000000000000010
	0b_00000000000000000000000000000100
	0b_00000000000000000000000000001000

	0b_00000000000000000000000000010000
	0b_00000000000000000000000000100000
	0b_00000000000000000000000001000000
	0b_00000000000000000000000010000000

	0b_00000000000000000000000100000000
	0b_00000000000000000000001000000000
	0b_00000000000000000000010000000000
	0b_00000000000000000000100000000000

	0b_00000000000000000001000000000000
	0b_00000000000000000010000000000000
	0b_00000000000000000100000000000000
	0b_00000000000000001000000000000000

	0b_00000000000000010000000000000000
	0b_00000000000000100000000000000000
	0b_00000000000001000000000000000000
	0b_00000000000010000000000000000000

	0b_00000000000100000000000000000000
	0b_00000000001000000000000000000000
	0b_00000000010000000000000000000000
	0b_00000000100000000000000000000000

	0b_00000001000000000000000000000000
	0b_00000010000000000000000000000000
	0b_00000100000000000000000000000000
	0b_00001000000000000000000000000000

	0b_00010000000000000000000000000000
	0b_00100000000000000000000000000000
	0b_01000000000000000000000000000000
	0b_10000000000000000000000000000000

<<<
	Raw values can also be defined, one after another on the same line
	so long as white space seperates the values.
>>>

!primes
	2 3 5 7 11 13 17 19 23 29
