# Data Reader
A C program utility that reads arbitrary data from standard input and writes it to a file. 

## Features
- There is no length restriction to the input data. 
- Supports read from a file instead of _stdin_
- The files are stored with the extension - _.dat_ . File names have timestamps added to make them unique.
- Use key combination __< Enter > < Ctrl+z > < Enter > < Ctrl+z > < Enter >__ to generate EOF and close the file when reading from _stdin_



## Options
All arguments are optional
| Argument | Description |   Additional Comments |
| ------   | ------      |   ------              | 
|-p        | Path to store the file  | absolute paths alone are supported currently. Defaults to current working directory if not provided. |
|-n        | File name prefix to use | Default prefix is _File_ _  |
|-r        | Reads from the file instead of stdin | absolute paths only |
|-help     | Prints the help instructions |

## Typical Usage

```
DataReader.exe -p <directory path> -n <Preferred file name prefix> -r <file to read from>
```
> Tested in Windows with gcc compiler. Not tested and may have to be tuned for other environments.
