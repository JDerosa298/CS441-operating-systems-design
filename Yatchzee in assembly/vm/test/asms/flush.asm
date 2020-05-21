.mode NOP_DELAY off

	MOVI r1 = 'B'  # 4
	MOVI r2 = 'n'  # 1
	MOVI r3 = 'p'  # 1
	MOVI r4 = 't'  # 1

	FLUSH  # 1
	DEC r1  # 3
	DEC r2  # 1
	DEC r3  # 1
	DEC r4  # 1

	MOVI r100 = 1  # 1

# cycles so far... should be 15

<<< ensure NOP_DELAY doesn't NOP after flush >>>

.mode NOP_DELAY on

	MOVI r1 = 'B'  # 1
	MOVI r2 = 'n'  # 1
	MOVI r3 = 'p'  # 1
	MOVI r4 = 't'  # 1

	FLUSH  # 1
	DEC r1  # 3
	DEC r2  # 1
	DEC r3  # 1
	DEC r4  # 1

	MOVI r100 = 2  # 1

# cycles so far... should be 27
