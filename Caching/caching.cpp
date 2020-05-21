//john derosa caching program CS441
//written in c++
//g++ target

#include <vector>
#include <iostream>
#include<cstdlib>
#include<math.h>
#include<fstream>

using namespace std;
int misses=0;

int main (int argc, char * argv[])
{   
    int X = pow(2, atoi(argv[1]));
    int Y = pow(2,atoi(argv[2]));
    int Z = pow(2,atoi(argv[3]));
    int totalSets=X;
    //cout<<totalSets<<endl;
    //int totalBlocks=X*Y;
    //int totalAddr=totalBlocks*Z;
    int sets[totalSets]={0};
    uint64_t blocks[totalSets][Y]={0};
    int blockCounter[totalSets][Y]={0};

for (int t=0; t<totalSets;t++)
{
    for(int v=0; v<Y; v++)
    {
        blockCounter[t][v]=0;
        blocks[t][v]=~0ULL;
    }

}
    //int addr[totalBlocks][Z]={0};

    int numSets=X;
    int setCnt=0;
    while(numSets>1)
    {
        numSets/=2;
        setCnt++;
    }
   // cout<<setCnt<<endl;
    int addrPer=Z;
    int addrCnt=0;
    while(addrPer>1)
    {
        addrPer/=2;
        addrCnt++;
    }
    //cout<<addrCnt<<endl;
    //cout<<setCnt<<endl;


    ifstream traF;
    traF.open(argv[4]);

    int lines = 0;
    while (!traF.eof())
    {
        char n = traF.get();
        if (n == '\n')
            lines++;
    }
    traF.clear();
    traF.seekg(0, traF.beg);
    vector<uint64_t> addresses(lines);
    char garbage;
    int i = 0;
    int numLines=0;
    uint64_t currAddr;
    while (traF >> hex >> currAddr >> garbage)
    {
        cout << hex << currAddr << "  ";
        int setNum=currAddr & ~(~0<<setCnt+addrCnt)>>addrCnt;
        uint64_t tagNum=(currAddr>>setCnt+addrCnt);
        int index=currAddr & ~(~0<<addrCnt);
        //cout<<tagNum<<" "<<setNum<<endl;
        numLines++;
        int setFound=0;
        for(int j=0;j<X&&setFound==0;j++)
        {
            if(setNum==j)
            {   
                setFound=1;
                int blockFound=0;
                for(int k=0;k<Y&&blockFound==0;k++)
                {
                    int index=0;
                    int currCounter=-1000,tempCounter=-1000;
                    int LRU=0;
                    while(index<Y)
                    {  
                        currCounter=blockCounter[j][index];
                        //cout<<currCounter<<endl;
                        if(currCounter>tempCounter)
                        {
                            LRU=index; 
                            tempCounter=currCounter;
                           
                        }

                        index++;
                    }

                    

                    if(blocks[j][k]==tagNum)
                    {
                        int f=0;
                        while(f<Y)
                        {
                            //cout<<blockCounter[j][f]<<endl;
                            blockCounter[j][f]+=1;
                            f++;
                        }
                        blockFound=1;
                        blockCounter[j][k]=0;
                    }
                    else if(blocks[j][k]==0)
                    {
                        int f=0;
                        while(f<Y)
                        {
                            //cout<<blockCounter[j][f]<<endl;
                            blockCounter[j][f]+=1;
                            f++;
                        }
                        blocks[j][k]=tagNum;
                        blockFound=1;
                        blockCounter[j][k]=0;
                    }
                    else if(k==(Y-1)&&!(blocks[j][k]==tagNum))
                    {
                        int f=0;
                        while(f<Y)
                        {
                            //cout<<blockCounter[j][f]<<endl;
                            blockCounter[j][f]+=1;
                            f++;
                        }
                        blocks[j][LRU]=tagNum;
                        blockCounter[j][LRU]=0;
                        misses+=1;
                        cout << "miss";
                    }
                   //cout<<blocks[j][k]<<endl;
                }
            }
            else if(j==(totalSets-1))
            {
               misses++;
               cout << "miss2";
            }
        }
        cout << endl;

    }
cout<<"MISSED: "<<int(misses)<<"/"<<int(numLines)<<endl;
return(EXIT_SUCCESS);
}