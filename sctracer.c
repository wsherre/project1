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

    //need at least executable, executable to trace, output file
    if(argc < 3){
        fprintf(stderr, "ERROR: must have at least 3 arguments\n");
        exit(1);
    }
    //make an array that will hold the syscalls. the syscall number will be the index of
    //the array. the value will be the amount of syscalls are made
    int array[max_array_size];
    //initialize to 0
    for(int i = 0; i < max_array_size; ++i){
        array[i] = 0;
    }
    

    //create a vector for the second argument. this will take the string and separate it
    char *vector[vector_size];
    int i = 1;
    vector[0] = strtok(argv[1], " ");
    while (vector[i] != NULL){
        vector[i] = strtok(NULL, " ");
        i++;
    }
    vector[i] = NULL;
    
    //fork
    pid_t child = fork();
    if(child == 0){
        //allow the child to be traced
        ptrace(PTRACE_TRACEME);
        //give the parent time to trace
        kill(getpid(), SIGSTOP);   
        //run the thing 
        execvp(vector[0], vector);
    
    
    }else{    
        int status,syscall_num;

        while(1){

                //request syscall
                ptrace(PTRACE_SYSCALL, child, 0, 0);

                //wait for child to stop
                waitpid(child, &status, 0);


                //set the option to check for traps
                ptrace(PTRACE_SETOPTIONS, child, 0,
                        PTRACE_O_TRACESYSGOOD);
            
            do{
                //request syscall
                ptrace(PTRACE_SYSCALL, child, 0, 0);

                //actually wait for child status to change
                waitpid(child, &status, 0);
                

                //if child exits print array to output file and exit
                if (WIFEXITED(status)) {
                    //the child exited...let's exit too
                    print_array(array, argv);
                    exit(1);
                }
                
                //wait until the process is stopped or bit 7 is set in status
            } while (!(WIFSTOPPED(status) && WSTOPSIG(status) & 0x80));

            //read out the saved value of the RAX register,
            syscall_num = ptrace(PTRACE_PEEKUSER, child, sizeof(long)*ORIG_RAX, NULL);
            //update numerical value in the array
            array[syscall_num]++;

        }
    }
    return 0;
}


void print_array(int array[], char** argv){
    //just print the array out to the output file
    FILE *output;
    output = fopen(argv[2], "w+");
    for(int i = 0; i < max_array_size - 1; ++i){
        if(array[i] > 0)
            fprintf(output, "%d\t%d\n", i, array[i]);
    }
    fclose(output);
}
