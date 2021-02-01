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
        //create a variable for the preload
        char *envp[] = {"LD_PRELOAD=./memory_shim.so", NULL};

        //create and open a file and place -1 in it
        //the leaks and total are stored in the file separated by a -1, if there
        //are no leaks then we should have a negative -1 in place to indicate that
        FILE*in;
        in = fopen("in.txt", "w+");
        fprintf(in, "-1\n0 0\n");
        fclose(in);
        
        //make pid and fork
        pid_t pid;
        if((pid = fork()) == 0){
            //shift the argv over one. this will shift the 2nd argument into the 
            //first slot
            argv = argv+1;
            //run the thing
            execvpe(argv[0], argv, envp);

        }else{
            //wait for child to finish
            waitpid(pid, NULL, 0);
            int total = 0, total_bytes = 0;
            size_t leak = 0;

            //open the input file and read
            in = fopen("in.txt", "r");
            //scan the first leak size
            //if there are no leaks then this will be -1
            fscanf(in, "%zu\n", &leak);
            while(leak != -1){
                //print to output and scan next number. repeat till -1
                fprintf(stderr, "LEAK\t%zu\n", leak);
                fscanf(in, "%zu\n", &leak);
            }
            //after -1 will be the total amount of leaks and data, get that
            //and print to output
            fscanf(in, "%d %d", &total, &total_bytes);
            fprintf(stderr, "TOTAL\t%d\t%d\n", total, total_bytes);
            //close file
            fclose(in);
        }
        //file is not longer needed
        execlp("rm", "rm", "-f", "in.txt");
    }
    return 0;
}

