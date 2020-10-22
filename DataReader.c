#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <direct.h>
#include <stdbool.h>

/*----------------------------------------------------------------------------------*/
/* definitions */
#define MAX_FILEPATH_LENGTH 255
#define TIMESTAMP_LENGTH 64
#define DEFAULT_FILENAME  "File_"
#define DEFAULT_FILE_EXTENSTION ".dat"
#define PATH_DELIMITER '\\'

/*----------------------------------------------------------------------------------*/
/* Custom data types */
/* Command line argument types supported */
typedef enum
{
    ARGUMENT_PATH = 0,
    ARGUMENT_FILENAME,
    ARGUMENT_HELP,
    ARGUMENT_MAX /*This item should always be at the end*/
} ARGUMENT_TYPE;

/* Maps the command line argument type and the argument string id */
struct Args
{
    ARGUMENT_TYPE type;
    char* argString;
    char* argDescription;
};
/* Command line argument list*/
const struct Args argument_list[ARGUMENT_MAX] =
{
    {ARGUMENT_PATH, "-p", "- Path to store the file (absolute paths only)"},
    {ARGUMENT_FILENAME, "-n", "- file name prefix to use" },
    {ARGUMENT_HELP, "-help", "- Prints the help instructions"}
};
/*----------------------------------------------------------------------------------*/
/* Local function declarations */
static void help();
static ARGUMENT_TYPE findArgument(const char* string);
static void getTimeStamp(char* string);
/*----------------------------------------------------------------------------------*/
/* main() start */
int main(int argc, char* argv[])
{
    char* fileName = DEFAULT_FILENAME;
    char filePath[MAX_FILEPATH_LENGTH] = { '\0' };
    FILE* file;
    /* Check for command line arguments */
    if(argc != 1)
    {
        int i;
        for(i = 1; i < argc; i = i + 2)
        {
            switch(findArgument(argv[i]))
            {
            case ARGUMENT_PATH:
                strcpy(filePath, argv[i + 1]);
                break;

            case ARGUMENT_FILENAME:
                fileName = argv[i + 1];
                break;

            case ARGUMENT_HELP:
                help();
                exit(1);
                break;

            default:
                printf("invalid argument %s \n", argv[i]);
                help();
                exit(1);
                break;
            }
        }
    }
    /* Get current working directory if path not provided by user */
    if(!strlen(filePath))
    {
        (void)_getcwd(filePath, MAX_FILEPATH_LENGTH);
    }
    /* Append path limiter to the end */
    if(filePath[strlen(filePath) - 1] != PATH_DELIMITER)
    {
        filePath[strlen(filePath)] = PATH_DELIMITER;
    }

    char timeStamp[TIMESTAMP_LENGTH];
    char currentFile[MAX_FILEPATH_LENGTH] = { '\0' };
    /* File Name is a combination of file prefix and current time stamp. Fetch timestamp */
    getTimeStamp(timeStamp);
    if((strlen(filePath) + strlen(timeStamp) + strlen(timeStamp) + strlen(DEFAULT_FILE_EXTENSTION)) > MAX_FILEPATH_LENGTH)
    {
        printf("Error! Path length exceeds the max size of %d characters\n", MAX_FILEPATH_LENGTH);
        exit(1);
    }
    printf("-----------------------------------------------------\n");

    /* Open the file.
       Note: Since time stamps are unique, the possibility of file overwrite is not considered
    */
    strcat(currentFile, filePath);
    strcat(currentFile, fileName);
    strcat(currentFile, timeStamp);
    strcat(currentFile, DEFAULT_FILE_EXTENSTION);
    /* Open the file for writing*/
    file = fopen(currentFile, "wb");
    if(file == NULL)
    {
        printf("Error! Unable to open new file - %s\n", currentFile);
        exit(1);
    }
    else
    {
        printf("Enter the data to be read: \n");
        printf("-----------------------------------------------------\n");
        /* Continue to read from stdin until end of input is indicated */
        while(1)
        {
            char readChar;
            readChar = getc(stdin);
            if(readChar != EOF)
            {
                /* Write data to file */
                fwrite(&readChar, sizeof(readChar), 1, file);
                fflush(file);
            }
            else
            {
                printf("-----------------------------------------------------\n");
                printf("End of input. \n");
                break;
            }
        }
        printf("Data saved to file - %s\n", currentFile);
        printf("-----------------------------------------------------\n");
        fclose(file);
    }
    return 0;
}
/* main() end */
/*----------------------------------------------------------------------------------*/
/* Local function definitions */
/*-----------------------------------------------------------------------------------
 * Name         : help
 * Inputs       :
 * Outputs      :
 * Description  : Prints the help information for the program
 -----------------------------------------------------------------------------------*/
static void help(void)
{
    int i;
    printf("------------------Data Reader------------------------\n");
    printf("Reads data from stdin and saves it to a file \n");
    printf("Supported Arguments: \n");
    for(i = 0; i < ARGUMENT_MAX; i++)
    {
        printf("%s %s \n", argument_list[i].argString, argument_list[i].argDescription);
    }
    printf("-----------------------------------------------------\n");
}
/*-----------------------------------------------------------------------------------
 * Name         : findArgument
 * Inputs       : char* sring - argument string
 * Outputs      : Argument type.
 * Description  : Searches for the current argument string in the supported
                  argument list and returns the corresponding argument type.
                  For invalid strings, ARGUMENT_MAX is returned
 -----------------------------------------------------------------------------------*/
static ARGUMENT_TYPE findArgument(const char* string)
{
    int i;
    for(i = 0; i < ARGUMENT_MAX; i++)
    {
        if(!strcmp(string, argument_list[i].argString))
        {
            return argument_list[i].type;
        }
    }
    return i;
}
/*-----------------------------------------------------------------------------------
 * Name         : getTimeStamp
 * Inputs       : char* timestamp - Buffer to store the timestamp string
 * Outputs      :
 * Description  : Gets the current time stamp, replaces unsupported file name
                  characters with '_' and stores the string to the passed buffer
 -----------------------------------------------------------------------------------*/
static void getTimeStamp(char* timestamp)
{
    unsigned int i;
    time_t t = time(NULL);
    struct tm* currentTime = localtime(&t);
    strftime(timestamp, TIMESTAMP_LENGTH, "%c", currentTime);
    /* Remove space and unsupported file name characters */
    for(i = 0; i < strlen(timestamp); i++)
    {
        if((timestamp[i] == ' ') || (timestamp[i] == ':') || (timestamp[i] == '/'))
        {
            timestamp[i] = '_';
        }
    }
}
/*----------------------------------------------------------------------------------*/