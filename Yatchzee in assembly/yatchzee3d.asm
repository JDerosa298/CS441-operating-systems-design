#Yatchzee3d by john derosa cs-441 spring 2019
.const TERM 0x4000
.const KEYB 0x4002
.const CR   0xd 
.const LF   0xa

.variable dice[5]


    MOVI r100=42
    MOVI r101=0xdeadbeef
    MOVI r1=0
    MOVI r2=0
    MOVI r3=0
    MOVI r4=0
    MOVI r5=0
    MOVI r6=0
    MOVI r11=0
    MOVI r22=1
    MOVI r33=2
    MOVI r44=3
    MOVI r55=4
    MOVI r66=5
    MOVI r105=5
    MOVI r133=3
    MOVI r223=0
    MOVI r225=0
    MOVI r155=0xa
    MOVI r125= 0x7c
    MOVI r226= 0x20
    MOVI r242=0x2f
    MOVI r188=2
    MOVI r123=0
    MOVI r221=0
    MOVI r244=4
    MOVI r222=2
    MOVI r131=0
    MOVI r237=0
    MOVI r238=0
    MOVI r239=0
    MOVI r99=0
    MOVI r97=0
    #MOVI r135 = !print_dice_top_mid
    #ADD r141=PC+2
    #BRA[!print_str]
    BRA[!inf]

!inf
    MOVI r135 = !ask_for_enter
    ADD r141=PC+2
    BRA[!print_str]
    LOAD r200 = [KEYB]
    MOVI r135 = !diceTT
    ADD r141=PC+2
    BRA[!print_str]
    STOR[TERM]=r155
    MOVI r135 = !diceTM
    ADD r141=PC+2
    BRA[!print_str]
    STOR[TERM]=r155
    MOVI r135 = !diceTB
    ADD r141=PC+2
    BRA[!print_str]
    STOR[TERM]=r155
    ADD r102 = PC + 2
    BRA[!rand]
    MOVI r135 = !diceBottom
    ADD r141=PC+2
    BRA[!print_str]
    STOR[TERM]=r155
    ADD r102 = PC + 2
    BRA[!hands]
    ADD r102 = PC+2
    BRA[!inf]

!ask_for_enter
    "Press the Enter key to roll the dice" 

!print_dice
   LOAD r144=[!dicePips]
   ADD r142 = PC + 2
   BRA[!Which_row]
   INC r223
   BRAL[!print_dice], r223<r133
   MOVI r223=0
   BRA[r141]

!Which_row
    LOAD r177=[r225+dice]
    SUB r177 = r177 - 49
    MULT r165 = r223 * 19
    MULT r233 = r177*3
    ADD r163= r165+49
    #STOR[TERM]=r163
    #STOR[TERM]=r155
    ADD r146 = r233 + r165
    LOAD r199 =[r146+!dicePips]
    STOR[TERM]=r125
    STOR[TERM]=r226
    STOR[TERM]=r226
    STOR[TERM]=r199
    ADD r146= r146+1
    LOAD r199 = [r146+!dicePips]
    STOR [TERM] = r199
    ADD r146= r146+1
    LOAD r199 = [r146+!dicePips]
    STOR [TERM] = r199
    STOR[TERM]=r226
    STOR[TERM]=r226
    STOR[TERM]=r125
    BRANE[!not_bottom],r223!=r188
    STOR[TERM]=r242
    STOR[TERM]=r226
    BRA[!skip]
    !not_bottom
        STOR[TERM]=r226
        STOR[TERM]=r125
    !skip
    INC r225
    BRAL[!Which_row], r225<r105
    BRANE[!endline],r223!=r188
    BRA[!skip2]
    !endline
        STOR[TERM]=r155
    !skip2
    MOVI r225=0
    MOVI r199 = 0
    MOVI r146=0
    BRA[r142]

   
!rand
    SHL r111 = r100 << 7
    SHR r100 = r100 >> 25
    ADD r100 = r100 + r101
    EXOR r100 = r100 ^ r111
    SHL r111 = r101 << 7
    SHR r101 = r101 >> 25
    EXOR r101 = r111 ^ (r101+0xdeadbeef)
    AND  r100 = r100 & 0x7fffffff
    MOD r103 = r100 % 6 
    ADD r103 = r103 + 49
    STOR [r123+dice]= r103
    LOAD r7 = [r123 + dice]
    #STOR[TERM]=r7
    INC r123
    BRAL [!rand], r123 < r105 
    ADD r141=PC+2
    BRA[!print_dice] 
    MOVI r135 = !CRLF
    ADD r141=PC+2
    BRA[!print_str]  
    MOVI r123 = 0
    BRA [r102] 

