using namespace std;
#include <stdio.h>
#include <vector>
#include <stdbool.h>
#include<unistd.h>
#include <algorithm>
//globals for later use
int headLocation=0;
int begining=0;
int moves = 0;
int instructions=0;
int currStep=0;
int compared=-1;
int moveACC=0;
int instructionsACC=0;
//sentinel for the loop running the actual machine
bool sent = false;
vector<char> TBU;
vector<char> alphabet;
//opc globals
const int leftOPC = 0;
const int rightOPC = 1;
const int haltOPC=2;
const int failOPC=3;
const int drawOPC=4;
const int alphaOPC=5;
const int braeOPC=6;
const int braneOPC=7;
const int braOPC=8;
const int cmpOPC=9;
//functions for each of the potential operations
void left();//opc: 0
void right();//opc: 1
void halt();//opc: 2
void fail();//opc: 3
void draw(vector<char> &tbu, int opc[][4]);//opc: 4
void alpha(int opc[][4], vector<char> &alphabet);//opc: 5
void brae(int opc[][4]);//opc: 6
void brane(int opc[][4]);//opc: 7
void bra(int opc[][4]);//opc: 8
void cmp(vector<char> &tbu, int opc[][4]);//opc: 9
///////////////////////////////////////////////////////////////////////////
int main(int argc , char * argv[])
{
//opening and storing the assembly code
    FILE *binary;
    binary=fopen(argv[1],"rb");
    fseek(binary, 0 , SEEK_END);
    int fileSize=ftell(binary);
    fseek(binary,0,SEEK_SET);
    unsigned int Memory[fileSize];
    for(int i=0;i<fileSize;i++)
    {
      char value;
      fread(&value,1,1,binary);
      Memory[i]=(unsigned char) value;
    }
    fclose(binary);
//opening and storing the tape
    FILE *tape;
    tape=fopen(argv[2],"r");
    fseek(tape,0,SEEK_END);
    int tapeSize=ftell(tape);
    fseek(tape,0,SEEK_SET);
    char tapeStored[tapeSize];
    for(int k=0;k<tapeSize;k++)
    {
        char nextTape;
        fread(&nextTape,1,1,tape);
        tapeStored[k] = nextTape;
    }
    fclose(tape);
//storing the individual instructions
//parsing each nibble into an array element
    int hexer[fileSize/2][4];
    for(int t=0;t<=fileSize;t+=2)
    {
      hexer[t/2][0]=Memory[t+1]/16;
      hexer[t/2][1]=Memory[t+1]%16;
      hexer[t/2][2]=Memory[t]/16;
      hexer[t/2][3]=Memory[t]%16;
    }
    int keepGoing=0;
    while(keepGoing<tapeSize)
    {// this is where the actual turring machine begins to run
    ////////////////////////
              while(tapeStored[keepGoing]!='\n'&& keepGoing!=tapeSize)
              {
                TBU.push_back(tapeStored[keepGoing]);
                keepGoing++;

              }
              if(TBU.empty())
              {
                  TBU.push_back(' ');
              }
              while(!sent)
              {//start of the while loop for the turing machine
              //  usleep(100000);
                //printf("current step %d\n",currStep);
                switch(hexer[currStep][0])
                {//switch case statement for each op code that is possible
                  case leftOPC:
                            left();
                            break;
                  case rightOPC:
                            right();
                            break;
                  case haltOPC:
                            //printf("h\n");
                            halt();
                            break;
                  case failOPC:
                            //printf("f\n");
                            fail();
                            break;
                  case drawOPC:
                            if(alphabet.end()==(find(alphabet.begin(),alphabet.begin(),(char)(hexer[currStep][2]*16 + hexer[currStep][3]))))
                            {
                              fail();
                            }
                            else
                            {
                            draw(TBU,hexer);
                            }
                            break;
                  case alphaOPC:
                            alpha(hexer,alphabet);
                            break;
                  case braeOPC:
                            brae(hexer);
                            break;
                  case braneOPC:
                            brane(hexer);
                            break;
                  case braOPC:
                            bra(hexer);
                            break;
                  case cmpOPC:
                            if(alphabet.end()==(find(alphabet.begin(),alphabet.begin(),(char)(hexer[currStep][2]*16 + hexer[currStep][3]))))
                            {
                              fail();
                            }
                            else if (alphabet.end()==(find(alphabet.begin(),alphabet.begin(),TBU[headLocation])))
                            {
                              fail();
                            }
                            else
                            {
                              cmp(TBU,hexer);
                            }

                            break;
                  default:
                          printf("this is the default");
                          break;
                }//end of the switch case for each opcode
              }//end of while loop for turing machine
              //printing out the tape that is being worked on currently
                  for(int i=0;i<TBU.size();i++)
                  {
                    if(TBU[i]=='\0')
                    printf(" ");
                    else
                    printf("%c",TBU[i]);

                  }
                  printf("\n");
              //putting the head in the correct spot when displayed
                  int spacer=0;
                  while(spacer<=headLocation)
                  {
                    if(spacer!=headLocation)
                    {
                      printf(" ");
                      spacer++;
                    }
                    else if(spacer==headLocation)
                    {
                      printf("^\n");
                      spacer++;
                    }
                  }
                // reinitializing the golbal variables for the turing machine
                headLocation=0;
                currStep=0;
                compared=-1;
                sent=false;
                moveACC+=moves;
                instructionsACC+=instructions;
                moves=0;
                instructions=0;
                //resetting the tape being used
                TBU.erase(TBU.begin(),TBU.end());
                //resetting the alphabet
                alphabet.erase(alphabet.begin(),alphabet.end());
                keepGoing++;
              }
      printf("Total moves:%d Total instructions %d\n",moveACC,instructionsACC);
return 0;
}

