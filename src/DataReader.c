/*----------------------------------------------------------------------------------*/
/* Header includes */
#include <string.h>
#include <time.h>
#include <direct.h>
#include <stdbool.h>
#include "DataReader.h"

/*----------------------------------------------------------------------------------*/
/* Definitions */
#define TIMESTAMP_LENGTH 64
#define BUFFER_SIZE 1024

/*----------------------------------------------------------------------------------*/
/* Custom data types */

/* Maps the command line argument type and the argument string id */
struct Args
{
    ARGUMENT_TYPE type;
    char* argString;
    char* argDescription;
};

/* Maps the error to a description */
struct Errors
{
    ERROR_TYPE errorId;
    char* errorDescription;
};

/* Command line argument list */
const struct Args argument_list[ARGUMENT_MAX] =
{
    {ARGUMENT_PATH, "-p", ": Path to store the file (absolute paths only)"},
    {ARGUMENT_FILENAME, "-n", ": File Name prefix to use" },
    {ARGUMENT_MAXFILESIZE, "-s", ": Maximum size limit for output file (in KB)" },
    {ARGUMENT_HELP, "-help", ": Prints the help instructions"}
};

/* Error list */
const struct Errors error_list[ERROR_MAX] =
{
    {ERROR_NOERROR, "No error"},
    {ERROR_INVALIDARG, "Invalid Arguments"},
    {ERROR_PATHTOOLONG, "Path exceeds max length"},
    {ERROR_INVALIDFILESIZE, "Max output file size limit is invalid"},
    {ERROR_READ_FILEOPEN, "Unable to open file for read"},
    {ERROR_WRITE_FILEOPEN, "Unable to open file for write"},
    {ERROR_FILE_SIZELIMIT_REACHED, "Maximum output file size has been reached"},
    {ERROR_HELP_INVOKED, "Program help requested"},
    {ERROR_UNKNOWN, "Unknown error"}
};
/*----------------------------------------------------------------------------------*/
/* Static variables */
static char fl_WritePath[MAX_FILEPATH_LENGTH] = { NULL_CHARACTER };
static char fl_WriteFilePrefix[MAX_FILEPATH_LENGTH] = { NULL_CHARACTER };
static unsigned int fl_MaxOutputFileSize = 0;
/*----------------------------------------------------------------------------------*/
/* Local function declarations */
static ARGUMENT_TYPE findArgument(const char* pString);
static bool initializeWritePath(const char* pWritePath, unsigned int pSize);
static bool initializeWriteFilePrefix(const char* pWriteFilePrefix, unsigned int pSize);
static bool initializeOutputFileSizeLimit(const char* pSize);
static bool defineWriteFile(char* pWriteFile, unsigned int pSize);
static void getTimeStamp(char* pTimeStamp);
/*----------------------------------------------------------------------------------*/
/* Extern function definitions */
ERROR_TYPE DataReader_ParseArguments(int pArgc, char* pArgv[])
{
    ERROR_TYPE ret = ERROR_NOERROR;
    /* Check for passed arguments */
    if(pArgc)
    {
        int i;
        for(i = 0; i < pArgc; i = i + 2)
        {
            switch(findArgument(pArgv[i]))
            {
            case ARGUMENT_FILENAME:
                if(!initializeWriteFilePrefix(pArgv[i + 1], strlen(pArgv[i + 1])))
                {
                    ret = ERROR_PATHTOOLONG;
                }
                break;

            case ARGUMENT_PATH:
                if(!initializeWritePath(pArgv[i + 1], strlen(pArgv[i + 1])))
                {
                    ret = ERROR_PATHTOOLONG;
                }
                break;

            case ARGUMENT_MAXFILESIZE:
                if(!initializeOutputFileSizeLimit(pArgv[i + 1]))
                {
                    ret = ERROR_INVALIDFILESIZE;
                }
                break;

            case ARGUMENT_HELP:
                return(ERROR_HELP_INVOKED);
                break;

            default:
                /* Reset all config for an invalid argument */
                DataReader_ResetArguments();
                return(ERROR_INVALIDARG);
                break;
            }
        }
    }
    return ret;
}
/*----------------------------------------------------------------------------------*/
ERROR_TYPE DataReader_ReadData(const char* pReadFile, char* pWriteFile, int pSize)
{
    FILE* output;
    FILE* input;
    char writeFile[MAX_FILEPATH_LENGTH] = { '\0' };
    ERROR_TYPE ret = ERROR_NOERROR;
    /* Determine the output file with full path */
    if(!defineWriteFile(writeFile, sizeof(writeFile)))
    {
        pWriteFile = '\0';
        /* Fatal error. Return immediately */
        return(ERROR_PATHTOOLONG);
    }
    /* Open the input file to read if provided */
    if(strlen(pReadFile))
    {
        input = fopen(pReadFile, "rb");
        if(input == NULL)
        {
            /* Fatal error. Return immediately */
            return(ERROR_READ_FILEOPEN);
        }
    }
    else
    {
        input = stdin;
    }
    /* Open the output file for writing
       Note: Since time stamps are unique, the possibility of file overwrite is not considered
    */
    output = fopen(writeFile, "wb");
    if(output == NULL)
    {
        strncpy(pWriteFile, writeFile, strlen(writeFile) < pSize ? strlen(writeFile) : pSize);
        /* Fatal error. Return immediately */
        return(ERROR_WRITE_FILEOPEN);
    }
    else
    {
        unsigned int currentFileSize = 0;
        bool running = true;
        /* Read until end of input */
        while(running)
        {
            char readChar[BUFFER_SIZE];
            unsigned int readSize = fread(&readChar, sizeof(char), BUFFER_SIZE, input);
            if(currentFileSize <= (fl_MaxOutputFileSize - readSize))
            {
                currentFileSize = currentFileSize + readSize;
                if(readSize)
                {
                    /* Write data to file */
                    fwrite(&readChar, sizeof(char), readSize, output);
                    fflush(output);
                }
                else
                {
                    /* File read completed */
                    running = false;
                }
            }
            else
            {
                /* File size limit reached. Stop reading */
                ret = ERROR_FILE_SIZELIMIT_REACHED;
                running = false;
            }
        }
        fclose(output);
        /* Do not close stdin */
        if(input != stdin)
        {
            fclose(input);
        }
    }
    /* Save the generated write file path to the passed buffer */
    strncpy(pWriteFile, writeFile, strlen(writeFile) < pSize ? strlen(writeFile) : pSize);
    return(ret);
}
/*----------------------------------------------------------------------------------*/
const unsigned int DataReader_GetMaxOutputFileSize(void)
{
    return fl_MaxOutputFileSize / 1024;
}
/*----------------------------------------------------------------------------------*/
const char* DataReader_GetWriteFilePath(void)
{
    return fl_WritePath;
}
/*----------------------------------------------------------------------------------*/
const char* DataReader_GetWriteFileNamePrefix(void)
{
    return fl_WriteFilePrefix;
}
/*----------------------------------------------------------------------------------*/
void DataReader_ResetArguments(void)
{
    memset(fl_WritePath, NULL_CHARACTER, sizeof(fl_WritePath));
    memset(fl_WriteFilePrefix, NULL_CHARACTER, sizeof(fl_WriteFilePrefix));
    fl_MaxOutputFileSize = 0;
}
/*----------------------------------------------------------------------------------*/
const char* DataReader_ConvertErrorToString(ERROR_TYPE pError)
{
    unsigned int i;
    for(i = 0; i < ERROR_MAX; i++)
    {
        if(pError == error_list[i].errorId)
        {
            return error_list[i].errorDescription;
        }
    }
    return NULL;
}
/*----------------------------------------------------------------------------------*/
void DataReader_Help(void)
{
    unsigned int i;
    printf("------------------Data Reader------------------------\n");
    printf("Reads data from stdin or a file and saves it to a file \n");
    printf("Supported Arguments: \n");
    for(i = 0; i < ARGUMENT_MAX; i++)
    {
        printf("%s %s \n", argument_list[i].argString, argument_list[i].argDescription);
    }
    printf("-----------------------------------------------------\n");
}
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
    unsigned int i;
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
 * Name         : initializeOutputFileSizeLimit
 * Inputs       : const char* pSize - Output file size limit in string format
 * Outputs      : True for successful config update. False otherwise
 * Description  : Checks and stores the configured write path
 -----------------------------------------------------------------------------------*/
