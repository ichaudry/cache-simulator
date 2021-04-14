#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>

//Buffer to store tokens
#define TOKENS_BUFFER 64
#define TOKENS_DELIMITER "\n"

char **tokenizeFileContents(char *fileContent);

int main(int argc, char ** argv){
    char filename[100];
    printf("Enter Trace File: "); 
    scanf("%s", filename);

    
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
    char **tokens = tokenizeFileContents(fileContent);

    int index=0;
    int writeCount = 0;
    int readCount = 0;

    //Loop to iterate over all tokens/traces
    while(1){
        if(tokens[index]==NULL){
            break;
        }

        //Get token
        char * token=tokens[index];
        // printf("The token at index %d is %s\n",index,token);
        
        if (strstr(token, "W") != NULL) {
            writeCount++;
        }

        if (strstr(token, "R") != NULL) {
            readCount++;
        }

        index++;
    }

    printf("The read count is : %d\n", readCount);
    printf("The write count is : %d\n",writeCount);    

    return 0;
}



/**
 *
 * @param fileContent
 * @return
 */
char **tokenizeFileContents(char *fileContent)
{
    int bufferSize = TOKENS_BUFFER, position = 0;
    char **tokens = malloc(bufferSize * sizeof(char*));
    char *token;
    char *savePtr;

    if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while ((token = strtok_r(fileContent, TOKENS_DELIMITER,&savePtr))) {
        tokens[position] = token;
        position++;

        if (position >= bufferSize) {
            bufferSize += TOKENS_BUFFER;
            tokens = realloc(tokens, bufferSize * sizeof(char*));
            if (!tokens) {
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        fileContent=savePtr;
    }

    tokens[position] = NULL;

    return tokens;
}
