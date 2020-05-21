#john derosa cs441 Branch predictors program
numCorrect = 0
import sys
lines = [line.split() for line in open(sys.argv[1]).read().split('\n') if line]
def fixedTrue():
    global numCorrect
    for row in lines:
        if row[1]=='@':
            numCorrect+=1
    print("Fixed:",numCorrect,"/",len(lines))


def staticFirst():
    pred=['@']*1024
    tags=[""]*1024
    global numCorrect
    numCorrect=0
    for row in lines:
        index = int(row[0],16) & ~(~0<<10)
        
        if (not(tags[index]==int(row[0],16) >>10)):
            tags[index]=int(row[0],16) >>10
            pred[index]='@'
            if row[1]=='@':
                numCorrect+=1
            else:
                pred[index]='.'
        else:
            if pred[index]==row[1]:
                numCorrect+=1

    print("Static:",numCorrect,"/",len(lines))

def bimodal():
    error=[1]*1024
    pred=['@']*1024
    tags=[""]*1024
    global numCorrect
    numCorrect=0
    for row in lines:
        index = int(row[0],16) & ~(~0<<10)
            
        if (not(tags[index]==int(row[0],16) >>10)):
            tags[index]=int(row[0],16) >>10
            pred[index]='@'
            error[index]=1
            if row[1]=='@':
                numCorrect+=1
                error[index]=0
            else:
                pred[index]='.'
                error[index]=0
        else:
            if pred[index]==row[1]:
                numCorrect+=1
                error[index]=0
            else:
                error[index]+=1
                if error[index]%2==0:
                    if pred[index]=='@':
                        pred[index]='.'
                    else:
                        pred[index]='@'
    print("Bimodal",numCorrect,"/",len(lines))

def bimodal2L():
    history=[3]*1024
    error=[[1,1,1,1]]*1024
    pred=[[0,1,0,1]]*1024
    tags=[""]*1024
    global numCorrect
    numCorrect=0
    for row in lines:
        index = int(row[0],16) & ~(~0<<10)
        if pred[index][history[index]]==1:
            Q='@'
        else:
            Q='.'
            
        if (not(tags[index]==int(row[0],16) >>10)):
            tags[index]=int(row[0],16) >>10
            pred[index]=[0,1,0,1]
            error[index]=[1,1,1,1]
            history[index]=3
            if row[1]=='@':
                numCorrect+=1
                error[index][history[index]]=0
            else:
                pred[index][history[index]]=0
                error[index][history[index]]=0
        else:
            if Q==row[1]:
                numCorrect+=1
                error[index][history[index]]=0
            else:
                error[index][history[index]]+=1
                if error[index][history[index]]%2==0:
                    if pred[index][history[index]]==1:
                        pred[index][history[index]]=0
                    else:
                        pred[index][history[index]]=1
        if row[1]=='@':
            history[index]<<=1
            history[index]|=1
        else:
            history[index]<<=1
        history[index]=(history[index] & 3)
    print("2-Layer:",numCorrect,"/",len(lines))


fixedTrue()
staticFirst()
bimodal()
bimodal2L()


