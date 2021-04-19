#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "main.h"

int main(int argc, char ** argv){
    char filename[100];
    int verboseModeOn = 0;
    printf("Enter Trace File: "); 
    scanf("%s", filename);

    // Grab the size of cache
    unsigned long cacheSize;
    printf("Enter cache size in KB: "); 
    scanf("%lu", &cacheSize);
    printf("the cache size provided is : %lu\n", cacheSize);
    long noOfblocks = (cacheSize* KILOBYTE)/BLOCKSIZE ;
    printf("The no of blocks is : %lu\n", noOfblocks);

    indexBits = (int) ceil( log2(noOfblocks) );
    printf("The number of index bits is %d\n", indexBits);

    tagBits = BLOCKSIZE - indexBits - OFFSET; 
    printf("The number of tag bits is %d\n", tagBits);


    // Set verbose mode 
    char verboseMode[100];
    printf("Verbose mode (y/n): "); 
    scanf("%s", verboseMode);
    if(strcmp(verboseMode, "y")== 0){
        verboseModeOn = 1;
    }
    
    // Initialize an array the size of cache specified above
    cacheContent cache[noOfblocks];

    // Run a loop through to preload the cache with all arr[cacheContent] all the dirty bits to 0
    for(int i =0 ; i< noOfblocks ; i++){
        cache[i].dirtyBit = 0;
        cache[i].validBit = 0;
    }

    printf("The cache tag is : %s\n",cache[].cacheTag);

    FILE * file = fopen( filename, "r" ); 
    char * fileContent = 0;

    //Total File Size
    long fileSize;
    
    if (file) {
        fseek(file, 0, SEEK_END);
        fileSize = ftell(file);
        
        //Set file pointer to start of chunk thread needs to read
        fseek (file, 0, SEEK_SET);

        //Allocate buffer to store file content
        fileContent = malloc (fileSize);

        if (fileContent)
        {
            //Load the buffer up with threads desginated portion of file
            fread (fileContent, 1, fileSize, file);
        }
        else{
            perror("Error");
        }

        fclose(file);

    }
    else{
        perror("Error");
    }

    //printf("%s",fileContent);
     //Tokenize file contents
    char **traces = tokenizeFileContents(fileContent, "\n");

    int index=1;

    // Statistics being collected  
    int writeCount = 0;
    int readCount = 0;
    int accessCount = 0;
    int readMisses = 0;
    int writeMisses = 0;
    int totalMisses = 0;
    int dirtyReadMisses = 0;
    int dirtyWriteMisses = 0;
    int bytesRead = 0;
    int bytesWritten = 0;
    int readTime = 0;
    int writeTime = 0;
    int missRate = 0;


    //Loop to iterate over all traces/traces
    while(1){
        if(traces[index]==NULL){
            break;
        }

        //Get trace
        char * trace=traces[index];
        char ** traceComponents = tokenizeFileContents(trace, " ");

        // printf("The memory address for the %s data access is : %s \n",traceComponents[1], traceComponents[2]);

        // Need a function that maps the address to the right index in the array
        char * memAdd = traceComponents[2];

        char * memAddBin = malloc(128);

        HexToBin(memAdd, memAddBin);

        int cacheIndex = getCacheIndex(memAddBin);

        int tagIndex = getTagIndex(memAddBin);

        // cacheContent cc = cache[cacheIndex];

        int validBit = 0;
        int dirtyBit = 0;
        char * cacheTag = 0; 
        int hitOrMiss = 0;
        char caseNumber [32];

        // if verbose mode on print out the cache index we figured out in the above function

        // Function to figure out cache tag
        // if verbose mode on print cache tag

        // The above function figures out what index in cache array we need to look at 
        
        // We process the cache content

        // If verbose mode print valid bit of cache at that index

        // If verbose mode on print already existing tag - if empty/invalid print 0

        // If verbose mode on print dirty bit 

        // If verbose mode print if its a hit or miss

        // If verbose mode on print out which case

        
        if (strcmp(traceComponents[1], "W")== 0) {
            writeCount++;
        }

        if (strcmp(traceComponents[1], "R")== 0) {
            readCount++;
        }

        if(verboseModeOn == 1){
            if(index < 20){
                // printf("The mem address in binary is : %s",memAddBin);
                printf("%d %X %X %d %s %d %s\n",index,cacheIndex,tagIndex,validBit,cacheTag, dirtyBit, caseNumber);
                // printf("The cache index is : %X\n", cacheIndex);
            }            
        }

        free(traceComponents);
        free(memAddBin);
        index++;
    }

    printf("The read count is : %d\n", readCount);
    printf("The write count is : %d\n",writeCount);    

    free(fileContent);
    free(traces);

    return 0;
}

int getCacheIndex(char * memAddBin){
    
    char * cacheIndexBin = malloc(32);

    // reverse iterate to get the index bits
    for (unsigned i = strlen(memAddBin) - OFFSET; i-- > strlen(memAddBin) - OFFSET - indexBits ; )
    {
        strncat(cacheIndexBin, &memAddBin[i], 1);
    }

    inplace_reverse(cacheIndexBin);

    char *a = cacheIndexBin;
    int cacheIndex = 0;
    do {
        int b = *a=='1'?1:0;
        cacheIndex = (cacheIndex<<1)|b;
        a++;
    } while (*a);

    free(cacheIndexBin);

    return cacheIndex;
}


int getTagIndex(char * memAddBin){
    
    char * tagIndexBin = malloc(32);

    // reverse iterate to get the index bits
    for (unsigned i = strlen(memAddBin) - OFFSET - indexBits; i-- > 0 ; )
    {
        strncat(tagIndexBin, &memAddBin[i], 1);
    }

    inplace_reverse(tagIndexBin);

    char *a = tagIndexBin;
    int tagIndex = 0;
    do {
        int b = *a=='1'?1:0;
        tagIndex = (tagIndex<<1)|b;
        a++;
    } while (*a);

    free(tagIndexBin);

    return tagIndex;
}



/**
 *
 * @param fileContent
 * @return
 */
char **tokenizeFileContents(char *fileContent , char * delimeters)
{
    int bufferSize = TRACES_BUFFER, position = 0;
    char **traces = malloc(bufferSize * sizeof(char*));
    char *token;
    char *savePtr;

    if (!traces) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while ((token = strtok_r(fileContent, delimeters,&savePtr))) {
        traces[position] = token;
        position++;

        if (position >= bufferSize) {
            bufferSize += TRACES_BUFFER;
            traces = realloc(traces, bufferSize * sizeof(char*));
            if (!traces) {
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        fileContent=savePtr;
    }

    traces[position] = NULL;

    return traces;
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
