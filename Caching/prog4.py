import sys
import math

lines = [line.split()[0] for line in open(sys.argv[4]).read().split('\n') if line]

X = 2**int(sys.argv[1]) #Sets  = 8
Y = 2**int(sys.argv[2]) #Blocks per set  = 4
Z = 2**int(sys.argv[3]) #Addresses per block   = 16

logX, logY, logZ = int(sys.argv[1]),int(sys.argv[2]),int(sys.argv[3])

#these arrays all parse the binary into the groups needed for each line
sets = [0]*len(lines)
tags = [0]*len(lines)

# for each set there are blocks and each block stores information
cache = [[[0 for b in range(2)] for i in range(Y)] for j in range(X)]  # cache[sets][blocks]

#note blocks are where LRU is implemented each set needs its own LRU
def getSet(line):
    t = int(line)
    s = int(t/Z)%X
    #s = (t>>logZ) & ((2**(logX)) - 1)
    #print("Set: ", line, s)
    return s

def getTag(line):
    t = int(line)
    i = int(int(t/Z)/X)
    #print("tag: ", line, i)
    return i

#gets the smallest number in an array
def getLRU(i):
    LRU = -10000
    m = -10000
    for j in range(Y):
        if(cache[sets[i]][j][1] > m):
            LRU = j
            m = cache[sets[i]][j][1]
    #print(LRU)
    return LRU

def main():
    i = 0
    miss = 0
    location = 0
    hit = 0
    hits = 0
    misses = 0

    for line in lines:
        tags[i] = getTag(int(line,16))
        sets[i] = getSet(int(line,16))

        #blocks[i] = getIndex(int(line,16))

        for j in range(Y):
            #print(i, j)

            if (cache[sets[i]][j][0] == tags[i]):
                hit = 1
                location = j
                break
                #print(lines[i] , "hit",tags[i], sets[i],  cache)

        if hit == 1:
            hits += 1
            for z in range(Y):
                cache[sets[i]][z][1] += 1
            cache[sets[i]][location][1] = 0
        else:
            misses += 1
            LRU = getLRU(i) #gets the LRU for current set
            cache[sets[i]][LRU][0] = tags[i] #adds the tag to the block in the LRU location
            for z in range(Y):
                cache[sets[i]][z][1] += 1 #increments the LRU for each block
            cache[sets[i]][LRU][1] = 0 #resets the LRU for the block being used
            #print(lines[i] ,"miss",tags[i], sets[i],  cache)
            
        #print(i)
        hit = 0
        miss = 0
        i+=1
    print(sys.argv[4], "missed" ,misses,"/", len(lines))

main()
#print(lines)