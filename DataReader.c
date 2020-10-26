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
#define READ_BUFFER_SIZE 1024

/*----------------------------------------------------------------------------------*/
/* Custom data types */
/* Error list */
typedef enum
{
    ERROR_NOERROR,
    ERROR_INVALIDARG,
    ERROR_PATHTOOLONG,
    ERROR_FILEOPEN,
    ERROR_UNKNOWN
} ERROR_TYPE;

/* Command line argument types supported */
typedef enum
{
    ARGUMENT_PATH = 0,
    ARGUMENT_FILENAME,
    ARGUMENT_READFROM,
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
    {ARGUMENT_READFROM, "-r", "- Reads data from file instead of stdin (absolute path only)" },
    {ARGUMENT_HELP, "-help", "- Prints the help instructions"}
};
/*----------------------------------------------------------------------------------*/
/* Local function declarations */
static void help();
static void printAndExit(ERROR_TYPE error, const char* displayString, bool printHelp);
static ARGUMENT_TYPE findArgument(const char* string);
static void getTimeStamp(char* string);
/*----------------------------------------------------------------------------------*/
/* main() start */
int main(int argc, char* argv[])
{
    char writeFileName[MAX_FILEPATH_LENGTH] = DEFAULT_FILENAME;
    char readFile[MAX_FILEPATH_LENGTH] = { '\0' };
    char writeFile[MAX_FILEPATH_LENGTH] = { '\0' };
    FILE* output;
    FILE* input;
    /* Check for command line arguments */
    if(argc != 1)
    {
        int i;
        for(i = 1; i < argc; i = i + 2)
        {
            switch(findArgument(argv[i]))
            {
            case ARGUMENT_PATH:
                if(strlen(argv[i + 1]) > MAX_FILEPATH_LENGTH)
                {
                    printAndExit(ERROR_PATHTOOLONG, NULL, false);
                }
                else
                {
                    strncpy(writeFile, argv[i + 1], sizeof(writeFile));
                }
                break;

            case ARGUMENT_FILENAME:
                if(strlen(argv[i + 1]) > MAX_FILEPATH_LENGTH)
                {
                    printAndExit(ERROR_PATHTOOLONG, NULL, false);
                }
                else
                {
                    strncpy(writeFileName, argv[i + 1], sizeof(writeFileName));
                }
                break;

            case ARGUMENT_READFROM:
                if(strlen(argv[i + 1]) > MAX_FILEPATH_LENGTH)
                {
                    printAndExit(ERROR_PATHTOOLONG, NULL, false);
                }
                else
                {
                    strncpy(readFile, argv[i + 1], sizeof(readFile));
                }
                break;

            case ARGUMENT_HELP:
                printAndExit(ERROR_NOERROR, NULL, true);
                break;

            default:
                printAndExit(ERROR_INVALIDARG, argv[i], true);
                break;
            }
        }
    }
    /* Get current working directory if path not provided by user */
    if(!strlen(writeFile))
    {
        (void)_getcwd(writeFile, MAX_FILEPATH_LENGTH);
    }
    /* Append path limiter to the end */
    if(writeFile[strlen(writeFile) - 1] != PATH_DELIMITER)
    {
        writeFile[strlen(writeFile)] = PATH_DELIMITER;
    }

    /* File Name is a combination of file prefix and current time stamp. Fetch timestamp */
    char timeStamp[TIMESTAMP_LENGTH];
    getTimeStamp(timeStamp);
    strcat(writeFileName, timeStamp);
    strcat(writeFileName, DEFAULT_FILE_EXTENSTION);

    if((strlen(writeFile) + strlen(writeFileName)) > MAX_FILEPATH_LENGTH)
    {
        printAndExit(ERROR_PATHTOOLONG, NULL, false);
    }
    printf("-----------------------------------------------------\n");

    /* Open the input file to read if provided */
    if(strlen(readFile))
    {
        input = fopen(readFile, "rb");
        if(input == NULL)
        {
            printAndExit(ERROR_FILEOPEN, readFile, false);
        }
        printf("Input read from %s \n", readFile);
    }
    else
    {
        input = stdin;
        printf("Input read from stdin \n");
    }

    /* Open the output file for writing
       Note: Since time stamps are unique, the possibility of file overwrite is not considered
    */
    strcat(writeFile, writeFileName);
    output = fopen(writeFile, "wb");
    if(output == NULL)
    {
        printAndExit(ERROR_FILEOPEN, writeFile, false);
    }
    else
    {
        printf("-----------------------------------------------------\n");
        /* Read until end of input is indicated */
        while(1)
        {
            char readChar[READ_BUFFER_SIZE];
            unsigned int readSize = fread(&readChar, sizeof(char), READ_BUFFER_SIZE, input);
            if(readSize)
            {
                /* Write data to file */
                fwrite(&readChar, sizeof(char), readSize, output);
                fflush(output);
            }
            else
            {
                printf("-----------------------------------------------------\n");
                printf("End of input. \n");
                break;
            }
        }
        printf("Data saved to file - %s\n", writeFile);
        printf("-----------------------------------------------------\n");
        fclose(output);
    }
    fclose(input);
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
 * Name         : printAndExit
 * Inputs       : ERROR_TYPE error - Error to be reported
                  const char* displayString - String to be printed in the error
                  bool printHelp - Set to true if help should be printed as well.
 * Outputs      : Argument type.
 * Description  : Prints error and exits program
 -----------------------------------------------------------------------------------*/
static void printAndExit(ERROR_TYPE error, const char* displayString, bool printHelp)
{
    switch(error)
    {
    case ERROR_INVALIDARG:
        printf("invalid argument %s \n", displayString);
        break;

    case ERROR_PATHTOOLONG:
        printf("Error! Path length exceeds the max length of %d characters \n", MAX_FILEPATH_LENGTH);
        break;

    case ERROR_FILEOPEN:
        printf("Error! Unable to open file - %s\n", displayString);
        break;

    case ERROR_UNKNOWN:
        printf("Unknown error - %s \n", displayString);
        break;

    case ERROR_NOERROR:
    default:
        break;
    }
    if(printHelp)
    {
        help();
    }
    /* Exit program */
    exit(1);
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
