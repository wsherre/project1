#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>

//highest syscall number in linux(335) + 1 for #0 ,
#define max_array_size 336
//arbitraily chose this number. this detirmines how many args a program can run with
#define vector_size 50

void print_array(int array[], char**);


int main(int argc, char** argv){

    if(argc < 3){
        fprintf(stderr, "ERROR: must have at least 3 arguments\n");
        exit(1);
    }
    int array[max_array_size];
    for(int i = 0; i < max_array_size; ++i){
        array[i] = 0;
    }
    
    char *vector[vector_size];
    int i = 1;
    vector[0] = strtok(argv[1], " ");
    while (vector[i] != NULL){
        vector[i] = strtok(NULL, " ");
        i++;
    }
    vector[i] = NULL;
    
    pid_t child = fork();
    if(child == 0){
        ptrace(PTRACE_TRACEME);
        kill(getpid(), SIGSTOP);    
        execvp(vector[0], vector);
    
    
    }else{    
    
        int status,syscall_num;
        //I'm the parent...keep tabs on that child process
        //wait for the child to stop itself


        while(1){

            //Request: I want to wait for a system call
                ptrace(PTRACE_SYSCALL, child, 0, 0);

                //actually wait for child status to change
                waitpid(child, &status, 0);


                //this option makes it easier to distinguish normal traps from
                //system calls
                ptrace(PTRACE_SETOPTIONS, child, 0,
                        PTRACE_O_TRACESYSGOOD);
            
            do{

                //there are differented reasons that a child's
                //status might change. Check to see if the child
                //exited
                if (WIFEXITED(status)) {
                    //the child exited...let's exit too
                    print_array(array, argv);
                    exit(1);
                }
                
                //wait until the process is stopped or bit 7 is set in
                //the status (see man page comment on
                //PTRACE_O_TRACESYSGOOD)
            } while (!(WIFSTOPPED(status) && WSTOPSIG(status) & 0x80));

            //read out the saved value of the RAX register,
            //which contains the system call number
            //For 32-bit machines, you would use EAX.
            syscall_num = ptrace(PTRACE_PEEKUSER, child, sizeof(long)*ORIG_RAX, NULL);
            array[syscall_num]++;
            
            //for this example, I only want the first
            //system call. So...
            //let the child run to completion
            //ptrace(PTRACE_CONT, child, NULL, NULL);
            //waitpid(child, NULL, 0);

        }
    }
    return 0;
}


void print_array(int array[], char** argv){
    FILE *output;
    output = fopen(argv[2], "w+");
    for(int i = 0; i < max_array_size - 1; ++i){
        if(array[i] > 0)
            fprintf(output, "%d\t%d\n", i, array[i]);
    }
    fclose(output);
}
