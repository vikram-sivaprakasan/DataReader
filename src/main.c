/*----------------------------------------------------------------------------------*/
/* Header includes */
#include "DataReader.h"
#include "String.h"

/*----------------------------------------------------------------------------------*/
/* main() start */
int main(int argc, char* argv[])
{
    ERROR_TYPE result = ERROR_NOERROR;
    /* Parse the arguments */
    if(argc > 1)
    {
        /* The first argument is the program name. It can be skipped */
        result = DataReader_ParseArguments(argc - 1, &argv[1]);
    }

    if(result == ERROR_NOERROR)
    {
        /* Start data read */
        while(1)
        {
            char readFile[MAX_FILEPATH_LENGTH] = { '\0' };
            char writeFile[MAX_FILEPATH_LENGTH] = { '\0' };
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
                printf("-----------------------------------------------------\n");
                printf("Input read from stdin \n");
                printf("-----------------------------------------------------\n");
                result = DataReader_ReadData("", writeFile, sizeof(writeFile));
                break;

            case 'f':
            case 'F':
                printf("Enter the file name with full path: \n");
                (void)scanf("%s", readFile);
                printf("-----------------------------------------------------\n");
                printf("Input read from file - %s \n", readFile);
                printf("-----------------------------------------------------\n");
                result = DataReader_ReadData(readFile, writeFile, sizeof(writeFile));
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
            /* Print output file path if generated */
            if(strlen(writeFile))
            {
                printf("-----------------------------------------------------\n");
                printf("Write file path - %s\n", writeFile);
                printf("-----------------------------------------------------\n");
            }
            if(ERROR_NOERROR != result)
            {
                printf("-----------------------------------------------------\n");
                printf("Error! %s\n", DataReader_ConvertErrorToString(result));
                printf("-----------------------------------------------------\n");
            }
        }
    }
    else if(result == ERROR_HELP_INVOKED)
    {
        DataReader_Help();
    }
    else
    {
        printf("Error - %s\n", DataReader_ConvertErrorToString(result));
    }

    return 0;
}
/* main() end */
/*----------------------------------------------------------------------------------*/