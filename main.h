#ifndef MAIN
#define MAIN

//Buffer to store tokens
#define TRACES_BUFFER 64
#define KILOBYTE 1024
#define BLOCKSIZE 32
#define OFFSET 5
#define MISS_PENALTY 100
#define HEX_PREPEND

/**
 * Struct for passing in arguments to threads
 */
typedef struct {
    int validBit;
    int dirtyBit;
    char cacheTag[128];
} cacheContent;

int indexBits;
int tagBits;

char **tokenizeFileContents(char *fileContent, char * delimeter);
void HexToBin(char* hexdec, char * memAddBin);
void inplace_reverse(char * str);
int getTagIndex(char * memAddBin);
int getCacheIndex(char * memAddBin);

#endif
