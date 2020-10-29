/*----------------------------------------------------------------------------------*/
/* Header includes */
#include <stdio.h>
#include <stdlib.h>

#ifndef DATA_READER_H
#define DATA_READER_H

/*----------------------------------------------------------------------------------*/
/* Definitions */
#define MAX_FILEPATH_LENGTH 255
#define DEFAULT_FILENAME_PREFIX  "File_"
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

/* Error list */
typedef enum
{
    ERROR_NOERROR = 0,
    ERROR_INVALIDARG,
    ERROR_PATHTOOLONG,
    ERROR_READ_FILEOPEN,
    ERROR_WRITE_FILEOPEN,
    ERROR_HELP_INVOKED,
    ERROR_UNKNOWN,
    ERROR_MAX /*This item should always be at the end*/
} ERROR_TYPE;
/*----------------------------------------------------------------------------------*/
/* External function declarations */
/*-----------------------------------------------------------------------------------
 * Name         : DataReader_ParseArguments
 * Inputs       : char pArgc - argument count
 *                char* pArgv[] - reference to arguments string
 * Outputs      : returns -
 *                ERROR_NOERROR - arguments are successfully parsed
 *                ERROR_INVALIDARG - argument is invalid
 *                ERROR_GRACEFUL_CLOSE - argument parsed but execution can be stopped
 * Description  : Parses the argument list and stores the necessary information. The
 *                 argument list is received as an array of strings with the argument id
 *                 and the argument placed one after the other
 -----------------------------------------------------------------------------------*/
extern ERROR_TYPE DataReader_ParseArguments(int pArgc, char* pArgv[]);
/*-----------------------------------------------------------------------------------
 * Name         : DataReader_ReadData
 * Inputs       : const char* pReadFile - Input file to be read from
 *                char* pWriteFile - String buffer to store the output file path
 *                int pSize - Size of pWriteFile buffer
 * Outputs      : returns -
 *                ERROR_NOERROR - data read is successful
 *                ERROR_READ_FILEOPEN - read file cannot be opened
 *                ERROR_WRITE_FILEOPEN - write file cannot be opened
 * Description  : Reads the data and saves it to the output file.
 -----------------------------------------------------------------------------------*/
extern ERROR_TYPE DataReader_ReadData(const char* pReadFile, char* pWriteFile, int pSize);
/*-----------------------------------------------------------------------------------
 * Name         : DataReader_GetWriteFilePath
 * Inputs       :
 * Outputs      : returns -
 *                String buffer reference to the write file path
 * Description  : returns the path to which files will be saved to
 -----------------------------------------------------------------------------------*/
extern const char* DataReader_GetWriteFilePath(void);
/*-----------------------------------------------------------------------------------
 * Name         : DataReader_GetWriteFileNamePrefix
 * Inputs       :
 * Outputs      : returns -
 *                String buffer reference to the write file name prefix
 * Description  : returns the file name prefix used to save the files
 -----------------------------------------------------------------------------------*/
extern const char* DataReader_GetWriteFileNamePrefix(void);
/*-----------------------------------------------------------------------------------
 * Name         : DataReader_ConvertErrorToString
 * Inputs       : ERROR_TYPE pError - error to be fetched
 * Outputs      : returns -
 *                String buffer reference to the error description
 * Description  : returns the string reference to the error description
 -----------------------------------------------------------------------------------*/
extern const char* DataReader_ConvertErrorToString(ERROR_TYPE pError);
/*-----------------------------------------------------------------------------------
 * Name         : DataReader_Help
 * Inputs       :
 * Outputs      :
 * Description  : Prints the help information for the program
 -----------------------------------------------------------------------------------*/
extern void DataReader_Help(void);
/*----------------------------------------------------------------------------------*/
#endif /* DATA_READER_H */