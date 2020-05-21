#the correct numbers should be
#
import sys
import math

#number of direct mapped sets
X=2**int(sys.argv[1],10)
#number of blocks per set
Y=2**int(sys.argv[2],10)
#number of addresses cached in each block
Z=2**int(sys.argv[3],10)

totalSets= X
totalBlocks=X*Y
totalAddr=totalBlocks*Z

addrPer=Z
addrCnt=0
while(addrPer>1):
    addrPer//=2
    addrCnt+=1
print(addrCnt)

numSets=X
setCnt=0
while(numSets>1):
    numSets//=2
    setCnt+=1
print(setCnt)


sets = [0]*totalSets
blocks=[totalSets][Y]
addr=[totalBlocks][Z]

'''
print(X,Y,Z)
print(totalSets,totalBlocks,totalAddr)
print (addr)
'''
lines=[line.split() for line in open(sys.argv[4]).read().split('\n') if line]

#print(lines)
row=0
numLines=0
binary=[0]*len(lines)
for row in range (len(lines)):
    currAddr=int(lines[row][0],16)
    binAddr=bin(addrCnt)
    #print(currAddr)
    setNum=currAddr & ~(~0 <<setCnt+addrCnt)>>addrCnt
    #index = currAddr & ~(~0 << addrCnt)
    tagNum =(currAddr>>(setCnt+addrCnt))
    print(tagNum,setNum)#, index)
    #print (lines[row][0])
    numLines+=1
    #for i in range(totalSets):
        #if (setNum==i):

print("all done")