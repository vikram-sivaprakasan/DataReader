/*----------------------------------------------------------------------------------*/
/* Header includes */
#include <stdio.h>
#include "lib\CuTest.h"

/*----------------------------------------------------------------------------------*/
/* Test Suites */
CuSuite* DataReaderGetSuite();
/*----------------------------------------------------------------------------------*/
/* Run Tests*/
void RunAllTests(void)
{
    CuSuite* suite = CuSuiteNew();
    CuString* summary = CuStringNew();
    /* Add and run Test suite */
    CuSuiteAddSuite(suite, DataReaderGetSuite());
    CuSuiteRun(suite);
    /* Print results */
    printf("-----------------------------------------------------\n");
    printf("Unit Test Results\n");
    printf("-----------------------------------------------------\n");
    int i;
    for(i = 0; i < suite->count; ++i)
    {
        CuTest* testCase = suite->list[i];
        CuStringAppend(summary, testCase->failed ? "FAILED .. " : "PASSED .. ");
        CuStringAppend(summary, testCase->name);
        CuStringAppend(summary, "\n");
    }
    CuStringAppend(summary, "-----------------------------------------------------\n");
    CuSuiteDetails(suite, summary);
    CuStringAppend(summary, "-----------------------------------------------------\n");
    printf("%s", summary->buffer);
}
/*----------------------------------------------------------------------------------*/
int main(void)
{
    RunAllTests();
    return 0;
}
/*----------------------------------------------------------------------------------*/