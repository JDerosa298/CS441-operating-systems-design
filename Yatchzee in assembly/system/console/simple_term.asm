.constant	CR	0xD
.constant	LF	0xA
.constant	TERM	0x4000
.constant	KEYB	0x4002

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
    MOVI r41 = CR
    MOVI r42 = LF
    STOR [TERM] = r41
    STOR [TERM] = r42

!echo_loop
    LOAD r1 = [KEYB] # Read from the KB
    STOR [TERM] = r1 # Write to the terminal
    BRA  [!echo_loop]

!string
    "MiniAT Echo Terminal, type a string hit enter and the MiniAT echo's it back!"

