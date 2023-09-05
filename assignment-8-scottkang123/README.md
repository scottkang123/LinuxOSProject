[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-8d59dc4de5201274e310e4c54b9627a8934c3b88527886e3b421487c677d23eb.svg)](https://classroom.github.com/a/PoOCeebM)
Assignment 8
=============

**Due: March 30, 2023 11:59 PM US Central Time**. Push to GitHub to submit the assignment.

In this assignment you are extending the basic shell interpreter we created in assignment 2. The program should read one or two commands to be executed possibly separated by the pipe character `|`. 
Reading the input is already written for you in `main.c`; you need to complete the `execute2` function in `shell.c` to actually execute a pair of commands that were entered and separated by the pipe character. You should create the `build` directory and compile there with `cmake` as usual. 

## Tasks

* Finish the implementation of the `execute2` function in `shell.c`. 
* Use the `pipe` system call to create a pipe that will be used to send the standard output of the first command to the standard input of the second one. Do not use the `popen` function.
* Use the `dup2` system call to actually change the standard input and output file descriptors.
* Use `fork`, `execvp` and `waitpid` as required (maybe before some of the previous steps).
* Properly handle all error conditions. 
* Properly close all file descriptors (except the standard input, output and error ones).

## Evaluation

Your assignment will be graded according to the following criteria:

- **25 points** for the correct usage of and error handling for the `pipe` system call.
- **25 points** for the correct usage of and error handling for the `dup2` system call.
- **25 points** for the correct usage of and error handling for the `fork`, `execvp` and `waitpid` system calls.
- **25 points** for properly closing all file descriptors that need to be closed.
- **5 points** for coding style, including 1) brief comments on complex portions of code, 2) consistent formatting, and 3) consistent and proper indentation.
- **10 extra points** if you can write a version that allows an arbitrary number (e.g., 100) of commands to pipelined (e.g., a three command pipeline such as `ls -l | grep "homework" | wc -l`).
