#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>


int main(int argc, char **argv){

    char *envp[] = {"LD_PRELOAD=./memory_shim.so", NULL};
    for(int i = 0; i < argc; ++i){
        argv[i] = argv[i+1];
    }
    argv[argc] = NULL;
    execvpe(argv[0], argv, envp);

}
