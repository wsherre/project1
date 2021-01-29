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


int main(int argc, char** argv){
    scall array[max_array_size];

    for(int i = 0; i < argc; ++i){
            argv[i] = argv[i+1];
        }
        argv[argc] = NULL;
    /*char *token = strtok(argv[1], " ");
    char *envp[20];
    int i = 0;
    while(token != NULL){
        envp[i] = token;
        token = strtok(NULL, " ");
        i++;
    }
    envp[i] = NULL;
    for(int k = 0; k < i; ++k){
        printf("%s\n", envp[k]);
    }*/
    pid_t child = fork();
    if(child == 0){
        //ptrace(PTRACE_TRACEME);
        //kill(getpid(), SIGSTOP);
        execvp(argv[0], argv);


    }else{
        /*int status,syscall_num;      
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
            
        } while (!(WIFSTOPPED(status) &&
            WSTOPSIG(status) & 0x80));

        syscall_num = ptrace(PTRACE_PEEKUSER, child, sizeof(long)*ORIG_RAX, NULL);
        printf("My child called system call #%d.\n",syscall_num);
        
        ptrace(PTRACE_CONT, child, NULL, NULL);
        waitpid(child, NULL, 0);


    }
    */

}

int find_call(int call, scall array[], int size_of_array){

    for(int i = 0; i < size_of_array; ++i){
        if(array[i].call == call){
            return 1;
        }
    }
    return 0;
}