/*----------------------------------------------------------------------------------*/
/* Header includes */
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <direct.h>

#include "lib\CuTest.h"
#include "DataReader.h"

/*----------------------------------------------------------------------------------*/
/* Test Definitions */
#define TEST_OUTPUT_FILESIZE_LIMIT_KB "1"
#define TEST_WRITE_FILE_DIR "\\dummy\\"
#define TEST_WRITE_FILE_NAME_PREFIX "test_"
#define TEST_STRING "This is a test line"
#define TEST_STDIN "input.txt"
#define TEST_CUSTOM_INPUT_FILE "custom.txt"
#define TEST_BUFFER_SIZE 1024
#define TEST_INVALID_READ_FILE "*dummy"
/*----------------------------------------------------------------------------------*/
/* static variables */
static char  fl_WorkingDirectory[MAX_FILEPATH_LENGTH] = { '\0' };

/*----------------------------------------------------------------------------------*/
/* Helper functions */
void RedirectInput()
{
    (void)freopen(TEST_STDIN, "r", stdin);
}
void RestoreInput()
{
    (void)freopen("CON", "r", stdin);
    remove(TEST_STDIN);
}
void WriteData(const char* pFileName, const char* pData, const int pSize)
{
    FILE* file = fopen(pFileName, "w");
    fwrite(pData, sizeof(char), pSize, file);
    fclose(file);
}

void ReadData(const char* pFileName, char* pData, int pSize)
{
    FILE* file = fopen(pFileName, "r");
    fread(pData, sizeof(char), pSize, file);
    fclose(file);
}

void GetTestWritePath(char* pFilePath, int pSize)
{
    /* save the current path for restoring later */
    (void)_getcwd(fl_WorkingDirectory, MAX_FILEPATH_LENGTH);
    strcat(pFilePath, fl_WorkingDirectory);
    strcat(pFilePath, TEST_WRITE_FILE_DIR);
    (void)_mkdir(pFilePath);
}

void ResetFilePath(const char* pFilePath)
{
    (void)_chdir(fl_WorkingDirectory);
    (void)_rmdir(pFilePath);
}

