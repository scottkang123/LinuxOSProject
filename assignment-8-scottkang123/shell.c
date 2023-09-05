#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
int MAX_ARGS = 100;

void execute1(char* const args[])
{
    int i;
    printf("*** Command:");
    for (i = 0; args[i] != NULL; i++)
        printf(" %s", args[i]);
    printf(" (%d words)\n", i);

    if (args[0] == NULL) {
        printf("*** Empty command\n");
        return;
    }

    pid_t pid = fork();
    if (pid == 0) {
        execvp(args[0], args);
        printf("*** Command not found: %s\n", args[0]);
        exit(1);
    } else if (pid > 0) {
        int status = 0;
        waitpid(pid, &status, 0);
        if (WEXITSTATUS(status) == 0)
            printf("*** Child exited sucessfully\n");
        else
            printf("*** Child exited with %d\n", WEXITSTATUS(status));
    } else {
        printf("*** Fork failed\n");
    }
}

void execute2(char* const args1[], char* const args2[])
{
    int i;
    printf("*** First command:");
    for (i = 0; args1[i] != NULL; i++)
        printf(" %s", args1[i]);
    printf(" (%d words)\n", i);

    printf("*** Second command:");
    for (i = 0; args2[i] != NULL; i++)
        printf(" %s", args2[i]);
    printf(" (%d words)\n", i);
    
    //Initialize variables 
    int pipefd[2];
    pid_t pid1, pid2;
    int status1, status2;
    
    //Create a pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return;
    }
    
    // Execute the first part of command with a child process
    pid1 = fork();
    if (pid1 == -1) {
        printf("fork unsuccessful\n");
        exit(1);
    } else if (pid1 == 0) {
        // redirect stdout to the write end of the pipe
        // and close the read and write ends of the pipe
        close(pipefd[0]);
        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            printf("dup2 unsuccessful\n");
            exit(1);
        }
        close(pipefd[1]);
        execvp(args1[0], args1);
        printf("execution of the first command unsuccessful\n");
        exit(1);
    }
    
    // Execute the second part of command with a child process
    pid2 = fork();
    if (pid2 == -1) {
        printf("fork unsuccessful\n");
        exit(1);
    } else if (pid2 == 0) {
        // redirect stdin to the read end of the pipe 
        // and close the read and write ends of the pipe
        close(pipefd[1]);
        if (dup2(pipefd[0], STDIN_FILENO) == -1) {
            printf("dup2 unsuccessful\n");
            exit(1);
        }
        //close write and read ends of the pipe
        close(pipefd[0]);
        execvp(args2[0], args2);
        printf("execution of the second command unsuccessful\n");
        exit(1);
    }
    
    //parent process
    close(pipefd[0]);
    close(pipefd[1]);
    
    //wait for both child processes
    waitpid(pid1, &status1, 0);
    waitpid(pid2, &status2, 0);
    
    // check exit status of both children
    if(WEXITSTATUS(status1) == 0){
        printf("*** Child 1 exited successfully\n");
    }else{
        printf("*** Child 1 exited with %d\n", WEXITSTATUS(status1));
    }
    
    if(WEXITSTATUS(status2) == 0){
        printf("*** Child 2 exited successfully\n");
    }else{
        printf("*** Child 2 exited with %d\n", WEXITSTATUS(status2));
    }
}

//TRY OF EXTRA CREDIT
void execute3(char* const args[][MAX_ARGS])
{
    int i, j, fd[2];
    pid_t pid;

    //Find the number of commands 
    //Assumed that this works -> not sure if main.c could be implemented in this way
    
    int num_cmds = 0;
    while (args[num_cmds][0] != NULL) {
        num_cmds++;
    }

    int prev_pipe = -1;
    for (i = 0; i < num_cmds; i++) {
        
        //Create a pipe for each command
        if (pipe(fd) < 0) {
            printf("pipe error\n");
            exit(1);
        }

        //fork for each command
        if ((pid = fork()) < 0) {
            printf("fork error\n");
            exit(1);
        } else if (pid == 0) {
            //Run if the previous pipe was the read end of the pipe
            if (prev_pipe != -1) {
                if (dup2(prev_pipe, STDIN_FILENO) < 0) {
                    printf("dup2 error\n");
                    exit(1);
                }
                close(prev_pipe);
            }

            if (fd[1] != STDOUT_FILENO) {
                if (dup2(fd[1], STDOUT_FILENO) < 0) {
                    printf("dup2 error\n");
                    exit(1);
                }
                close(fd[1]);
            }

            //Close all other read and write ends of the pipe
            for (j = 0; j < num_cmds; j++) {
                if (j != i) {
                    close(fd[0]);
                    close(fd[1]);
                }
            }
 
            //Execute the command
            if (execvp(args[i][0], args[i]) < 0) {
                printf("execute error\n");
                exit(1);
            }
        } else {
            //Closes both read and write ends of the parent process
            //And assign the prev_pipe as the read end of the pipe
            if (prev_pipe != -1) {
                close(prev_pipe);
            }
            prev_pipe = fd[0];
            close(fd[1]);
        }
    }
    
    //simply wait for all child proccesses before the parent exits
    for (i = 0; i < num_cmds; i++) {
        wait(NULL);
    }
}



