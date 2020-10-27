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
#define DEFAULT_FILENAME_PREFIX  "File_"
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
    {ARGUMENT_FILENAME, "-n", "- file fileName prefix to use" },
    {ARGUMENT_HELP, "-help", "- Prints the help instructions"}
};
/*----------------------------------------------------------------------------------*/
/* Static variables */
static char fWritePath[MAX_FILEPATH_LENGTH] = { '\0' };
static char fWriteFilePrefix[MAX_FILEPATH_LENGTH] = { '\0' };
/*----------------------------------------------------------------------------------*/
/* Local function declarations */
static ARGUMENT_TYPE findArgument(const char* pString);
static bool configWritePath(const char* pWritePath, int pSize);
static bool configWriteFilePrefix(const char* pWriteFilePrefix, int pSize);
static bool defineWriteFile(char* pWriteFile, int pSize);
static void readData(char* pWriteFile, char* pReadFile);
static void getTimeStamp(char* pTimeStamp);
static void help();
static void printError(ERROR_TYPE pError, const char* pDisplayString, bool pExit);
/*----------------------------------------------------------------------------------*/
/* main() start */
int main(int argc, char* argv[])
{
    /* Check for command line arguments */
    if(argc != 1)
    {
        int i;
        for(i = 1; i < argc; i = i + 2)
        {
            switch(findArgument(argv[i]))
            {
            case ARGUMENT_PATH:
                if(!configWritePath(argv[i + 1], strlen(argv[i + 1])))
                {
                    printError(ERROR_PATHTOOLONG, NULL, true);
                }
                break;

            case ARGUMENT_FILENAME:
                if(!configWriteFilePrefix(argv[i + 1], strlen(argv[i + 1])))
                {
                    printError(ERROR_PATHTOOLONG, NULL, true);
                }
                break;

            case ARGUMENT_HELP:
                printError(ERROR_NOERROR, NULL, true);
                break;

            default:
                printError(ERROR_INVALIDARG, argv[i], true);
                break;
            }
        }
    }
    /* Set file path to default working directory if not provided */
    if(!strlen(fWritePath))
    {
        char workingDirectory[MAX_FILEPATH_LENGTH] = {'\0'};
        (void)_getcwd(workingDirectory, MAX_FILEPATH_LENGTH);
        if(!configWritePath(workingDirectory, strlen(workingDirectory)))
        {
            printError(ERROR_PATHTOOLONG, NULL, true);
        }
    }
    /* Set file name prefix to default if not provided */
    if(!strlen(fWriteFilePrefix))
    {
        if(!configWriteFilePrefix(DEFAULT_FILENAME_PREFIX, strlen(DEFAULT_FILENAME_PREFIX)))
        {
            printError(ERROR_PATHTOOLONG, NULL, true);
        }
    }

    /* Start data read */
    while(1)
    {
        char readFile[MAX_FILEPATH_LENGTH] = { '\0' };
        char writeFile[MAX_FILEPATH_LENGTH] = { '\0' };
        char choice;
        /* determine the output file with full path */
        if(!defineWriteFile(writeFile, sizeof(writeFile)))
        {
            printError(ERROR_PATHTOOLONG, NULL, true);
        }
        printf("------------------Data Reader------------------------\n");
        printf("s - Read from stdin\n");
        printf("f - Read from file\n");
        printf("e - Exit\n");
        printf("Enter you choice: ");
        (void)scanf("%c", &choice);
        (void)getchar(); /* Added to capture the unwanted newline */
        switch(choice)
        {
        case 's':
        case 'S':
            readData(writeFile, readFile);
            break;

        case 'f':
        case 'F':
            printf("Enter the file name with full path: \n");
            (void)scanf("%s", readFile);
            readData(writeFile, readFile);
            break;

        case 'e':
        case 'E':
            exit(0);
            break;

        default:
            printf("Invalid input.Please try again\n");
            break;
        }
    }
    return 0;
}
/* main() end */
/*----------------------------------------------------------------------------------*/
/* Local function definitions */
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
}/*-----------------------------------------------------------------------------------
 * Name         : configWritePath
 * Inputs       : char* pWritePath - string argument containing the file path
 * Outputs      : True for successful config update. False otherwise
 * Description  : Checks and stores the configured write path
 -----------------------------------------------------------------------------------*/
static bool configWritePath(const char* pWritePath, int pSize)
{
    bool ret = false;
    /* Ensure the string size does not exceed the max limit */
    if(sizeof(fWritePath) > pSize)
    {
        strncpy(fWritePath, pWritePath, pSize);
        /* Ensure write path ends with Path delimiter */
        if(fWritePath[strlen(fWritePath) - 1] != PATH_DELIMITER)
        {
            if(strlen(fWritePath) < sizeof(fWritePath))
            {
                /* Append path delimiter to the end */
                fWritePath[strlen(fWritePath)] = PATH_DELIMITER;
                ret = true;
            }
        }
        else
        {
            ret = true;
        }

    }
    return ret;
}
/*-----------------------------------------------------------------------------------
 * Name         : configWriteFilePrefix
 * Inputs       : char* pWriteFilePrefix - string argument containing the file path
 *                int size - size of the write File buffer
 * Outputs      : True for successful config update. False otherwise
 * Description  : Checks and stores the configured write file name prefix
 -----------------------------------------------------------------------------------*/