static bool initializeOutputFileSizeLimit(const char* pSize)
{
    /* Let the size be in bytes */
    unsigned int size = atoi(pSize) * 1024;
    if(size != 0)
    {
        fl_MaxOutputFileSize = size;
        return true;
    }
    return false;
}
/*-----------------------------------------------------------------------------------
 * Name         : initializeWritePath
 * Inputs       : char* pWritePath - string argument containing the file path
 * Outputs      : True for successful config update. False otherwise
 * Description  : Checks and stores the configured write path
 -----------------------------------------------------------------------------------*/
static bool initializeWritePath(const char* pWritePath, unsigned int pSize)
{
    /* Ensure the string size does not exceed the max limit */
    if(sizeof(fl_WritePath) > pSize)
    {
        strcpy(fl_WritePath, pWritePath);
        /* Ensure write path ends with Path delimiter */
        char pathDelimiter = PATH_DELIMITER;
        if(fl_WritePath[strlen(fl_WritePath) - 1] != pathDelimiter)
        {
            if(strlen(fl_WritePath) < sizeof(fl_WritePath))
            {
                /* Append path delimiter to the end */
                //fl_WritePath[strlen(fl_WritePath)] = PATH_DELIMITER;
                strncat(fl_WritePath, &pathDelimiter, 1);
                return true;
            }
        }
        else
        {
            return true;
        }

    }
    return false;
}
/*-----------------------------------------------------------------------------------
 * Name         : initializeWriteFilePrefix
 * Inputs       : char* pWriteFilePrefix - string argument containing the file path
 *                int size - size of the write File buffer
 * Outputs      : True for successful config update. False otherwise
 * Description  : Checks and stores the configured write file name prefix
 -----------------------------------------------------------------------------------*/
