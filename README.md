# Data Reader
A C program that reads arbitrary data from standard input and writes it to a file. 

## Features
- There is no length restriction to the input data. 
- The files have a default extension - _.dat_ . File Names have timestamps to make them unique.
- Use key combination __< Enter > < Ctrl+z > < Enter >__ to generate EOF and save the data to file

## Options
| Argument | Description |
| ------   | ------      |
| -p       | Path to store the file (absolute paths alone are supported currently) |
|-n        | File name prefix to use |
|-help     | Prints the help instructions |

> Tested in Windows. Not tested and may have to be tuned for other environments
