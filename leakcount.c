#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>


int main(int argc, char **argv){

    char *program = argv[1];
    for(int i = 0; i < argc; ++i){
        argv[i] = argv[i+1];
    }
    argv[argc] = NULL;
    execvp(program, argv);

}