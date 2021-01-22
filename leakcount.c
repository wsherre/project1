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
    if(strcmp(".", &argv[0][0]) == 0){
        printf("sup\n");
    }else{
    execvpe(argv[0], argv, envp);}

}
