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

//highest syscall number in linux(335) + 1 for #0 , + 1 for #-1
#define max_array_size 337
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
        waitpid(child, &status, 0);        
        ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_TRACESYSGOOD);
        
        
        while(!WIFEXITED(status)){
            ptrace(PTRACE_SYSCALL, child, 0, 0);    
            //wait for status to change     
            waitpid(child, &status, 0);
            syscall_num = ptrace(PTRACE_PEEKUSER, child, sizeof(long)*ORIG_RAX, NULL);
            
            if(syscall_num == -1)
                array[max_array_size - 1]++;
            else
                array[syscall_num]++;
            print_array(array, argv);
        }
    }
    return 0;
}


void print_array(int array[], char** argv){
    FILE *output;
    output = fopen(argv[2], "w+");
    if(array[max_array_size - 1] > 0)
        fprintf(output, "%d\t%d\n", -1, array[max_array_size - 1]);
    for(int i = 0; i < max_array_size - 1; ++i){
        if(array[i] > 0)
            fprintf(output, "%d\t%d\n", i, array[i]);
    }
    fclose(output);
}