int GetFileSize(const char* pFilePath)
{
    FILE* output = fopen(pFilePath, "r");
    fseek(output, 0L, SEEK_END);
    int fileSize = ftell(output);
    fclose(output);
    return fileSize;
}
/*----------------------------------------------------------------------------------*/
/* DataReader Test */
/*-----------------------------------------------------------------------------------
Test Name     : Test ParseArguments - No Arguments
PreConditions : NA
Action        : 1. Invoke DataReader_ParseArguments() with empty argument list
Expectation   : 1. Returns No Error
------------------------------------------------------------------------------------*/
void TestParseArguments_NoArgs(CuTest* tc)
{
    /*Test setup */
    RedirectInput();
    /* Action */
    int iArgC = 0;
    char* iArgV[] = { "" };
    ERROR_TYPE actual = DataReader_ParseArguments(iArgC, iArgV);
    /* Expectation */
    CuAssertIntEquals_Msg(tc, "ERROR_TYPE", ERROR_NOERROR, actual);
    /* Test Cleanup */
    RestoreInput();
}
/*-----------------------------------------------------------------------------------
Test Name     : Test ParseArguments - Valid Arguments
PreConditions : NA
Action        : 1. Invoke DataReader_ParseArguments() with valid arguments
Expectation   : 1. Returns No Error
                2. Write File path will be updated
                3. Write File Name Prefix will be updated
                4. Output File size limit will be updated
------------------------------------------------------------------------------------*/
void TestParseArguments_ValidArgs(CuTest* tc)
{
    /*Test setup */
    RedirectInput();
    /* Action */
    int iArgC = 6;
    char* iArgV[] = { "-p", TEST_WRITE_FILE_DIR, "-n", TEST_WRITE_FILE_NAME_PREFIX, "-s", TEST_OUTPUT_FILESIZE_LIMIT_KB };
    ERROR_TYPE actual = DataReader_ParseArguments(iArgC, iArgV);
    /* Expectation */
    CuAssertIntEquals_Msg(tc, "ERROR_TYPE", ERROR_NOERROR, actual);
    CuAssertStrEquals_Msg(tc, "writeFilePath", TEST_WRITE_FILE_DIR, DataReader_GetWriteFilePath());
    CuAssertStrEquals_Msg(tc, "writeFileNamePrefix", TEST_WRITE_FILE_NAME_PREFIX, DataReader_GetWriteFileNamePrefix());
    CuAssertIntEquals_Msg(tc, "MaxOutputFileSize", atoi(TEST_OUTPUT_FILESIZE_LIMIT_KB), DataReader_GetMaxOutputFileSize());
    /* Test Cleanup */
    RestoreInput();
}
/*-----------------------------------------------------------------------------------
Test Name     : Test ParseArguments - invalid Argument
PreConditions : NA
Action        : 1. Invoke DataReader_ParseArguments() with an invalid argument
Expectation   : 1. Returns Invalid Argument Error
                2. Write File path will be empty
                3. Write File Name Prefix will be empty
                4. Output File Size limit will be zero
------------------------------------------------------------------------------------*/
void TestParseArguments_InvalidArgs(CuTest* tc)
{
    /*Test setup */
    RedirectInput();
    /* Action */
    int iArgC = 1;
    char* iArgV[] = { "-c" };
    ERROR_TYPE actual = DataReader_ParseArguments(iArgC, iArgV);
    /* Expectation */
    CuAssertIntEquals_Msg(tc, "ERROR_TYPE", ERROR_INVALIDARG, actual);
    CuAssertStrEquals_Msg(tc, "writeFilePath", "", DataReader_GetWriteFilePath());
    CuAssertStrEquals_Msg(tc, "writeFileNamePrefix", "", DataReader_GetWriteFileNamePrefix());
    CuAssertIntEquals_Msg(tc, "MaxOutputFileSize", 0, DataReader_GetMaxOutputFileSize());
    /* Test Cleanup */
    RestoreInput();
}
/*-----------------------------------------------------------------------------------
Test Name     : Test ParseArguments - valid and invalid Argument
PreConditions : NA
Action        : 1. Invoke DataReader_ParseArguments() with both valid and invalid args
Expectation   : 1. Returns Invalid Argument Error
                2. Write File path will be empty
                3. Write File Name Prefix will be empty
                4. Output File Size limit will be zero
------------------------------------------------------------------------------------*/
void TestParseArguments_ValidInvalidArgs(CuTest* tc)
{
    /*Test setup */
    RedirectInput();
    /* Action */
    int iArgC = 5;
    char* iArgV[] = { "-p", TEST_WRITE_FILE_DIR, "-n", TEST_WRITE_FILE_NAME_PREFIX, "invalid" };
    ERROR_TYPE actual = DataReader_ParseArguments(iArgC, iArgV);
    /* Expectation */
    CuAssertIntEquals_Msg(tc, "ERROR_TYPE", ERROR_INVALIDARG, actual);
    CuAssertStrEquals_Msg(tc, "writeFilePath", "", DataReader_GetWriteFilePath());
    CuAssertStrEquals_Msg(tc, "writeFileNamePrefix", "", DataReader_GetWriteFileNamePrefix());
    CuAssertIntEquals_Msg(tc, "MaxOutputFileSize", 0, DataReader_GetMaxOutputFileSize());
    /* Test Cleanup */
    RestoreInput();
}
/*-----------------------------------------------------------------------------------
Test Name     : Test ParseArguments - invalid file size limit
PreConditions : NA
Action        : 1. Invoke DataReader_ParseArguments() with invalid file size limit
Expectation   : 1. Returns Invalid File size limit error
                2. Write File path will be empty
                3. Write File Name Prefix will be empty
                4. Output File Size limit will be zero
------------------------------------------------------------------------------------*/
void TestParseArguments_ValidInvalidFileSizeLimit(CuTest* tc)
{
    /*Test setup */
    RedirectInput();
    /* Action */
    int iArgC = 2;
    char* iArgV[] = {"-s", "invalid" };
    ERROR_TYPE actual = DataReader_ParseArguments(iArgC, iArgV);
    /* Expectation */
    CuAssertIntEquals_Msg(tc, "ERROR_TYPE", ERROR_INVALIDFILESIZE, actual);
    CuAssertStrEquals_Msg(tc, "writeFilePath", "", DataReader_GetWriteFilePath());
    CuAssertStrEquals_Msg(tc, "writeFileNamePrefix", "", DataReader_GetWriteFileNamePrefix());
    CuAssertIntEquals_Msg(tc, "MaxOutputFileSize", 0, DataReader_GetMaxOutputFileSize());
    /* Test Cleanup */
    RestoreInput();
}
/*-----------------------------------------------------------------------------------
Test Name     : Test ParseArguments - help
PreConditions : NA
Action        : 1. Invoke DataReader_ParseArguments() with help argument
Expectation   : 1. Returns Help invocation error
------------------------------------------------------------------------------------*/
void TestParseArguments_Help(CuTest* tc)
{
    /*Test setup */
    RedirectInput();
    /* Action */
    int iArgC = 1;
    char* iArgV[] = {"-help"};
    ERROR_TYPE actual = DataReader_ParseArguments(iArgC, iArgV);
    /* Expectation */
    CuAssertIntEquals_Msg(tc, "ERROR_TYPE", ERROR_HELP_INVOKED, actual);
    /* Test Cleanup */
    RestoreInput();
}
/*-----------------------------------------------------------------------------------
Test Name     : Test ReadData - stdin with default config
PreConditions : Clear any existing configurations
Action        : 1. Invoke DataReader_ReadData() with empty ReadFile
Expectation   : 1. Returns No Error
------------------------------------------------------------------------------------*/
void TestReadData_StdinDefaultConfig(CuTest* tc)
{
    /*Test setup */
    /* Add Test string to redirected stdin */
    WriteData(TEST_STDIN, TEST_STRING, strlen(TEST_STRING));
    RedirectInput();
    /* Clear existing configurations */
    DataReader_ResetArguments();
    /* Action */
    char writeFile[MAX_FILEPATH_LENGTH] = { '\0' };
    ERROR_TYPE actual = DataReader_ReadData("", writeFile, sizeof(writeFile));
    /* Expectation */
    CuAssertIntEquals_Msg(tc, "ERROR_TYPE", ERROR_NOERROR, actual);
    /* Read data from saved file and checks if it matches the test string */
    char readData[TEST_BUFFER_SIZE] = { '\0' };
    ReadData(writeFile, readData, strlen(TEST_STRING));
    CuAssertStrEquals_Msg(tc, "Saved Data", TEST_STRING, readData);
    /* Test Cleanup */
    /* Remove the output file */
    remove(writeFile);
    RestoreInput();
}
/*-----------------------------------------------------------------------------------
Test Name     : Test ReadData - Read from file with default config
PreConditions : NA
Action        : 1. Invoke DataReader_ReadData() with custom ReadFile
Expectation   : 1. Returns No Error
------------------------------------------------------------------------------------*/
void TestReadData_FileReadDefaultConfig(CuTest* tc)
{
    /*Test setup */
    /* Add Test string to custom file */
    WriteData(TEST_CUSTOM_INPUT_FILE, TEST_STRING, strlen(TEST_STRING));
    RedirectInput();
    /* Clear existing configurations */
    DataReader_ResetArguments();
    /* Action */
    char writeFile[MAX_FILEPATH_LENGTH] = { '\0' };
    ERROR_TYPE actual = DataReader_ReadData(TEST_CUSTOM_INPUT_FILE, writeFile, sizeof(writeFile));
    /* Expectation */
    CuAssertIntEquals_Msg(tc, "ERROR_TYPE", ERROR_NOERROR, actual);
    /* Read data from saved file and checks if it matches the test string */
    char readData[TEST_BUFFER_SIZE] = { '\0' };
    ReadData(writeFile, readData, strlen(TEST_STRING));
    CuAssertStrEquals_Msg(tc, "Saved Data", TEST_STRING, readData);
    /* Test Cleanup */
    /* Remove the written and custom input file */
    remove(TEST_CUSTOM_INPUT_FILE);
    /* Remove the output file */
    remove(writeFile);
    RestoreInput();
}
/*-----------------------------------------------------------------------------------
Test Name     : Test ReadData - stdin with valid write file path
PreConditions : 1. Set custom valid write file path.
Action        : 1. Invoke DataReader_ReadData() with empty ReadFile
Expectation   : 1. Returns No Error
------------------------------------------------------------------------------------*/
void TestReadData_StdinValidWritePath(CuTest* tc)
{
    /*Test setup */
    /* Add Test string to redirected stdin */
    WriteData(TEST_STDIN, TEST_STRING, strlen(TEST_STRING));
    RedirectInput();
    /* PreConditions */
    /* Create a valid file path */
    char writePath[MAX_FILEPATH_LENGTH] = { '\0' };
    GetTestWritePath(writePath, sizeof(writePath));
    int iArgC = 2;
    char* iArgV[] = { "-p", writePath };
    (void)DataReader_ParseArguments(iArgC, iArgV);
    /* Action */
    char writeFile[MAX_FILEPATH_LENGTH] = { '\0' };
    ERROR_TYPE actual = DataReader_ReadData("", writeFile, sizeof(writeFile));
    /* Expectation */
    /* The output file must be in the custom path specified */
    if(strstr(writeFile, writePath) == NULL)
    {
        CuFail(tc, "Output file not written to the custom path");
    }
    CuAssertIntEquals_Msg(tc, "ERROR_TYPE", ERROR_NOERROR, actual);
    /* Read data from saved file and checks if it matches the test string */
    char readData[TEST_BUFFER_SIZE] = { '\0' };
    ReadData(writeFile, readData, strlen(TEST_STRING));
    CuAssertStrEquals_Msg(tc, "Saved Data", TEST_STRING, readData);
    /* Test Cleanup */
    /* Remove the output file */
    remove(writeFile);
    /* Reset File Path */
    ResetFilePath(writePath);
    RestoreInput();
}
/*-----------------------------------------------------------------------------------
Test Name     : Test ReadData - Read from file with valid write file path
PreConditions : 1. Set custom valid write file path.
Action        : 1. Invoke DataReader_ReadData() with empty ReadFile
Expectation   : 1. Returns No Error
------------------------------------------------------------------------------------*/
void TestReadData_FileReadValidWritePath(CuTest* tc)
{
    /*Test setup */
    RedirectInput();
    /* Add Test string to redirected stdin */
    WriteData(TEST_CUSTOM_INPUT_FILE, TEST_STRING, strlen(TEST_STRING));
    /* PreConditions */
    /* Create a valid file path */
    char writePath[MAX_FILEPATH_LENGTH] = { '\0' };
    GetTestWritePath(writePath, sizeof(MAX_FILEPATH_LENGTH));
    int iArgC = 2;
    char* iArgV[] = { "-p", writePath };
    (void)DataReader_ParseArguments(iArgC, iArgV);
    /* Action */
    char writeFile[MAX_FILEPATH_LENGTH] = { '\0' };
    ERROR_TYPE actual = DataReader_ReadData(TEST_CUSTOM_INPUT_FILE, writeFile, sizeof(writeFile));
    /* Expectation */
    /* The output file must be in the custom path specified */
    if(strstr(writeFile, writePath) == NULL)
    {
        CuFail(tc, "Output file not written to the custom path");
    }
    CuAssertIntEquals_Msg(tc, "ERROR_TYPE", ERROR_NOERROR, actual);
    /* Read data from saved file and checks if it matches the test string */
    char readData[TEST_BUFFER_SIZE] = { '\0' };
    ReadData(writeFile, readData, strlen(TEST_STRING));
    CuAssertStrEquals_Msg(tc, "Saved Data", TEST_STRING, readData);
    /* Test Cleanup */
    /* Remove the custom input file */
    remove(TEST_CUSTOM_INPUT_FILE);
    /* Remove the output file */
    remove(writeFile);
    /* Reset file path */
    ResetFilePath(writePath);
    RestoreInput();
}
/*-----------------------------------------------------------------------------------
Test Name     : Test ReadData - stdin Read with Max File size limit reached
PreConditions : 1. Set custom output file size limit.
Action        : 1. Invoke DataReader_ReadData() with empty ReadFile
Expectation   : 1. Returns Max file size limit error. Data saved upto the allowed limit
------------------------------------------------------------------------------------*/
void TestReadData_StdinFileSizeLimitReached(CuTest* tc)
{
    /*Test setup */
    /* Add large volume of data to redirected stdin */
    char dataBuffer[TEST_BUFFER_SIZE * 2] = { '\0' };
    while((sizeof(dataBuffer) - strlen(dataBuffer)) > strlen(TEST_STRING))
    {
        strcat(dataBuffer, TEST_STRING);
    }
    WriteData(TEST_STDIN, dataBuffer, strlen(dataBuffer));
    RedirectInput();
    /* PreConditions */
    /* Clear existing configurations */
    DataReader_ResetArguments();
    /* Set File Size limit to 1 KB */
    int iArgC = 2;
    char* iArgV[] = { "-s", "1" };
    (void)DataReader_ParseArguments(iArgC, iArgV);
    /* Action */
    char writeFile[MAX_FILEPATH_LENGTH] = { '\0' };
    ERROR_TYPE actual = DataReader_ReadData("", writeFile, sizeof(writeFile));
    /* Expectation */
    CuAssertIntEquals_Msg(tc, "ERROR_TYPE", ERROR_FILE_SIZELIMIT_REACHED, actual);
    /* Check if output file is limited to max size */
    CuAssertIntEquals_Msg(tc, "File size", (GetFileSize(writeFile) / 1024), DataReader_GetMaxOutputFileSize());
    /* Test Cleanup */
    /* Remove the written file */
    remove(writeFile);
    RestoreInput();
}
/*-----------------------------------------------------------------------------------
Test Name     : Test ReadData - Read from file with Max File size limit reached
PreConditions : 1. Set custom output file size limit.
Action        : 1. Invoke DataReader_ReadData() with valid ReadFile
Expectation   : 1. Returns Max file size limit error. Data saved upto the allowed limit
------------------------------------------------------------------------------------*/
void TestReadData_FileReadFileSizeLimitReached(CuTest* tc)
{
    /*Test setup */
    /* Add large volume of data to the Test read file */
    char dataBuffer[TEST_BUFFER_SIZE * 2] = { '\0' };
    while((sizeof(dataBuffer) - strlen(dataBuffer)) > strlen(TEST_STRING))
    {
        strcat(dataBuffer, TEST_STRING);
    }
    WriteData(TEST_CUSTOM_INPUT_FILE, dataBuffer, strlen(dataBuffer));
    RedirectInput();
    /* PreConditions */
    /* Clear existing configurations */
    DataReader_ResetArguments();
    /* Set File Size limit to 1 KB */
    int iArgC = 2;
    char* iArgV[] = { "-s", "1" };
    (void)DataReader_ParseArguments(iArgC, iArgV);
    /* Action */
    char writeFile[MAX_FILEPATH_LENGTH] = { '\0' };
    ERROR_TYPE actual = DataReader_ReadData(TEST_CUSTOM_INPUT_FILE, writeFile, sizeof(writeFile));
    /* Expectation */
    CuAssertIntEquals_Msg(tc, "ERROR_TYPE", ERROR_FILE_SIZELIMIT_REACHED, actual);
    /* Check if output file is limited to max size */
    CuAssertIntEquals_Msg(tc, "File size", (GetFileSize(writeFile) / 1024), DataReader_GetMaxOutputFileSize());
    /* Test Cleanup */
    /* Remove the custom input file */
    remove(TEST_CUSTOM_INPUT_FILE);
    /* Remove the output file */
    remove(writeFile);
    RestoreInput();
}
/*-----------------------------------------------------------------------------------
Test Name     : Test ReadData - Read with invalid write file path
PreConditions : 1. Set custom invalid write file path.
Action        : 1. Invoke DataReader_ReadData() with empty ReadFile
Expectation   : 1. Returns No Error
------------------------------------------------------------------------------------*/
void TestReadData_InvalidWritePath(CuTest* tc)
{
    /*Test setup */
    RedirectInput();
    /* PreConditions */
    int iArgC = 2;
    char* iArgV[] = { "-p", TEST_WRITE_FILE_DIR};
    (void)DataReader_ParseArguments(iArgC, iArgV);
    /* Action */
    char writeFile[MAX_FILEPATH_LENGTH] = { '\0' };
    ERROR_TYPE actual = DataReader_ReadData("", writeFile, sizeof(writeFile));
    /* Expectation */
    CuAssertIntEquals_Msg(tc, "ERROR_TYPE", ERROR_WRITE_FILEOPEN, actual);
    /* Test Cleanup */
    RestoreInput();
}
/*-----------------------------------------------------------------------------------
Test Name     : Test ReadData - Read with invalid read file
PreConditions : NA
Action        : 1. Invoke DataReader_ReadData() with empty ReadFile
Expectation   : 1. Returns No Error
------------------------------------------------------------------------------------*/
void TestReadData_InvalidReadFile(CuTest* tc)
{
    /*Test setup */
    RedirectInput();
    /* Action */
    char writeFile[MAX_FILEPATH_LENGTH] = { '\0' };
    ERROR_TYPE actual = DataReader_ReadData(TEST_INVALID_READ_FILE, writeFile, sizeof(writeFile));
    /* Expectation */
    CuAssertIntEquals_Msg(tc, "ERROR_TYPE", ERROR_READ_FILEOPEN, actual);
    /* Test Cleanup */
    RestoreInput();
}
/*----------------------------------------------------------------------------------*/
/* Suite Creation */
CuSuite* DataReaderGetSuite(void)
{
    CuSuite* suite = CuSuiteNew();
    SUITE_ADD_TEST(suite, TestParseArguments_NoArgs);
    SUITE_ADD_TEST(suite, TestParseArguments_ValidArgs);
    SUITE_ADD_TEST(suite, TestParseArguments_InvalidArgs);
    SUITE_ADD_TEST(suite, TestParseArguments_ValidInvalidArgs);
    SUITE_ADD_TEST(suite, TestParseArguments_ValidInvalidFileSizeLimit);
    SUITE_ADD_TEST(suite, TestParseArguments_Help);
    SUITE_ADD_TEST(suite, TestReadData_StdinDefaultConfig);
    SUITE_ADD_TEST(suite, TestReadData_FileReadDefaultConfig);
    SUITE_ADD_TEST(suite, TestReadData_StdinValidWritePath);
    SUITE_ADD_TEST(suite, TestReadData_FileReadValidWritePath);
    SUITE_ADD_TEST(suite, TestReadData_StdinFileSizeLimitReached);
    SUITE_ADD_TEST(suite, TestReadData_FileReadFileSizeLimitReached);
    SUITE_ADD_TEST(suite, TestReadData_InvalidWritePath);
    SUITE_ADD_TEST(suite, TestReadData_InvalidReadFile);

    return suite;
}
/*----------------------------------------------------------------------------------*/
