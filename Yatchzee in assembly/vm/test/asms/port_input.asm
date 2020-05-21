.const PORT_A_DIR 0x00001F16
.const PORT_B_DIR 0x00001F17
.const PORT_C_DIR 0x00001F18
.const PORT_D_DIR 0x00001F19

.const PORT_A 0x00001B1A
.const PORT_B 0x00001B1B
.const PORT_C 0x00001B1C
.const PORT_D 0x00001B1D

!main

	movi r10 = 0b_1111_1111_1111_1111

	stor [PORT_A_DIR] = r10
	stor [PORT_B_DIR] = r10
	stor [PORT_C_DIR] = r10
	stor [PORT_D_DIR] = r10

	load r1 = [PORT_A]
	load r2 = [PORT_B]
	load r3 = [PORT_C]
	load r4 = [PORT_D]

!inf
	bra [!inf]
