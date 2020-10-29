::Definitions
@SET $BUILD_FOLDER=.\build
@SET $EXECUTABLE=DataReader.exe
@SET $SOURCE_FILES=.\src\*.c
@SET $INCLUDE_FOLDER=.\inc
@SET $FINAL_OUTPUT=%$BUILD_FOLDER%\%$EXECUTABLE%
:: GCC build 
@echo ******************** DataReader Build Start *************************
@if not exist %$FINAL_OUTPUT% md %$BUILD_FOLDER%
@gcc -Wall -I%$INCLUDE_FOLDER% %$SOURCE_FILES% -o %$FINAL_OUTPUT%
@echo ******************** DataReader Build End ***************************
@pause