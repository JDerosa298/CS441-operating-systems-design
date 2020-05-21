.mode NOP_DELAY off

.const WD_COMPARE			0x00001B1F
.const WD_CONTROL			0x00001B20
.const IFR_L 				0x00001F1C
.const IER_L				0x00001F1A
.const SYS_REG 				0x00001B1E
.const IVT_WD				0x00001ADF

.address 0x2000

!main

# Initialize the Vector Table
	MOVI r2 = !watchdog_interrupt
	STOR [IVT_WD] = r2

<<<
	You could speed this up by doing all the adds ahead of the stores....
	Set up the watchdog CPS (2) and compare of (10) so 2^2 = 4 and 10 prescaled ticks,
	so we should get int every 40 cycles
>>>
	MOVI r3 = 0x_1000_000A
	STOR [WD_COMPARE] = r3

# Turn on IER_L
	MOVI r4 = 0x_FFFF_FFFF
	STOR [IER_L] = r4

# Turn on interrupts globably
	MOVI r5 = 1
	STOR [SYS_REG] = r5

# Set up the watchdog Timer Control by enableing the watchdog timer
	MOVI r6 = 0x_8000_0000
	STOR [WD_CONTROL] = r6

# Jump to a NOP region of memory and ensure nothing is in the delay slot
	BRA [0x_0000_3000]
	NOP

!watchdog_interrupt

# Interrupt routine
	ADD r1 = r1 + 10
# disable watchdog timer
	STOR [WD_CONTROL] = r0
# Enable watchdog timer
	MOVI r3 = 0x_8000_0000
	STOR [WD_CONTROL] = r3
	IRET
