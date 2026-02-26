# CMSC 125 - Lab 1: Unix Shell

This laboratory assignment focuses on process management and I/O redirection using the POSIX API in C. The members will implement a simplified Unix shell that demonstrates how operating systems manage processes and handle basic I/O operations.

## Members

- Caraig, Adriel Neyro S.
- Genilo, Kent Francis E.

## Compilation and Usage
To compile the shell, ensure you are in the root folder then run the following command on the terminal:
```
make all
```
Then, to run the shell, just run the following command:
```
make run
```
If you want to remove the previous compilation of the shell, run the following command:
```
make clean
```
Additionally, there are some initial tests that can be run with the following command:
```
make test
```

## Implemented Features
These are the currently implemented features:
- Interactive shell loop (mysh.c)
- String tokenization (lexer.c)
- Command parsing (parser.c)
- Shell command history (history.c)

The following implemented features are inside the interpreter.c file:
- Built-in command handling
- External command execution
- I/O redirection
- Background process management
- Zombie process cleanup

## Architecture Overview
Files are separated by task delagation on a Input → Tokenize → Parse → Execute program loop. There are 3 structs found on command.h, process.h, and history.h files that dictate the structure of the data that this shell uses.

Below is the full shell logic:
```
Input → Tokenize (converts to string array) → Parse (converts to Command struct) → Interpret
    ├→ Built-in? → Execute (pwd/cd/exit/history) → Cleanup
    └→ External? → Fork → I/O Setup → Execvp → Cleanup
```

## Limitations
This shell currently only follows the requirements stated in the laboratory guide. Advanced shell features are not yet implemented.