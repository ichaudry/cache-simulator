#ifndef MAIN
#define MAIN

//Buffer to store tokens
#define TRACES_BUFFER 64
#define KILOBYTE 1024
#define BLOCKSIZE 32

/**
 * Struct for passing in arguments to threads
 */
typedef struct {
    int validBit;
    int dirtyBit;
    char cacheTag[128];
} cacheContent;


char **tokenizeFileContents(char *fileContent, char * delimeter);

#endif
