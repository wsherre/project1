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
    struct list *next;
}list;


//globals such as head and size of list
list *head = NULL;
int check_for_leak = 0;
int list_size = 0;

void lib_init(){
    //ensures the list will only add nodes after the library is initialized
    check_for_leak = 1;
}

void lib_destroy(){
    //just a reset
    check_for_leak = 0;
    int total = 0, total_bytes = 0, current_bytes = 0;
    list* temp = head;
    list* kill = NULL;
    //open the input file
    FILE*in = fopen("in.txt", "w+");
    //loops through the remaining list items and prints them out as a leak
    //if size is 0 then there are no leaks
    if(list_size > 0){
        
        //loop through list printing leaks to file
        while(temp != NULL){
            total++;
            total_bytes += temp->data;
            current_bytes = temp->data;
            current_bytes = temp->data;
            kill = temp;
            temp = temp->next;
            real_free(kill);
            fprintf(in,"%d\n", current_bytes);
        }
        //print break point
        fprintf(in, "-1\n");
        //print the total
        fprintf(in, "%d %d\n", total, total_bytes);
    }else{
        fprintf(in, "-1\n0 0\n");
    }
    //close the file
    fclose(in);
}

//remove the node from the linked list and free the data from real memory
void free(void* ptr){

    if(real_free == NULL){
        real_free = dlsym(RTLD_NEXT, "free");
    }
    if(list_size > 0){
        remove_node(ptr);
        list_size--;
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
    if(check_for_leak == 1){
        add_node(size, p);
        list_size++;
    }
    return p;
}


//add a new node at the end of the linked list
void add_node(int byte, void* ptr){
    list *new_node = real_malloc(sizeof(list));
    new_node->data = byte;
    new_node->address = ptr;
    list* prev = head;
    list* current = head;

    if(head == NULL){
        head = new_node;
    }else{
        current = current->next;

        while(current != NULL){
            prev = current;
            current = current->next;
        }

        prev->next = new_node;
        new_node->next = NULL;
    }
}

//remove the node with the proper memory address 
//then free that memory
void remove_node(void* ptr){
    list* prev = head;
    list* current = head;
    if(list_size > 0){
        if(list_size > 1){ 
            current = current->next;
        }
        if(head->address == ptr && list_size == 1){
            real_free(head);
            head = NULL;
        }else if(head->address == ptr && list_size > 1){
            list* temp = head;
            head = head->next;
            real_free(temp);
        }else{
            while(current != NULL && ptr != current->address){
                prev = current;
                current = current->next;
            }
            if (current == NULL) return;
            prev->next = current->next;

            real_free(current);
        }
    }
}
