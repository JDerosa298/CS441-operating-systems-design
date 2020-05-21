.constant    TERM   0x4000
.constant    KEYB   0x4002
.constant    LAST   0x3FFF

.constant    HEX_AT  0x40
.constant    HEX_Z   0x5A
.constant    CR	     0xD
.constant    LF	     0xA

.address 0x2000

!main
    MOVI r40 = !string
    NOP

!prompt
    LOAD r20 = [r40]
    BRAE [!done : F], r20 == r0
    STOR [TERM] = r20
    ADD  r40 = r40 + 1
    BRA  [!prompt]

!done
    LOAD r1 = [LAST]
    BRANE [!put], r1 != r0

!initial
    MOVI r1 = HEX_AT

!put
    # Check that we are not goind past Z when we increment, if we do,
    # re-initalize LAST with the AT symbol
    MOVI r2 = HEX_Z
	BRAGE [!initial], r1 >= r2

    ADD r1 = r1 + 1
    STOR [LAST] = r1
    STOR [TERM] = r1

    MOVI r41 = CR
    MOVI r42 = LF
    STOR [TERM] = r41
    STOR [TERM] = r42

!echo_loop
    LOAD r1 = [KEYB] # Read from the KB
    STOR [TERM] = r1 # Write to the terminal
    BRA  [!echo_loop]

!string
    "MiniAT Echo Terminal, type a string hit enter and the MiniAT echo's it back! The next value output is the flash test char, it should be A-Z, incrementing each time you run: "
