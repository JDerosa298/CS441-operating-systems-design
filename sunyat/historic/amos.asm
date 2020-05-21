jmp 2
var cnt 26
lod r0 1
mov r1 'a'
str FF r1
add r1 1
sub r0 1
cmp r0 0
jgr 4
mov r3 0D
mov r4 0A
psh r3
psh r4
pop r6
pop r5
str FF r5
str FF r6
lod r7 FE
add r7 3
str FF r7
ret
