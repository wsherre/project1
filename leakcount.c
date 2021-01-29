#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

//first submission to see if i did it right please dont grade my lack of comments
int main(int argc, char **argv){

    if(argc > 1){
        char *envp[] = {"LD_PRELOAD=./memory_shim.so", NULL};
        
        pid_t pid;
        if((pid = fork()) == 0){
            execvpe(argv[1], (argv + 1),  envp);
            
        }else{
            waitpid(pid, NULL, 0);
        }
    }
    return 0;
}
