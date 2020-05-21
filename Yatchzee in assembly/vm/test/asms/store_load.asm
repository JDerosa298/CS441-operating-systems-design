!main
	MOVI r2 = 5
	STOR [0x0200] = r2
	NOP
	LOAD r1 = [0x0200]
