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
#define TEST_WRITE_FILE_PATH "\\dummy\\"
#define TEST_WRITE_FILE_NAME_PREFIX "test_"
#define TEST_STRING "This is a test line"
#define TEST_STDIN "input.txt"
#define TEST_CUSTOM_INPUT_FILE "custom.txt"
#define TEST_BUFFER_SIZE 1024
#define TEST_INVALID_READ_FILE "*dummy"

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
    /* Cleanup */
    RestoreInput();
}
/*-----------------------------------------------------------------------------------
Test Name     : Test ParseArguments - Valid Arguments
PreConditions : NA
Action        : 1. Invoke DataReader_ParseArguments() with valid arguments
Expectation   : 1. Returns No Error
                2. Write File path will be updated
                3. Write File Name Prefix will be updated
------------------------------------------------------------------------------------*/
void TestParseArguments_ValidArgs(CuTest* tc)
{
    /*Test setup */
    RedirectInput();
    /* Action */
    int iArgC = 4;
    char* iArgV[] = { "-p", TEST_WRITE_FILE_PATH, "-n", TEST_WRITE_FILE_NAME_PREFIX };
    ERROR_TYPE actual = DataReader_ParseArguments(iArgC, iArgV);
    /* Expectation */
    CuAssertIntEquals_Msg(tc, "ERROR_TYPE", ERROR_NOERROR, actual);
    CuAssertStrEquals_Msg(tc, "WriteFilePath", TEST_WRITE_FILE_PATH, DataReader_GetWriteFilePath());
    CuAssertStrEquals_Msg(tc, "WriteFileNamePrefix", TEST_WRITE_FILE_NAME_PREFIX, DataReader_GetWriteFileNamePrefix());
    /* Cleanup */
    RestoreInput();
}
/*-----------------------------------------------------------------------------------
Test Name     : Test ParseArguments - invalid Argument
PreConditions : NA
Action        : 1. Invoke DataReader_ParseArguments() with an invalid argument
Expectation   : 1. Returns Invalid Argument Error
                2. Write File path will be empty
                3. Write File Name Prefix will be empty
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
    CuAssertStrEquals_Msg(tc, "WriteFilePath", "", DataReader_GetWriteFilePath());
    CuAssertStrEquals_Msg(tc, "WriteFileNamePrefix", "", DataReader_GetWriteFileNamePrefix());
    /* Cleanup */
    RestoreInput();
}
/*-----------------------------------------------------------------------------------
Test Name     : Test ParseArguments - valid and invalid Argument
PreConditions : NA
Action        : 1. Invoke DataReader_ParseArguments() with both valid and invalid args
Expectation   : 1. Returns Invalid Argument Error
                2. Write File path will be empty
                3. Write File Name Prefix will be empty
------------------------------------------------------------------------------------*/
void TestParseArguments_ValidInvalidArgs(CuTest* tc)
{
    /*Test setup */
    RedirectInput();
    /* Action */
    int iArgC = 5;
    char* iArgV[] = { "-p", TEST_WRITE_FILE_PATH, "-n", TEST_WRITE_FILE_NAME_PREFIX, "invalid" };
    ERROR_TYPE actual = DataReader_ParseArguments(iArgC, iArgV);
    /* Expectation */
    CuAssertIntEquals_Msg(tc, "ERROR_TYPE", ERROR_INVALIDARG, actual);
    CuAssertStrEquals_Msg(tc, "WriteFilePath", "", DataReader_GetWriteFilePath());
    CuAssertStrEquals_Msg(tc, "WriteFileNamePrefix", "", DataReader_GetWriteFileNamePrefix());
    /* Cleanup */
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
    /* Cleanup */
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
    int iArgC = 1;
    char* iArgV[] = { "invalid" };
    (void)DataReader_ParseArguments(iArgC, iArgV);
    /* Action */
    char WriteFile[MAX_FILEPATH_LENGTH] = { '\0' };
    ERROR_TYPE actual = DataReader_ReadData("", WriteFile, sizeof(WriteFile));
    /* Expectation */
    CuAssertIntEquals_Msg(tc, "ERROR_TYPE", ERROR_NOERROR, actual);
    /* Read data from saved file and checks if it matches the test string */
    char readData[TEST_BUFFER_SIZE] = { '\0' };
    ReadData(WriteFile, readData, strlen(TEST_STRING));
    CuAssertStrEquals_Msg(tc, "Saved Data", TEST_STRING, readData);
    /* Cleanup */
    /* Remove the written file */
    remove(WriteFile);
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
    int iArgC = 1;
    char* iArgV[] = { "invalid" };
    (void)DataReader_ParseArguments(iArgC, iArgV);
    /* Action */
    char WriteFile[MAX_FILEPATH_LENGTH] = { '\0' };
    ERROR_TYPE actual = DataReader_ReadData(TEST_CUSTOM_INPUT_FILE, WriteFile, sizeof(WriteFile));
    /* Expectation */
    CuAssertIntEquals_Msg(tc, "ERROR_TYPE", ERROR_NOERROR, actual);
    /* Read data from saved file and checks if it matches the test string */
    char readData[TEST_BUFFER_SIZE] = { '\0' };
    ReadData(WriteFile, readData, strlen(TEST_STRING));
    CuAssertStrEquals_Msg(tc, "Saved Data", TEST_STRING, readData);
    /* Cleanup */
    /* Remove the written and custom input file */
    remove(TEST_CUSTOM_INPUT_FILE);
    remove(WriteFile);
    RestoreInput();
}
/*-----------------------------------------------------------------------------------
Test Name     : Test ReadData - stdin with valid write file path
PreConditions : 1. Set custom valid write file path.
Action        : 1. Invoke DataReader_ReadData() with empty ReadFile
Expectation   : 1. Returns No Error
------------------------------------------------------------------------------------*/
void TestReadData_StdinValidWriteFile(CuTest* tc)
{
    /*Test setup */
    /* Add Test string to custom file */
    WriteData(TEST_CUSTOM_INPUT_FILE, TEST_STRING, strlen(TEST_STRING));
    RedirectInput();
    /* PreConditions */
    /* Create a valid file path */
    char writePath[MAX_FILEPATH_LENGTH] = { '\0' };
    (void)_getcwd(writePath, MAX_FILEPATH_LENGTH);
    strcat(writePath, TEST_WRITE_FILE_PATH);
    (void)_mkdir(writePath);
    int iArgC = 2;
    char* iArgV[] = { "-p", writePath };
    (void)DataReader_ParseArguments(iArgC, iArgV);
    /* Action */
    char WriteFile[MAX_FILEPATH_LENGTH] = { '\0' };
    ERROR_TYPE actual = DataReader_ReadData(TEST_CUSTOM_INPUT_FILE, WriteFile, sizeof(WriteFile));
    /* Expectation */
    CuAssertIntEquals_Msg(tc, "ERROR_TYPE", ERROR_NOERROR, actual);
    /* Read data from saved file and checks if it matches the test string */
    char readData[TEST_BUFFER_SIZE] = { '\0' };
    ReadData(WriteFile, readData, strlen(TEST_STRING));
    CuAssertStrEquals_Msg(tc, "Saved Data", TEST_STRING, readData);
    /* Cleanup */
    /* Remove the test folder, written file and custom input directory */
    remove(TEST_CUSTOM_INPUT_FILE);
    remove(WriteFile);
    (void)_rmdir(writePath);
    RestoreInput();
}
/*-----------------------------------------------------------------------------------
Test Name     : Test ReadData - Read from file with valid write file path
PreConditions : 1. Set custom valid write file path.
Action        : 1. Invoke DataReader_ReadData() with empty ReadFile
Expectation   : 1. Returns No Error
------------------------------------------------------------------------------------*/
void TestReadData_FileReadValidWriteFile(CuTest* tc)
{
    /*Test setup */
    /* Add Test string to redirected stdin */
    WriteData(TEST_STDIN, TEST_STRING, strlen(TEST_STRING));
    RedirectInput();
    /* PreConditions */
    /* Create a valid file path */
    char writePath[MAX_FILEPATH_LENGTH] = { '\0' };
    (void)_getcwd(writePath, MAX_FILEPATH_LENGTH);
    strcat(writePath, TEST_WRITE_FILE_PATH);
    (void)_mkdir(writePath);
    int iArgC = 2;
    char* iArgV[] = { "-p", writePath };
    (void)DataReader_ParseArguments(iArgC, iArgV);
    /* Action */
    char WriteFile[MAX_FILEPATH_LENGTH] = { '\0' };
    ERROR_TYPE actual = DataReader_ReadData("", WriteFile, sizeof(WriteFile));
    /* Expectation */
    CuAssertIntEquals_Msg(tc, "ERROR_TYPE", ERROR_NOERROR, actual);
    /* Read data from saved file and checks if it matches the test string */
    char readData[TEST_BUFFER_SIZE] = { '\0' };
    ReadData(WriteFile, readData, strlen(TEST_STRING));
    CuAssertStrEquals_Msg(tc, "Saved Data", TEST_STRING, readData);
    /* Cleanup */
    /* Remove the test folder written file */
    remove(WriteFile);
    (void)_rmdir(writePath);
    RestoreInput();
}
/*-----------------------------------------------------------------------------------
Test Name     : Test ReadData - Read with invalid write file path
PreConditions : 1. Set custom invalid write file path.
Action        : 1. Invoke DataReader_ReadData() with empty ReadFile
Expectation   : 1. Returns No Error
------------------------------------------------------------------------------------*/
void TestReadData_InvalidWriteFile(CuTest* tc)
{
    /*Test setup */
    RedirectInput();
    /* PreConditions */
    int iArgC = 2;
    char* iArgV[] = { "-p", TEST_WRITE_FILE_PATH};
    (void)DataReader_ParseArguments(iArgC, iArgV);
    /* Action */
    char WriteFile[MAX_FILEPATH_LENGTH] = { '\0' };
    ERROR_TYPE actual = DataReader_ReadData("", WriteFile, sizeof(WriteFile));
    /* Expectation */
    CuAssertIntEquals_Msg(tc, "ERROR_TYPE", ERROR_WRITE_FILEOPEN, actual);
    /* Cleanup */
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
    char WriteFile[MAX_FILEPATH_LENGTH] = { '\0' };
    ERROR_TYPE actual = DataReader_ReadData(TEST_INVALID_READ_FILE, WriteFile, sizeof(WriteFile));
    /* Expectation */
    CuAssertIntEquals_Msg(tc, "ERROR_TYPE", ERROR_READ_FILEOPEN, actual);
    /* Cleanup */
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
    SUITE_ADD_TEST(suite, TestParseArguments_Help);
    SUITE_ADD_TEST(suite, TestReadData_StdinDefaultConfig);
    SUITE_ADD_TEST(suite, TestReadData_FileReadDefaultConfig);
    SUITE_ADD_TEST(suite, TestReadData_StdinValidWriteFile);
    SUITE_ADD_TEST(suite, TestReadData_FileReadValidWriteFile);
    SUITE_ADD_TEST(suite, TestReadData_InvalidWriteFile);
    SUITE_ADD_TEST(suite, TestReadData_InvalidReadFile);

    return suite;
}
/*----------------------------------------------------------------------------------*/
