::Definitions
@SET $BUILD_FOLDER=.\build\test
@SET $EXECUTABLE=DataReaderTest.exe
@SET $TEST_CASES=.\test\*.c
@SET $TEST_FILE=.\src\DataReader.c
@SET $INCLUDE_FOLDER=.\inc
@SET $TEST_LIBRARY=.\test\lib\*.c
@SET $TEST_EXECUTABLE=%$BUILD_FOLDER%\%$EXECUTABLE%
:: TESTCASE BUILD
@if not exist %$BUILD_FOLDER% mkdir %$BUILD_FOLDER%
@echo ******************** DataReader Unit Test Build Start *************************
@gcc -Wall -I%$INCLUDE_FOLDER% %$TEST_LIBRARY% %$TEST_CASES% %$TEST_FILE% -o %$TEST_EXECUTABLE%
@echo ******************** DataReader Unit Test Build End ***************************
@if not exist %$TEST_EXECUTABLE% goto _END
@echo Build Successful
@cd %$BUILD_FOLDER%
@%$EXECUTABLE%
@cd ..\..\
:: Delete Test output executable after testing completion
@del %$TEST_EXECUTABLE% /f /q
@del %$BUILD_FOLDER% /f /q
:_END
@pause