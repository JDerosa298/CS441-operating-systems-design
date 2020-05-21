#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


void left();
void right();
void halt();
void fail();
void draw(char tape[], int opc[][4]);
//void alpha(int opc[][4], char alphabet[]);
void brae(int opc[][4]);
void brane(int opc[][4]);
void bra(int opc[][4]);
void cmp();
void resetAlpha(char alphabet[]);
void turing(char tape[], int opc[][4], char alphabet[]);
void printTape(char tape[]);



//Global Variables
int headPosition = 0;
int lengthOfTape = 200;
int tapeStart = 0;
int tapeEnd = 1000; 
int step = 0;
int cmpResult = -1;
int end = 0;
int totalMoves = 0;
int totalInstructions = 0;
int moves = 0;
int instructions = 0; 

int main(int argc, char * argv[])
{
    int a = 0;
    char alphabet[1000];
    char tape[lengthOfTape];
    resetAlpha(alphabet);
    for(int i = 0; i<lengthOfTape; i++)
    {
        tape[i] = '\0';
    }


//gets the length of the bin file
    FILE *size = fopen(argv[1], "rb");
    fseek(size, 0, SEEK_END);
    int lengthOfFile = ftell(size);
    fclose(size);

//gets the length of the tape file and stores the tape
    FILE *t = fopen(argv[2], "r");
    fseek(size, 0, SEEK_END);
    int lengthOfTapeFile = ftell(t);
    char tapeFile[lengthOfTapeFile];
    rewind(t);
    fread(tapeFile,sizeof(tapeFile),1,t);
    fclose(t);

//put each line of the tape in its own array

    //left();

    unsigned char buffer[lengthOfFile];
    //unsigned char tape[lengthOfTape];
    FILE *ptr;
    ptr = fopen(argv[1],"rb");  // r for read, b for binary
    fread(buffer,sizeof(buffer),1,ptr); // read 10 bytes to our buffer
    fclose(ptr);

    //extract opcode
    int opc[lengthOfFile/2][4];
    for(int i = 0; i<lengthOfFile; i+=2)
    {
        opc[i/2][0] = buffer[i+1]/16;
        opc[i/2][1] = buffer[i+1]%16;
        opc[i/2][2] = buffer[i]/16;
        opc[i/2][3] = buffer[i]%16;
    }

//print length of file
    //printf("%d\n", lengthOfTapeFile);
    //printf("head pos = %d , start = %d\n", headPosition, tapeStart);
    //printf("head pos = %d , start = %d\n", headPosition, tapeStart);

//print the tape and head
    //printTape(tape);
//fill array with b's
    /*
    for(int i = 0; i<12; i++)
    {
        draw(tape, opc);
        right();
    }
    printTape(tape);
    */

//prints the whole tape file
    /*
    for(int i = 0; i<lengthOfTapeFile;i++)
    {
        printf("%c", tapeFile[i]);
    }
    */

//print instructions and opcode
    /*
    printf("\n");
    
    for(int i = 0; i<lengthOfFile; i+=2)
    {
        printf("%2x %2x  ", buffer[i+1], buffer[i]); // prints a series of bytes
        printf("opc[%d][0] = %d,  opc1 = %d,  opc2 = %d,  opc3 = %d\n",i/2,opc[i/2][0],opc[i/2][1],opc[i/2][2],opc[i/2][3]);
    }
    */
    int it = 0;
    while(it<lengthOfTapeFile)
    {
        int j = 0;
        while(tapeFile[it]!= '\n' && it!=lengthOfTapeFile)
        {
            tape[j] = tapeFile[it];
            //printf("%c", (char)tape[j]);
            it++;
            j++;
            
        }
        printf("\n");
        tapeEnd = j;
        it++;
        turing(tape,opc,alphabet);
        
    }


    //printTape(tape);
    printf("\n");
    printf("Totals across all tapes...\n");
    printf("       moves: %d\n", totalMoves);
    printf("instructions: %d\n", totalInstructions);
    return 0;
}


//function definitions
void left()
{
    instructions++;
    if(headPosition == tapeStart)
    {
        tapeStart-=1;
        if(tapeStart<0)
            tapeStart = lengthOfTape - 1;
    }
    headPosition -= 1;
    if(headPosition < 0)
        headPosition = lengthOfTape-1;

    moves++;
    step++;
    return;
}

void right()
{
    instructions++;
    if(headPosition == tapeEnd)
    {
        tapeEnd++;
        if (tapeEnd>lengthOfTape-1) {
            tapeEnd = 0;
        }
        
    }
    headPosition += 1;
    if(headPosition > lengthOfTape - 1)
        headPosition = 0;

    //if you hit begining double the array
    moves++;
    step++;
    return;
}

