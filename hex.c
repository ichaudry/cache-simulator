#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "main.h"

void HexToBin(char* hexdec, char * memAddBin);

// reverse the given null-terminated string in place
void inplace_reverse(char * str)
{
  if (str)
  {
    char * end = str + strlen(str) - 1;

    // swap the values in the two given variables
    // XXX: fails when a and b refer to same memory location
#   define XOR_SWAP(a,b) do\
    {\
      a ^= b;\
      b ^= a;\
      a ^= b;\
    } while (0)

    // walk inwards from both ends of the string, 
    // swapping until we get to the middle
    while (str < end)
    {
      XOR_SWAP(*str, *end);
      str++;
      end--;
    }
#   undef XOR_SWAP
  }
}


int main(int argc, char ** argv){
    char * memAdd = "0x80c06b0";

    char * memAddBin = malloc(128);
    HexToBin(memAdd, memAddBin);

    printf("The mem add to bin is %s and the size is %ld\n",memAddBin, strlen(memAddBin));

   

    char * cacheIndexBin = malloc(32);

    for (unsigned i = strlen(memAddBin) - 5 ; i-- >  strlen(memAddBin) - 5 - 10; )
    {
        printf("The char at this index is : %c\n",memAddBin[i]);
        strncat(cacheIndexBin, &memAddBin[i], 1);
    }

    printf("The log result is : %d\n",(int) ceil(log2(640)));
    printf("The cache index in binary is : %s\n",cacheIndexBin);

    inplace_reverse(cacheIndexBin);

    char *a = cacheIndexBin;
    int num = 0;
    do {
        int b = *a=='1'?1:0;
        num = (num<<1)|b;
        a++;
    } while (*a);
    printf("%X\n", num);

    

    free(memAddBin);
    free(cacheIndexBin);

    return 0;
}


// function to convert Hexadecimal to Binary Number
void HexToBin(char* hexdec, char * memAddBin)
{
 
    long int i = 2;
 
    while (hexdec[i]) {
 
        switch (hexdec[i]) {
        case '0':
            strcat(memAddBin, "0000");
            break;
        case '1':
            strcat(memAddBin, "0001");
            break;
        case '2':
            strcat(memAddBin, "0010");
            break;
        case '3':
            strcat(memAddBin, "0011");
            break;
        case '4':
            strcat(memAddBin, "0100");
            break;
        case '5':
            strcat(memAddBin, "0101");
            break;
        case '6':
            strcat(memAddBin, "0110");
            break;
        case '7':
            strcat(memAddBin, "0111");
            break;
        case '8':
            strcat(memAddBin, "1000");
            break;
        case '9':
            strcat(memAddBin, "1001");
            break;
        case 'A':
        case 'a':
            strcat(memAddBin, "1010");
            break;
        case 'B':
        case 'b':
            strcat(memAddBin, "1011");
            break;
        case 'C':
        case 'c':
            strcat(memAddBin, "1100");
            break;
        case 'D':
        case 'd':
            strcat(memAddBin, "1101");
            break;
        case 'E':
        case 'e':
            strcat(memAddBin, "1110");
            break;
        case 'F':
        case 'f':
            strcat(memAddBin, "1111");
            break;
        default:
            printf("\nInvalid hexadecimal digit %c",
                   hexdec[i]);
        }
        i++;
    }
  
}