<<<
	Interrupt a low-32 priority interrupt with a higher priority high-32
	interrupt.  The intent is to determine whether a suspected bug exists
	and, if so, determine when it has been removed.
>>>

.const IVT_XINT0  0x1AFA  # the low priority INT 32
.const INT_XINT0_MASK  0x00000001
.const IVT_TIMER0 0x1AE0  # the high priority INT 6
.const INT_TIMER0_MASK 0x00000040

.const IER_A           0x00001F1A
.const IER_B           0x00001F1B
.const IFR_A           0x00001F1C
.const IFR_B           0x00001F1D
.const SYS_REG         0x00001B1E
.const SYS_REG_IE_MASK 0x00000001

.address 0x2000

!main

# Initialize the Vector Table
	MOVI r1 = !int_timer0
	STOR [IVT_TIMER0] = r1
	MOVI r1 = !int_xint0
	STOR [IVT_XINT0] = r1

# Enable individual interrupts
	MOVI r1 = 0xFFFFFFFF
	STOR [IER_A] = r1
	STOR [IER_B] = r1

# Enable interrupts globably
	MOVI r1 = SYS_REG_IE_MASK
	STOR [SYS_REG] = r1

	MOVI r5 = 1

# Force low priority int
	MOVI r1 = INT_XINT0_MASK
	STOR [IFR_B] = r1

	MOVI r5 = 5

!inf
	BRA [!inf]

!int_xint0
	MOVI r5 = 2

# Force high priority int
	MOVI r1 = INT_TIMER0_MASK
	STOR [IFR_A] = r1

	MOVI r5 = 4

	IRET

!int_timer0
	MOVI r5 = 3

	IRET
