.mode NOP_DELAY off

<<<
	Test1: unconditional branch -- should have 1 delay slot.
>>>

MOVI r1 = '1'
MOVI r2 = '5'
MOVI r3 = 'A'
MOVI r4 = 'D'
MOVI r5 = 0
MOVI r6 = 0
MOVI r7 = 0

BRA [!done_1]
ADD r1 = r1 + 1
ADD r2 = r2 + 1
ADD r3 = r3 + 1
ADD r4 = r4 + 1

!done_1
	ADD r5 = r5 + 1
	ADD r6 = r6 + 1
	ADD r7 = r7 + 1
	NOP
	NOP
	NOP
	NOP

MOVI r100 = 1

<<<
	Test2: Hint False (correctly) -- should fall through without branching at all.
>>>

MOVI r1 = '1'
MOVI r2 = '5'
MOVI r3 = 'A'
MOVI r4 = 'D'
MOVI r5 = 0
MOVI r6 = 0
MOVI r7 = 0

BRANE [!done_2 : F], r0 != r0
ADD r1 = r1 + 1
ADD r2 = r2 + 1
ADD r3 = r3 + 1
ADD r4 = r4 + 1

!done_2
	ADD r5 = r5 + 1
	ADD r6 = r6 + 1
	ADD r7 = r7 + 1
	NOP
	NOP
	NOP
	NOP

MOVI r100 = 2

<<<
	Test3: Hint False (incorrectly) -- should have a 3 cycle delay slot.
>>>

MOVI r1 = '1'
MOVI r2 = '5'
MOVI r3 = 'A'
MOVI r4 = 'D'
MOVI r5 = 0
MOVI r6 = 0
MOVI r7 = 0

BRAE [!done_3 : F], r0 == r0
ADD r1 = r1 + 1
ADD r2 = r2 + 1
ADD r3 = r3 + 1
ADD r4 = r4 + 1

!done_3
	ADD r5 = r5 + 1
	ADD r6 = r6 + 1
	ADD r7 = r7 + 1
	NOP
	NOP
	NOP
	NOP

MOVI r100 = 3

<<<
	Test4: Hint True (incorrectly) -- Flush should occur, then all registers should be incremented.
>>>

MOVI r1 = '1'
MOVI r2 = '5'
MOVI r3 = 'A'
MOVI r4 = 'D'
MOVI r5 = 0
MOVI r6 = 0
MOVI r7 = 0

BRANE [!done_4 : T], r0 != r0
ADD r1 = r1 + 1
ADD r2 = r2 + 1
ADD r3 = r3 + 1
ADD r4 = r4 + 1

!done_4
	ADD r5 = r5 + 1
	ADD r6 = r6 + 1
	ADD r7 = r7 + 1
	NOP
	NOP
	NOP
	NOP

MOVI r100 = 4

<<<
	Test5: Hint true (correctly) -- should have 1 delay slot.
>>>

MOVI r1 = '1'
MOVI r2 = '5'
MOVI r3 = 'A'
MOVI r4 = 'D'
MOVI r5 = 0
MOVI r6 = 0
MOVI r7 = 0

BRAE [!done_5 : T], r0 == r0
ADD r1 = r1 + 1
ADD r2 = r2 + 1
ADD r3 = r3 + 1
ADD r4 = r4 + 1

!done_5
	ADD r5 = r5 + 1
	ADD r6 = r6 + 1
	ADD r7 = r7 + 1
	NOP
	NOP
	NOP
	NOP

	MOVI r100 = 5