void halt()
{
    instructions++;
    printf("Halted after %d moves and %d instructions executed\n", moves, instructions);
    end = 1;
}

void fail()
{
    instructions++;
    printf("Failed after %d moves and %d instructions executed\n", moves, instructions);
    end = 1;
}

void draw(char tape[], int opc[][4])
{
    instructions++;
    tape[headPosition] = (char)(opc[step][2]*16 + opc[step][3]);
    step++;
}
/*
void alpha(int opc[][4], char alphabet[])
{
    alphabet[a] = (char)(opc[step][2]*16+opc[step][3]);
    instructions+=1;
    step+=1;
    a++;
}
*/
void resetAlpha(char alphabet[])
{
    for(int i = 0; i < 100; i++)
    {
        alphabet[i] = '\0';
    }
}

void brae(int opc[][4])
{
    instructions++;
    if(cmpResult == 1)
    {
        step = opc[step][1]*16*16 + opc[step][2]*16 + opc[step][3];
        cmpResult = -1; 
    }
    else
    {
        step++;
    }
    
    
}

void brane(int opc[][4])
{
    instructions++;
    if(cmpResult == 0)
    {
        step = opc[step][1]*16*16 + opc[step][2]*16 + opc[step][3];
        cmpResult = -1;
    }
    else
    {
        step++;
    }
}

void bra(int opc[][4])
{
    instructions++;
    step = opc[step][1]*16*16 + opc[step][2]*16 + opc[step][3];
}

void cmp(char tape[], int opc[][4])
{
    instructions++;
    cmpResult = (tape[headPosition] == (char)(opc[step][2]*16 + opc[step][3]));
    step++;
}

void turing(char tape[], int opc[][4], char alphabet[])
{
    int a = 0;
    int alphaFlag = 1;
    //Start of turing machine
    //printf("entering turing\n");
    //int inst
    while(!end)
    {
        //printf("in while");
        //usleep(100000000);
        //sleep(1);
        //delay(10);
        //printf("step = %d   opc = %d", step, opc[step][0] );
        switch (opc[step][0])
        {
            case 0:
                //printf("0");
                left();
                break;
            case 1:
                //printf("1");
                right();
                break;
            case 2:
                //printf("2");
                halt();
                printTape(tape);
                break;
            case 3:
                //printf("3");
                fail();
                break;
            case 4:
                //printf("4");
                draw(tape, opc);
                break;
            case 5:
                //printf("5");
                //alpha(opc,alphabet);
                for(int i = 0; i < a; i++)
                {
                    if(alphabet[i] == (char)(opc[step][2]*16+opc[step][3]))
                    {
                        alphaFlag = 0;
                    }
                }
                if(alphaFlag)
                {
                    alphabet[a] = (char)(opc[step][2]*16+opc[step][3]);
                    a++;
                    alphaFlag = 1;
                }
                instructions++;
                step++;
                break;
            case 6:
                //printf("6");
                brae(opc);
                break;
            case 7:
                //printf("7");
                brane(opc);
                break;
            case 8:
                //printf("8");
                bra(opc);
                break;
            case 9:
                //printf("9");
                cmp(tape, opc);
                break;

            default:
                printf("default");
                end = 1;
                break;
        }
        //instructions++;
        //printf("\n");
        //printTape(tape);
    }
    for(int i = 0; i<a;i++)
        {
            printf("%c", alphabet[i]);
        }
    printf("\n");
    resetAlpha(alphabet);
    a=0;
    for(int i = 0; i<10;i++)
        {
            printf("%c", alphabet[i]);
        }
    printf("\n");
    
    headPosition = 0;
    lengthOfTape = 200;
    tapeStart = 0; 
    step = 0;
    cmpResult = -1;
    end = 0;
    totalMoves += moves;
    totalInstructions += instructions;
    moves = 0;
    instructions = 0;

    //End of turing machine
    for(int i = 0; i<lengthOfTape; i++)
    {
        tape[i] = '\0';
    }
}

void printTape(char tape[])
{
    int j = tapeStart;
    for(int i = 0; i < (tapeEnd - tapeStart); i++)
    {
        if(tape[j]!='\0')
        printf("%c", tape[j]);
        j++;
        if(j>lengthOfTape)
            j = 0;
    }

    j = tapeStart;
    printf("\n");
    for(int i = 0; i < (tapeEnd - tapeStart); i++)
    {
        if(j == headPosition)
        {
            printf("^");
            break;
        }
        else if(tape[j]!='\0')
        {
            printf(" ");
        }

        //printf(" ");

        //printf("%d", j);
        j++;
        if(j>lengthOfTape-1)
            j=0;
    }
    printf("\n");
}
