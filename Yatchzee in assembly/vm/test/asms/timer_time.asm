.mode NOP_DELAY off

.const TIMER_CONTROL      0x00001B21
.const TIMER0_COMPARE     0x00001B22
.const TIMER_IO           0x00001B2A
.const IER_L              0x00001F1A
.const SYS_REG            0x00001B1E
.const IVT_TIMER0         0x00001AE0
.const T0_SETUP           20
.const TIMER0_COMPARE_VAL 10

<<<
	A timer control of 0x12 results in the timer going off once, then again in your
	routine when you re-enable it, which causes you to disable it again (right after
	your routine enabled it). Becuase of this, you stay in a disabled state. Also,
	when it goes off, you are in a delay slot for a branch which causes the
	interrupt to return to the delay slot instruction instead of starting the branch
	again, and my while (1) loop falls apart
>>>

.address 0x3800

!interrupt_timer0

#Enable the timer
	MOVI r3 = T0_SETUP
	STOR [TIMER_CONTROL] = r3
	IRET

.address 0x2000

!main

#Initialize the Vector Table
	MOVI r2 = !interrupt_timer0
	STOR [IVT_TIMER0] = r2

#Set up the timer compare
	MOVI r4 = TIMER0_COMPARE_VAL
	STOR [TIMER0_COMPARE] = r4

#Turn on IER_L
	MOVI r5 = 0xFFFFFFFF
	STOR [IER_L] = r5

#Turn on interrupts globably
	MOVI r6 = 1
	STOR [SYS_REG] = r6

#Set up the timer control
	MOVI r3 = T0_SETUP
	STOR [TIMER_CONTROL] = r3

!loop
	BRA [!loop]