static bool initializeWriteFilePrefix(const char* pWriteFilePrefix, unsigned int pSize)
{
    /* Ensure the string size does not exceed the max limit */
    if(sizeof(fl_WriteFilePrefix) > pSize)
    {
        strcpy(fl_WriteFilePrefix, pWriteFilePrefix);
        /* Ensure file name prefix ends with underscore */
        char underScore = '_';
        if(fl_WriteFilePrefix[strlen(fl_WriteFilePrefix) - 1] != underScore)
        {
            if(strlen(fl_WriteFilePrefix) < sizeof(fl_WriteFilePrefix))
            {
                /* Append path delimiter to the end */
                strncat(fl_WriteFilePrefix, &underScore, 1);
                return true;
            }
        }
        else
        {
            return true;
        }
    }
    return false;
}
/*-----------------------------------------------------------------------------------
 * Name         : defineWriteFile
 * Inputs       : char* pWriteFile - Points to the write file path. Shall be loaded
 *                                    with filename and full file path
 *                int size - size of the write File buffer
 * Outputs      : True if Write file path defined successfully. False for failure
 * Description  : Determine the current file to write with full file path
 -----------------------------------------------------------------------------------*/
static bool defineWriteFile(char* pWriteFile, unsigned int pSize)
{
    /* Set max file size if not configured */
    if(!fl_MaxOutputFileSize)
    {
        (void)initializeOutputFileSizeLimit(DEFAULT_OUTPUT_FILE_SIZE_KB);
    }

    /* Set file path to default working directory if not provided */
    if(!strlen(fl_WritePath))
    {
        char workingDirectory[MAX_FILEPATH_LENGTH] = { '\0' };
        (void)_getcwd(workingDirectory, MAX_FILEPATH_LENGTH);
        if(!initializeWritePath(workingDirectory, strlen(workingDirectory)))
        {
            return false;
        }
    }
    /* Set file name prefix to default if not provided */
    if(!strlen(fl_WriteFilePrefix))
    {
        if(!initializeWriteFilePrefix(DEFAULT_FILENAME_PREFIX, strlen(DEFAULT_FILENAME_PREFIX)))
        {
            return false;
        }
    }
    /* File Name is a combination of file prefix and current time stamp. Fetch timestamp */
    char timeStamp[TIMESTAMP_LENGTH] = { '\0' };
    getTimeStamp(timeStamp);
    strcat(timeStamp, DEFAULT_FILE_EXTENSTION);
    /* Ensure the combined path is less than the maximum path length */
    if((strlen(fl_WritePath) + strlen(fl_WriteFilePrefix) + strlen(timeStamp)) < pSize)
    {
        /* Store the file name with full path */
        strcat(pWriteFile, fl_WritePath);
        strcat(pWriteFile, fl_WriteFilePrefix);
        strcat(pWriteFile, timeStamp);
        return true;
    }
    return false;
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
/*----------------------------------------------------------------------------------*/