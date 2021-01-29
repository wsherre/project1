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

#define max_array_size 336
#define vector_size 50
typedef struct scall{
    int call;
    int num_of_calls;
}scall;

int find_call(int, scall array[], int);
void print_array(scall array[], int, char**);
void quicksort(scall number[25],int first,int last);


int main(int argc, char** argv){
    scall array[max_array_size];
    int size_of_array = 0;
    
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
        
        
            int array_place = 0;
            while(!WIFEXITED(status)){
                ptrace(PTRACE_SYSCALL, child, 0, 0);    
                //wait for status to change     
                waitpid(child, &status, 0);
                syscall_num = ptrace(PTRACE_PEEKUSER, child, sizeof(long)*ORIG_RAX, NULL);
                array_place = find_call(syscall_num, array, size_of_array);
                if(array_place != -1){
                    array[array_place].num_of_calls++;
                }else{
                    array[size_of_array].call = syscall_num;
                    array[size_of_array].num_of_calls = 1;
                    size_of_array++;
                }
            }
            quicksort(array, 0, size_of_array - 1);
            print_array(array, size_of_array, argv);
    }

    return 0;
}

int find_call(int call, scall array[], int size_of_array){

    for(int i = 0; i < size_of_array; ++i){
        if(array[i].call == call){
            return i;
        }
    }
    return -1;
}

void print_array(scall array[], int size_of_array, char** argv){
    FILE *output;
    output = fopen(argv[2], "w+");
    for(int i = 0; i < size_of_array; ++i){
        fprintf(output, "%d %d\n", array[i].call, array[i].num_of_calls);
    }
    fclose(output);
}

void quicksort(scall number[25],int first,int last){
   int i, j, pivot;
   scall temp;

   if(first<last){
      pivot=first;
      i=first;
      j=last;

      while(i<j){
         while(number[i].call<=number[pivot].call&&i<last)
            i++;
         while(number[j].call>number[pivot].call)
            j--;
         if(i<j){
            temp.call=number[i].call;
            temp.num_of_calls = number[i].num_of_calls;
            number[i].call=number[j].call;
            number[i].num_of_calls=number[j].num_of_calls;
            number[j].call=temp.call;
            number[j].num_of_calls=temp.num_of_calls;
         }
      }

        temp.call=number[pivot].call;
        temp.num_of_calls = number[pivot].num_of_calls;
        number[pivot].call=number[j].call;
        number[pivot].num_of_calls=number[j].num_of_calls;
        number[j].call=temp.call;
        number[j].num_of_calls=temp.num_of_calls;

        quicksort(number,first,j-1);
        quicksort(number,j+1,last);

   }
}