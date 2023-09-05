#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
 #include <sys/types.h>

void execute(char* const args[])
{
    int i;
    printf("*** Entered:");
    for (i = 0; args[i] != NULL; i++)
        printf(" %s", args[i]);
    printf(" (%d words)\n", i);
    
    int child_status;
    
    pid_t pid = fork();
    
    /*
        First checks if the pid is less than 0, then it means fork has failed.
        If pid equals to 0, then it is a child process. If the argument passed is valid, 
        then it runs a new program. exec funciton only returns if there is an error, in which
        case it prints that the command is not found.
        The parent process waits for the child process. If the exit status of child process is 0, 
        then it prints that the child exited successfully. If the exit status of child process is 
        nonzero, then it indicates that the child process was not able to executed and prints out 
        the exit status of the child process.
    */
    
    if(pid < 0){
        printf("*** Fork Failed");
    }else if(pid == 0){
        if(execvp(args[0], args) < 0){
            printf("*** Command not found: %s\n", args[0]);
    	    exit(1);
    	}
    }else{
        wait(&child_status);
        if(WIFEXITED(child_status)){
            if(WEXITSTATUS(child_status) == 0){
                printf("*** Child exited successfully\n");
            }else{
                printf("*** Child exited with %d\n", WEXITSTATUS(child_status));
            }
        }
    } 
}


/*
    In this version of execute, the parent waits 1second for the child process to finish. 
    If the child process is not finished, then the parent process kills the child process.
*/

void execute_extra_credit(char* const args[])
{
    
    int i;
    printf("*** Entered:");
    for (i = 0; args[i] != NULL; i++)
        printf(" %s", args[i]);
    printf(" (%d words)\n", i);
    
    int child_status;
    
    pid_t pid = fork();
    
    if(pid < 0){
        printf("*** Fork Failed");
    }else if(pid == 0){
        if(execvp(args[0], args) < 0){
            printf("*** Command not found: %s\n", args[0]);
    	    exit(1);
        }
    }else{
        sleep(1);
        if(pid != waitpid(pid, &child_status, WNOHANG)){
            printf("parent waited 1s for child to finish\n");
            kill(pid, SIGTERM);
        }
        if(WIFEXITED(child_status)){
            if(WEXITSTATUS(child_status) == 0){
                printf("*** Child exited successfully\n");
            }else{
                printf("*** Child exited with %d\n", WEXITSTATUS(child_status));
            }
        }
    }
}