!hands
    LOAD r132 = [r123+dice]
    #STOR [TERM]=r132
    ### ISUSE HERE LIES WTH CONDITIONAL###
    ###STATEMENTS NOT BEING PROPERLY EXECUTED###  
    SUB r132 = r132-49
    ADD r234 = PC + 2
    BRAE[!is1], r132 == r11
    ADD r234 = PC + 2
    BRAE[!is2], r132 == r22
    ADD r234 = PC + 2
    BRAE[!is3], r132 == r33
    ADD r234 = PC + 2
    BRAE[!is4], r132 == r44
    ADD r234 = PC + 2
    BRAE[!is5], r132 == r55
    ADD r234 = PC + 2
    BRAE[!is6], r132 == r66
    INC r123
    BRAL[!hands], r123 < r105
    STOR[TERM]=r6
    ADD r234 = PC + 2
    BRA[!YACHTZEE]
    ADD r234 = PC + 2
    BRA[!largeStraight]
    ADD r234 = PC + 2
    BRA[!smallStraight]
    ADD r234 = PC + 2
    BRA[!FullHouse]
    ADD r234 = PC + 2
    BRA[!FourOfAKind]
    ADD r234 = PC + 2
    BRA[!ThreeOfAKind]
    ADD r234 = PC + 2
    BRA[!nothing]
    !handsEnd
        MOVI r1=0
        MOVI r2=0
        MOVI r3=0
        MOVI r4=0
        MOVI r5=0
        MOVI r6=0
        MOVI r123 = 0
        BRA[r102]

!is1
    INC r1
    BRA[r234]
!is2
    INC r2
    BRA[r234]
!is3
    INC r3
    BRA[r234]
!is4
    INC r4
    BRA[r234]
!is5
    INC r5
    BRA[r234]
!is6
    inc r6
    BRA[r234]


!nothing
    MOVI r135=!print_nothingOfIntrest
    ADD r141=PC+2
    BRA[!print_str]
    STOR[TERM]=r155
    STOR[TERM]=r155
    BRA[!handsEnd]

!ThreeOfAKind
    BRAE[!threeokFound], r1==r133
    BRAE[!threeokFound], r2==r133
    BRAE[!threeokFound], r3==r133
    BRAE[!threeokFound], r4==r133
    BRAE[!threeokFound], r5==r133
    BRAE[!threeokFound], r6==r133
    BRA[r234]
    !threeokFound
        MOVI r135=!print_3ok
        ADD r141=PC+2
        BRA[!print_str]
        STOR[TERM]=r155
        STOR[TERM]=r155
        BRA[!handsEnd]

!FourOfAKind
    BRAE[!fourokFound], r1==r244
    BRAE[!fourokFound], r2==r244
    BRAE[!fourokFound], r3==r244
    BRAE[!fourokFound], r4==r244
    BRAE[!fourokFound], r5==r244
    BRAE[!fourokFound], r6==r244
    BRA[r234]
    !fourokFound
        MOVI r135=!print_4ok
        ADD r141=PC+2
        BRA[!print_str]
        STOR[TERM]=r155
        STOR[TERM]=r155
        BRA[!handsEnd]

!FullHouse
    BRAE[!checkFor2],r1==r133
    BRAE[!checkFor2],r2==r133
    BRAE[!checkFor2],r3==r133
    BRAE[!checkFor2],r4==r133
    BRAE[!checkFor2],r5==r133
    BRAE[!checkFor2],r6==r133
    BRA[r234]
    !checkFor2
        BRAE[!fhFound],r1==r222
        BRAE[!fhFound],r2==r222
        BRAE[!fhFound],r3==r222
        BRAE[!fhFound],r4==r222
        BRAE[!fhFound],r5==r222
        BRAE[!fhFound],r6==r222
        BRA[r234]
    !fhFound
        MOVI r135=!print_FH
        ADD r141=PC+2
        BRA[!print_str]
        STOR[TERM]=r155
        STOR[TERM]=r155
        BRA[!handsEnd]

