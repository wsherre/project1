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

#define max_array_size 320
typedef struct scall{
    int call;
    int num_of_calls;
}scall;

int find_call(int, scall array[], int);
void print_array(scall array[], int);


int main(int argc, char** argv){
    scall array[max_array_size];
    int size_of_array = 0;
    
    char *vector[50];
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
        //printf("hey\n");
        //int child = getpid();
    
    }else{    
    
        int status,syscall_num;      
        waitpid(child, &status, 0);        
        ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_TRACESYSGOOD);

        do{         
            //wait for syscall
            ptrace(PTRACE_SYSCALL, child, 0, 0);    
            //wait for status to change     
            waitpid(child, &status, 0);
     
            //exit if child exits
            if (WIFEXITED(status)) {
                exit(1);
            }
            
        //for this example, I only want the first
        //system call. So...
        //let the child run to completion
        
        } while (!(WIFSTOPPED(status) &&
            WSTOPSIG(status) & 0x80));
        
        
            int array_place = 0;
            while(!WIFEXITED(status)){
                ptrace(PTRACE_SYSCALL, child, 0, 0);    
                //wait for status to change     
                waitpid(child, &status, 0);
                syscall_num = ptrace(PTRACE_PEEKUSER, child, sizeof(long)*ORIG_RAX, NULL);
                array_place = find_call(syscall_num, array, size_of_array);
                if(array_place > 0){
                    array[array_place].num_of_calls++;
                }else{
                    array[size_of_array].call = syscall_num;
                    array[size_of_array].num_of_calls = 1;
                    size_of_array++;
                }
            }
            print_array(array, size_of_array);
        //for this example, I only want the first
        //system call. So...
        //let the child run to completion
        //ptrace(PTRACE_CONT, child, NULL, NULL);
        //waitpid(child, NULL, 0);
    }
    


}

int find_call(int call, scall array[], int size_of_array){

    for(int i = 0; i < size_of_array; ++i){
        if(array[i].call == call){
            return i;
        }
    }
    return 0;
}

void print_array(scall array[], int size_of_array){
    for(int i = 0; i < size_of_array; ++i){
        fprintf(stderr, "%d %d\n", array[i].call, array[i].num_of_calls);
    }
}