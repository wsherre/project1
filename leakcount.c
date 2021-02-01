#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

//first submission to see if i did it right please dont grade my lack of comments
int main(int argc, char **argv){

    //to avoid seg faults checking the other argument
    if(argc > 1){
        char *envp[] = {"LD_PRELOAD=./memory_shim.so", NULL};
        FILE*in;
        in = fopen("in.txt", "w+");
        fprintf(in, "0 0");
        fclose(in);
        
        pid_t pid;
        if((pid = fork()) == 0){
            argv = argv+1;
            execvpe(argv[0], argv, envp);

        }else{
            waitpid(pid, NULL, 0);
            int total = 0, total_bytes = 0;
            size_t leak = 0;
            in = fopen("in.txt", "r");
            do{
                fscanf(in, "%zu\n", &leak);
                if(leak != -1){
                    fprintf(stderr, "LEAK\t%zu\n", leak);
                }
            }while(leak != -1);
            fscanf(in, "%d %d", &total, &total_bytes);
            fprintf(stderr, "TOTAL\t%d\t%d\n", total, total_bytes);
            fclose(in);
        }
        execlp("rm", "rm", "in.txt");
    }
    return 0;
}