static bool configWriteFilePrefix(const char* pWriteFilePrefix, int pSize)
{
    bool ret = false;
    /* Ensure the string size does not exceed the max limit */
    if(sizeof(fWriteFilePrefix) > pSize)
    {
        strncpy(fWriteFilePrefix, pWriteFilePrefix, pSize);
        /* Ensure file name prefix ends with underscore */
        if(fWriteFilePrefix[strlen(fWriteFilePrefix) - 1] != '_')
        {
            if(strlen(fWriteFilePrefix) < sizeof(fWriteFilePrefix))
            {
                /* Append path delimiter to the end */
                fWriteFilePrefix[strlen(fWriteFilePrefix)] = '_';
                ret = true;
            }
        }
        else
        {
            ret = true;
        }
    }
    return ret;
}
/*-----------------------------------------------------------------------------------
 * Name         : defineWriteFile
 * Inputs       : char* pWriteFile - Points to the write file path. Shall be loaded
 *                                    with filename and full file path
 *                int size - size of the write File buffer
 * Outputs      : True if Write file path defined successfully. False for failure
 * Description  : Determine the current file to write with full file path
 -----------------------------------------------------------------------------------*/
static bool defineWriteFile(char* pWriteFile, int pSize)
{
    bool ret = false;
    /* File Name is a combination of file prefix and current time stamp. Fetch timestamp */
    char timeStamp[TIMESTAMP_LENGTH] = { '\0' };
    getTimeStamp(timeStamp);
    strcat(timeStamp, DEFAULT_FILE_EXTENSTION);
    /* Ensure the combined path is less than the maximum path length */
    if((strlen(fWritePath) + strlen(fWriteFilePrefix) + strlen(timeStamp)) < pSize)
    {
        /* Store the file name with full path */
        strcat(pWriteFile, fWritePath);
        strcat(pWriteFile, fWriteFilePrefix);
        strcat(pWriteFile, timeStamp);
        ret = true;
    }
    return ret;
}
/*-----------------------------------------------------------------------------------
 * Name         : readData
 * Inputs       : char* pWriteFile - Output file to be written to
 *                char* pReadFile - Input file to be read from
 * Outputs      :
 * Description  : Reads the data and saves it to the output file
 -----------------------------------------------------------------------------------*/
static void readData(char* pWriteFile, char* pReadFile)
{
    FILE* output;
    FILE* input;
    /* Open the input file to read if provided */
    if(strlen(pReadFile))
    {
        input = fopen(pReadFile, "rb");
        if(input == NULL)
        {
            printError(ERROR_FILEOPEN, pReadFile, false);
        }
        printf("Input read from %s \n", pReadFile);
    }
    else
    {
        input = stdin;
        printf("Input read from stdin \n");
    }

    /* Open the output file for writing
       Note: Since time stamps are unique, the possibility of file overwrite is not considered
    */
    output = fopen(pWriteFile, "wb");
    if(output == NULL)
    {
        /* Exit program if output file cannot be opened as it is a configuration error */
        printError(ERROR_FILEOPEN, pWriteFile, true);
    }
    else
    {
        printf("-----------------------------------------------------\n");
        /* Read until end of input */
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
        printf("Data saved to file - %s\n", pWriteFile);
        printf("-----------------------------------------------------\n");
        fclose(output);
        if(input != stdin)
        {
            fclose(input);
        }
    }
}
/*-----------------------------------------------------------------------------------
 * Name         : getTimeStamp
 * Inputs       : char* timestamp - Buffer to store the timestamp string
 * Outputs      :
 * Description  : Gets the current time stamp, replaces unsupported file fileName
                  characters with '_' and stores the string to the passed buffer
 -----------------------------------------------------------------------------------*/
static void getTimeStamp(char* timestamp)
{
    unsigned int i;
    time_t t = time(NULL);
    struct tm* currentTime = localtime(&t);
    strftime(timestamp, TIMESTAMP_LENGTH, "%c", currentTime);
    /* Remove space and unsupported file fileName characters */
    for(i = 0; i < strlen(timestamp); i++)
    {
        if((timestamp[i] == ' ') || (timestamp[i] == ':') || (timestamp[i] == '/'))
        {
            timestamp[i] = '_';
        }
    }
}
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
 * Name         : printError
 * Inputs       : ERROR_TYPE error - Error to be reported
                  const char* displayString - String to be printed in the error
                  bool exitProgram - Set to true if program has to be exited.
 * Outputs      : Argument type.
 * Description  : Prints error message and exits program if required
 -----------------------------------------------------------------------------------*/
static void printError(ERROR_TYPE error, const char* displayString, bool exitProgram)
{
    switch(error)
    {
    case ERROR_INVALIDARG:
        printf("invalid argument %s \n", displayString);
        help();
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
        help();
        break;
    }
    /* Exit program */
    if(exitProgram)
    {
        exit(1);

    }
}
/*----------------------------------------------------------------------------------*/
