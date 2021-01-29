#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/reg.h>

#define max_array_size 320
typedef struct scall{
    int call;
    int num_of_calls;
}scall;

int find_call(int, scall array[], int);


int main(int argc, char** argv){
    scall array[max_array_size];

    char *token = strtok(argv[1], " ");
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
    }
    if(fork() == 0){
        ptrace(PTRACE_TRACEME);
        kill(getpid(), SIGSTOP);
        int child = getpid();
        clock_t c = clock();
        write(STDOUT_FILENO, "Hello. I'm childish.\n",21);
        printf("time = %ld\n",c);

    }else{
        int status,syscall_num;
        //I'm the parent...keep tabs on that child process

        //wait for the child to stop itself
        waitpid(child, &status, 0);

        //this option makes it easier to distinguish normal traps from
        //system calls
        ptrace(PTRACE_SETOPTIONS, child, 0,
                PTRACE_O_TRACESYSGOOD);

        do{
            //Request: I want to wait for a system call
            ptrace(PTRACE_SYSCALL, child, 0, 0);

            //actually wait for child status to change
            waitpid(child, &status, 0);

            //there are differented reasons that a child's
            //status might change. Check to see if the child
            //exited
            if (WIFEXITED(status)) {
                //the child exited...let's exit too
                exit(1);
            }

            //wait until the process is stopped or bit 7 is set in
            //the status (see man page comment on
            //PTRACE_O_TRACESYSGOOD)
        } while (!(WIFSTOPPED(status) &&
            WSTOPSIG(status) & 0x80));

        //read out the saved value of the RAX register,
        //which contains the system call number
        //For 32-bit machines, you would use EAX.
        syscall_num = ptrace(PTRACE_PEEKUSER,
            child, sizeof(long)*ORIG_RAX, NULL);
        printf("My child called system call #%d.\n",syscall_num);
        
        //for this example, I only want the first
        //system call. So...
        //let the child run to completion
        ptrace(PTRACE_CONT, child, NULL, NULL);
        waitpid(child, NULL, 0);


    }
    

}

int find_call(int call, scall array[], int size_of_array){

    for(int i = 0; i < size_of_array; ++i){
        if(array[i].call == call){
            return 1;
        }
    }
    return 0;
}