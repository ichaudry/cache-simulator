#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>
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

    int index=0;
    int writeCount = 0;
    int readCount = 0;

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
            printf("%d\n",index);
        }

        free(traceComponents);
        index++;
    }

    printf("The read count is : %d\n", readCount);
    printf("The write count is : %d\n",writeCount);    

    free(fileContent);
    free(traces);

    return 0;
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