void left()
{
  instructions++;
    if (headLocation==0)
    {
      TBU.insert(TBU.begin(),'\0');
    }
    else
    {
      headLocation--;
    }
  moves++;
  currStep++;
}

void right()
{
  instructions++;
    if (headLocation==(int)(TBU.size()-1))
    {
      TBU.resize((int)(TBU.size()+1));
    }
  headLocation++;
  moves++;
  currStep++;
}

void halt()
{
  instructions++;
  printf("Halted after %d moves and %d instructions executed\n",moves,instructions);
  sent=true;
  currStep++;
}

void fail()
{
  instructions++;
  printf("Failed after %d moves and %d instructions executed\n",moves,instructions);
  sent=true;
  currStep++;
}

void draw(vector <char> &tbu, int opc[][4])
{
 instructions++;
 tbu[headLocation]= (char)(opc[currStep][2]*16 + opc[currStep][3]);
 currStep++;
}
void alpha(int opc[][4], vector<char> &alphabet)
{
  instructions++;
  alphabet.insert(alphabet.begin(),(char)(opc[currStep][2]*16 + opc[currStep][3]));
  currStep++;
}
void brae( int opc[][4])
{
  instructions++;
  if(compared==1)
  {
    currStep=opc[currStep][1]*16*16 + opc[currStep][2]*16 + opc[currStep][3];
    compared=-1;
  }
  else
  {
    currStep++;
  }
}
void brane( int opc[][4])
{
  instructions++;
  if(compared==0)
  {
    currStep=opc[currStep][1]*16*16 + opc[currStep][2]*16 + opc[currStep][3 ];
    compared=-1;
  }
  else
  {
    currStep++;
  }
}
void bra( int opc[][4])
{
 instructions++;
 currStep=opc[currStep][1]*16*16 + opc[currStep][2]*16 + opc[currStep][3];
}
void cmp(vector<char> &tbu, int opc[][4] )
{
   instructions++;
   compared=(tbu[headLocation] == (char)(opc[currStep][2] * 16 + opc[currStep][3]));
   currStep++;
}