!smallStraight
    MOVI r238=0
    MOVI r237=0
    MOVI r239=0
    MOVI r12=0
    BRA[!oneToFour]
    !oneToFour
        ADD r12=r5+r6
        BRAG[!threeToSix], r12>r22
        ADD r197 = PC+2
        BRANE[!sscounter1],r1!=r131
        ADD r197 = PC+2
        BRANE[!sscounter1],r2!=r131
        ADD r197 = PC+2
        BRANE[!sscounter1],r3!=r131
        ADD r197 = PC+2
        BRANE[!sscounter1],r4!=r131
        BRAE[!ssFound],r237==r244
        BRANE[!threeToSix],r237!=r244
    !threeToSix
        ADD r12=r1+r2
        BRAG[!twoToFive], r12>r22
        ADD r197 = PC+2
        BRANE[!sscounter3],r3!=r131
        ADD r197 = PC+2
        BRANE[!sscounter3],r4!=r131
        ADD r197 = PC+2
        BRANE[!sscounter3],r5!=r131
        ADD r197 = PC+2
        BRANE[!sscounter3],r6!=r131
        BRAE[!ssFound],r239==r244
        BRANE[!twoToFive],r239!=r244
    !twoToFive
        ADD r12=r1+r6
        BRAG[r234], r12>r22
        ADD r197 = PC+2
        BRANE[!sscounter2],r2!=r131
        ADD r197 = PC+2
        BRANE[!sscounter2],r3!=r131
        ADD r197 = PC+2
        BRANE[!sscounter2],r4!=r131
        ADD r197 = PC+2
        BRANE[!sscounter2],r5!=r131
        BRAE[!ssFound],r238==r244
        BRANE[r234],r238!=r244
    !sscounter1
        inc r237
        BRA[r197]
    !sscounter2
        inc r238
        BRA[r197]
    !sscounter3
        inc r239
        BRA[r197]
    !ssFound
        MOVI r135=!print_ss
        ADD r141=PC+2
        BRA[!print_str]
        STOR[TERM]=r155
        STOR[TERM]=r155
        BRA[!handsEnd]

!largeStraight
    MOVI r99=0
    MOVI r97=0
    BRA[!onetofive]
    !onetofive
        BRAE[!twoToSix],r6==r22
        ADD r197=PC+2
        BRAE[!LScounter1],r1==r22
        ADD r197=PC+2
        BRAE[!LScounter1],r2==r22
        ADD r197=PC+2
        BRAE[!LScounter1],r3==r22
        ADD r197=PC+2
        BRAE[!LScounter1],r4==r22
        ADD r197=PC+2
        BRAE[!LScounter1],r5==r22
        BRAE[!lsFound],r99==r105
        BRANE[r234],r99!=r105
    !twoToSix
        BRAE[r234],r1==r22
        ADD r197=PC+2
        BRAE[!LScounter2],r2==r22
        ADD r197=PC+2
        BRAE[!LScounter2],r3==r22
        ADD r197=PC+2
        BRAE[!LScounter2],r4==r22
        ADD r197=PC+2
        BRAE[!LScounter2],r5==r22
        ADD r197=PC+2
        BRAE[!LScounter2],r6==r22
        BRAE[!lsFound],r97==r105
        BRANE[r234],r97!=r105
    !LScounter1
        INC r99
        BRA[r197]
    !LScounter2
        INC r97
        BRA[r197]
    !lsFound
        MOVI r135=!print_largeStraight
        ADD r141=PC+2
        BRA[!print_str]
        STOR[TERM]=r155
        STOR[TERM]=r155
        BRA[!handsEnd]

!YACHTZEE
    BRAE[!foundYatchzee],r1==r66
    BRAE[!foundYatchzee],r2==r66
    BRAE[!foundYatchzee],r3==r66
    BRAE[!foundYatchzee],r4==r66
    BRAE[!foundYatchzee],r5==r66
    BRAE[!foundYatchzee],r6==r66
    BRA[r234]
    !foundYatchzee
        MOVI r135=!print_yatchzee
        ADD r141=PC+2
        BRA[!print_str]
        STOR[TERM]=r155
        STOR[TERM]=r155
        BRA[!handsEnd]




!CRLF
    0xd 0xa 0

!print_str
    LOAD r142 = [r135]
    BRAE[!found_null], r142 == r0
    STOR[TERM] = r142
    INC r135
    BRA [!print_str]
!found_null
    BRA [r141]





!dicePips
    "     @@  @ @@ @@ @" 
    " @     @     @ @ @"
    "   @    @@ @@ @@ @"       

!diceTT
    "  _______    _______    _______    _______    _______"

!diceTM
    " /       /| /       /| /       /| /       /| /       /|"
!diceTB
    "+-------+ |+-------+ |+-------+ |+-------+ |+-------+ |"
!diceBottom
    "+-------+  +-------+  +-------+  +-------+  +-------+"
!print_yatchzee
    "YATCHZEE!!!!!"

!print_largeStraight
    "Large Straight"
!print_3ok
    "Three of a kind"
!print_nothingOfIntrest
    "Nothing of interest"
!print_4ok
    "Four of a kind"
!print_FH
    "Full House"
!print_ss
    "Small Straight"