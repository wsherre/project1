#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>


void __attribute__((constructor)) lib_init();
void __attribute__((destructor)) lib_destroy();
void remove_node(void*);
void add_node(int, void*);
void* (*real_malloc)(size_t)=NULL;
void (*real_free)(void*)=NULL;

//each malloc call will be held as a struct with a memory address and byte number
//it'll be stored in a link list
typedef struct list{
    size_t data;
    void* address;
    //struct list *next;
}list;
#define max_size 1000000
list array[max_size];
int array_size = 0;

void lib_init(){
    for(int i = 0; i < max_size; ++i){
        array[i].data = 0;
        array[i].address = NULL;
    }
}

void lib_destroy(){
        int total = 0, total_bytes = 0;
        
        for(int i = 0; i < array_size; ++i){
            if(array[i].data > 0){
                fprintf(stderr, "LEAK    %zu\n", array[i].data);
                total_bytes += array[i].data;
                total++;
            }
        }
        fprintf(stderr, "TOTAL   %d  %d\n", total, total_bytes);
}

//remove the node from the linked list and free the data from real memory
void free(void* ptr){
    
    if(real_free == NULL){
        real_free = dlsym(RTLD_NEXT, "free");
    }
    for(int i = 0; i < array_size; ++i){
        if(array[i].address == ptr){
            array[i].data = 0;
            array[i].address = 0x0;
            break;
        }
    }
    real_free(ptr);
}

//assign real memory and add the node into the linked list
void *malloc(size_t size)
{
    if(real_malloc == NULL){
        real_malloc = dlsym(RTLD_NEXT, "malloc");
    }
    void *p = NULL;
    p = real_malloc(size);
    list temp;
    temp.data = size;
    temp.address = p;
    array[array_size] = temp;
    array_size++;
    
    return p;
}

