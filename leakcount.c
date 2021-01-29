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
        FILE*in;
        in = fopen("in.txt", "w+");
        fprintf(in, "0 0");
        fclose(in);
        
        pid_t pid;
        if((pid = fork()) == 0){
            //execvpe(argv[1], (argv + 1),  envp);
            for(int i = 0; i < argc - 1; ++i){
                printf("%s\n", *argv[i]);
            }

            
        }else{
            waitpid(pid, NULL, 0);
            int total = 0, total_bytes = 0;
            in = fopen("in.txt", "r");
            fscanf(in, "%d %d", &total, &total_bytes);
            fprintf(stderr, "TOTAL\t%d\t%d\n", total, total_bytes);
        }
        execlp("rm", "rm", "in.txt");
        fclose(in);
    }
    return 0;
}
