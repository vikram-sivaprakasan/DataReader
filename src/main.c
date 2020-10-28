#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include "DataReader.h"

int main(int argc, char* argv[])
{
    ERROR_TYPE result = ERROR_NOERROR;
    /* Parse the arguments */
    result = DataReader_ParseArguments(argc, argv);
    if(result == ERROR_NOERROR)
    {
        /* Start data read */
        while(1)
        {
            char readFile[MAX_FILEPATH_LENGTH] = { '\0' };
            char choice;

            printf("------------------Data Reader------------------------\n");
            printf("s - Read from stdin\n");
            printf("f - Read from file\n");
            printf("e - Exit\n");
            printf("Enter you choice: ");
            (void)scanf("%c", &choice);
            (void)getchar(); /* Added to capture an unwanted newline */
            switch(choice)
            {
            case 's':
            case 'S':
                /* No action */
                printf("Input read from stdin \n");
                result = DataReader_ReadData("");
                break;

            case 'f':
            case 'F':
                printf("Enter the file name with full path: \n");
                (void)scanf("%s", readFile);
                printf("-----------------------------------------------------\n");
                printf("Input read from %s \n", readFile);
                result = DataReader_ReadData(readFile);
                break;

            case 'e':
            case 'E':
                printf("-----------------------------------------------------\n");
                printf("Program terminated\n");
                printf("-----------------------------------------------------\n");
                exit(0);
                break;

            default:
                printf("Invalid input.Please try again\n");
                break;
            }
            if(ERROR_NOERROR != result)
            {
                printf("%s\n", DataReader_ConvertErrorToString(result));
            }
        }
    }
    else
    {
        printf("%s\n", DataReader_ConvertErrorToString(result));
    }

    return 0;
}