#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "main2.h"

int printCount = 0;

int main(int argc, char ** argv){
    char filename[100];
    int verboseModeOn = 0;
    printf("Enter Trace File: "); 
    scanf("%s", filename);

    // Grab the size of cache
    unsigned long cacheSize;
    printf("Enter cache size in KB: "); 
    scanf("%lu", &cacheSize);
    // printf("the cache size provided is : %lu\n", cacheSize);
    // long noOfSetsnoOfblocks = (cacheSize* KILOBYTE)/BLOCKSIZE ;
    // printf("The no of blocks is : %lu\n", noOfSetsnoOfblocks);

    // Grab the size of cache
    int setAssociativity;
    printf("Enter set associativity: "); 
    scanf("%d", &setAssociativity);
    // printf("The set associativity is %d:\n",setAssociativity);

    long noOfSets = (cacheSize * KILOBYTE)/(setAssociativity * BLOCKSIZE);
    // printf("%lu\n", noOfSets);

    indexBits = (int) ceil( log2(noOfSets) );
    printf("The number of index bits is %d\n", indexBits);

    tagBits = BLOCKSIZE - indexBits - OFFSET; 
    printf("The number of tag bits is %d\n", tagBits);

    // Set verbose mode 
    char verboseMode[100];
    
    // Set verbose mode start and end
    int verboseStart, verboseEnd;
    
    printf("Verbose mode (y/n): "); 
    scanf("%s", verboseMode);
    if(strcmp(verboseMode, "y")== 0){
        verboseModeOn = 1;
        printf("Enter first and last reference to track: "); 
        scanf("%d %d", &verboseStart, &verboseEnd);        
    }

   
    

    // Initialize an array the size of cache specified above
    cacheContent cache[noOfSets][setAssociativity];

    // Run a loop through to preload the cache with all arr[cacheContent] all the dirty bits to 0
    for(int i =0 ; i< noOfSets ; i++){
        for(int j = 0; j< setAssociativity ; j++){
            cache[i][j].dirtyBit = 0;
            cache[i][j].validBit = 0;
            cache[i][j].lastUsed = 0;
        }
    }

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
    double missRate = 0;


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

        int noOfBytes; 
        
        sscanf(traceComponents[3],"%d",&noOfBytes);

        char * memAddBin = HexToBin(memAdd);

        int cacheIndex = getCacheIndex(memAddBin);

        int tagIndex = getTagIndex(memAddBin);
        int validBit;
        int dirtyBit;
        int cacheTag;
        int hitOrMiss = 0;
        char caseNumber [32];
        int lruBlock = 0;


        for(int j = 0; j< setAssociativity ; j++){
            cacheContent * cc = &cache[cacheIndex][j];

            validBit = cc->validBit;
            dirtyBit = cc->dirtyBit;
            cacheTag = cc->cacheTag;                      
            caseNumber [32];

            if(cc->lastUsed < cache[cacheIndex][lruBlock].lastUsed){
                lruBlock = j;
            }

            // case 1 
            if(cacheTag == tagIndex){
                hitOrMiss = 1;
                strcpy(caseNumber, "1");
                if (strcmp(traceComponents[1], "W")== 0) {
                    writeTime++;
                    cc->dirtyBit = 1;
                    cc->lastUsed = index;
                    break;
                    // bytesWritten = bytesWritten + noOfBytes;
                }
                else if (strcmp(traceComponents[1], "R")== 0) {
                    // bytesRead = bytesRead + noOfBytes;
                    readTime++;
                    cc->lastUsed = index;
                    break;
                }
            }
        }

     
        if(hitOrMiss == 0){
            cacheContent * cc = &cache[cacheIndex][lruBlock];
            if(dirtyBit == 0){
                strcpy(caseNumber, "2a");
                if (strcmp(traceComponents[1], "W")== 0) {
                    cc->cacheTag = tagIndex;
                    cc->lastUsed = index;
                    cc->dirtyBit = 1;
                    cc->validBit = 1;
                    writeTime = writeTime + (1 + MISS_PENALTY);
                    bytesRead = bytesRead + noOfBytes;
                    writeMisses++;
                }

                else if (strcmp(traceComponents[1], "R")== 0) {
                    cc->cacheTag = tagIndex;
                    cc->lastUsed = index;
                    cc->dirtyBit = 0;
                    cc->validBit = 1;
                    readTime = readTime + (1 + MISS_PENALTY);
                    bytesRead = bytesRead + noOfBytes;                    
                    readMisses++;
                }
            }
            else if(dirtyBit == 1){
                strcpy(caseNumber, "2b");
                if (strcmp(traceComponents[1], "W")== 0) {
                    cc->cacheTag = tagIndex;
                    cc->dirtyBit = 1;
                    cc->lastUsed = index;
                    writeTime = writeTime + (1 + (2 * MISS_PENALTY));
                    writeMisses++;
                    bytesWritten = bytesWritten + noOfBytes;
                    bytesRead = bytesRead + noOfBytes;
                    dirtyWriteMisses++;
                }
                
                else if (strcmp(traceComponents[1], "R")== 0) {
                    cc->cacheTag = tagIndex;
                    cc->lastUsed = index;
                    cc->dirtyBit = 0;
                    readTime = readTime + (1 + (2 * MISS_PENALTY));
                    readMisses++;
                    bytesRead = bytesRead + noOfBytes;
                    bytesWritten = bytesWritten + noOfBytes;
                    dirtyReadMisses++;
                }
            }
        }   
            
         
        if (strcmp(traceComponents[1], "W")== 0) {
            writeCount++;
        }

        if (strcmp(traceComponents[1], "R")== 0) {
            readCount++;
        }   

        if(verboseModeOn == 1){
            if(index >= verboseStart &&  index <= verboseEnd){               
                // printf("The mem address in binary is : %s",memAddBin);
                printf("%d %X %X %d %X %d %d %s\n",index,cacheIndex,tagIndex,validBit,cacheTag, dirtyBit,hitOrMiss, caseNumber);
                // printf("The cache index is : %X\n", cacheIndex);
            }            
        }

        free(traceComponents);
        free(memAddBin);
        index++;
    }

    // calculate statistics
    accessCount = readCount + writeCount;
    totalMisses = readMisses + writeMisses;
    missRate = (double)totalMisses/(double)accessCount;


    printf("direct-mapped, writeback, size = %luKB\n",cacheSize);
    printf("loads %d stores %d total %d\n", readCount , writeCount, accessCount);
    printf("rmiss %d wmiss %d total %d\n", readMisses , writeMisses , totalMisses);
    printf("dirty rmiss %d dirty wmiss %d\n", dirtyReadMisses , dirtyWriteMisses);
    printf("bytes read %d bytes written %d\n", bytesRead, bytesWritten);
    printf("read time %d write time %d\n", readTime, writeTime);
    printf("miss rate %f\n", missRate);

    free(fileContent);
    free(traces);

    return 0;
}

int getCacheIndex(char * memAddBin){
    
    char * cacheIndexBin = malloc(32);
    cacheIndexBin[0] = 0;

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
    tagIndexBin[0] = 0;


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
char * HexToBin(char* hexdec)
{
    char * memAddBin = malloc(128);

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
    return memAddBin;
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
