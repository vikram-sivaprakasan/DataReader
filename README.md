# Data Reader
A C program utility that reads arbitrary data from standard input or a file and saves it to a file. 

## Options
All arguments are optional

| Argument | Description |   Additional Comments |
| ------   | ------      |   ------              | 
|-p        | Path to store the file  | absolute paths alone are supported currently. Defaults to current working directory if not provided. |
|-n        | File name prefix to use | Default prefix is _File_  _  |
|-help     | Prints the help instructions |

## Usage
- There is no length restriction to the input data. 
- Write File path length is restricted to a max of 255 characters.
- Supports read from a file or _stdin_
- The output files are stored with the extension - _.dat_ . File names have timestamps added to make them unique.
- Use key combination __< Enter > < Ctrl+z > < Enter > < Ctrl+z > < Enter >__ to generate EOF and close the file when reading from _stdin_

```
DataReader.exe -p <directory path> -n <Preferred file name prefix>
```

## Build
Use _build.bat_ to build and execute the code. output will be generated in _.\build_ folder

## Unit Testing
Unit tests created using CuTest framework. Use _execute_tests.bat_ to build and run the unit tests.


> Tested in Windows with gcc compiler. Not tested and may have to be tuned for other environments.