#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>


int main(int argc, char **argv){

    char *program = argv[1];
    char *envp[] = {"LD_PRELOAD=./memoryshim.so"};
    for(int i = 0; i < argc; ++i){
        argv[i] = argv[i+1];
    }
    argv[argc] = NULL;
    execvpe(program, argv, envp);

}