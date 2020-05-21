.constant	CR	0xD
.constant	LF	0xA
.constant	TERM	0x4000
.constant	KEYB	0x4002

.variable num_str[10] #string for reverse number storage

#.mode nop_delay off

.addr 0x2000

     MOVI r100 = '='
     MOVI r101 = CR
     MOVI r102 = LF
     MOVI r200 = '0'
     MOVI r209 = '9'
!main
     MOVI r1 = 0
!next_char
     LOAD r2 = [KEYB]
     BRAE [!got_equals], r2 == r100
#checks if character is a digit
     BRAL [!not_digit], r2 < r200 #< '0'
     BRAG [!not_digit], r2 > r209 # > '9'
     MULT r1 = r1 * 10
     SUB  r3 = r2 - '0'
     ADD  r1 = r1 + r3
     BRA  [!next_char]
!not_digit
     BRA  [!next_char]

!got_equals
     MOVR r4 = r1

     MOVI r6 = 0
!calculate_dig
     BRAE [!done_calculate_dig], r4 == r0
     MOD  r5 = r4 % 10
     DIV  r4 = r4 / 10
     ADD  r5 = r5 + '0'
     STOR [r6 + num_str] = r5
     INC r6
     BRA  [!calculate_dig]

!done_calculate_dig

!print_dig
     DEC r6
     LOAD r7 = [r6 + num_str]
     STOR [TERM] = r7
     BRAG [!print_dig], r6 > r0

     STOR[TERM] = r101 #CR
     STOR[TERM] = r102 #LF

     LOAD r0 = [KEYB]
     LOAD r0 = [KEYB]
     BRA  [!main]