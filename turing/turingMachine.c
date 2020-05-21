#include <stdio.h>
//#include <vector.h>


//globals for later use
int headLocation=0;
int begin=0;
int end;
int LOT=40;

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
/*void right();//opc: 1
void halt();//opc: 2
void fail();//opc: 3
void draw();//opc: 4
void alpha();//opc: 5
void brae();//opc: 6
void brane();//opc: 7
void bra();//opc: 8
void cmp();//opc: 9
*/
////////////////////////

int main(int argc , char * argv[])
{
  //if(argc!=2)
  //{
    //printf("Invalid Input!\n");
  //}
  //else if (argc==2)
  //{
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


    /*for(int j=0; j<fileSize; j+=2)
    {
      printf("%x%x\n",Memory[j+1],Memory[j]);
    }*/
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
    /*for(int a=0 ; a<tapeSize;a++)
    {
      printf("%c",tapeStored[a]);
    }*/
    //printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
    //printf("%d %d\n",Memory[1]/16,Memory[3]/16);
    int hexer[fileSize/2][4];
//storing the individual instructions
//parsing each nibble into an array element
    for(int t=0;t<=fileSize;t+=2)
    {
      hexer[t/2][0]=Memory[t+1]/16;
      hexer[t/2][1]=Memory[t+1]%16;
      hexer[t/2][2]=Memory[t]/16;
      hexer[t/2][3]=Memory[t]%16;
    }
    /*for(int b=0 ; b < fileSize/2 ; b++)
    {
      for(int h=0;h<4;h++)
      {
        printf("hexer[%d][%d]: %d\n", b , h , hexer[b][h]);
      }
      printf("\n");
    }*/

    int iterator=0;
    char TBU[LOT];
    while(tapeStored[iterator]!='\n')
    {
      TBU[iterator]=tapeStored[iterator];
      iterator++;
    }

//printing out the tape that is being worked on currently
    for(int i=0;i<iterator;i++)
    {
      printf("%c",TBU[i]);
    }
    printf("\n");

left();
//putting the head in the correct sot when displayed
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





  //}
return 0;
}

void left()
{

  if (headLocation==begin)
  {
    begin--;
    if (begin<0)
    {
      begin = LOT-1;
    }

  }
  headLocation--;
  if(headLocation<0)
  {
    headLocation=LOT-1;
  }
}

void right()
{

  if (headLocation==begin)
  {
    end++;
    if (begin<0)
    {
      begin = LOT-1;
    }

  }
  headLocation++;
  if(headLocation<LOT-1)
  {
    headLocation=0;
  }
}
