.constant TERM 0x4000

.address 0x2000

    MOVI r140 = !hello
    ADD  r141 = PC + 2
    BRA [!print_str]
    MOVI r140 = !CRLF
    ADD  r141 = PC + 2
    BRA [!print_str]

!inf
    BRA[!inf]

##########################################    
#r140: address of the string to print_str#
#r141: return address                    #
#r141: char loading                      #
##########################################
!print_str
    LOAD r142 = [r140]
    BRAE[!found_null], r142 == r0
    STOR[TERM] = r142
    INC r140
    BRA [!print_str]
!found_null
    BRA [r141]

######################################

!hello
    "Hello, world!"
    'Hello, world!'
    
!CRLF
    0xd 0xa 0  #CR LF NULL