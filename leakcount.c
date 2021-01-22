#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>


int main(int argc, char **argv){

    char *envp[] = {"LD_PRELOAD=./memory_shim.so", NULL};
    for(int i = 0; i < argc; ++i){
        argv[i] = argv[i+1];
    }
    argv[argc] = NULL;
    char c =  *argv[0];
    pid_t pid;
    if((pid = fork()) == 0){
        if('.' != c){
            execvp(argv[0], argv);
        }else{
            execvpe(argv[0], argv, envp);
        }
    }else{
        waitpid(pid, NULL, 0);
    }
    return 0;
}
