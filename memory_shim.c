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

typedef struct list{
    int data;
    void* address;
    struct list *next;
}list;
list *head = NULL;
int check = 0;

void lib_init(){
    check = 1;
}

void lib_destroy(){
    check = 0;
    FILE* file = fopen("temp.txt", "w+");
    int total = 0, total_bytes = 0, current_bytes = 0;
    list* temp = head;
    list* kill = NULL;
    while(temp != NULL){
        total++;
        total_bytes += temp->data;
	current_bytes = temp->data;
        kill = temp;
        temp = temp->next;
        real_free(kill);
	fprintf(file,"LEAK\t%d\n", current_bytes);
    }
    fprintf(file, "TOTAL\t%d\t%d\n", total, total_bytes);
//    fclose(file);
}

void free(void* ptr){
    
    if(real_free == NULL){
        real_free = dlsym(RTLD_NEXT, "free");
    }
    remove_node(ptr);
    real_free(ptr);
}

void *malloc(size_t size)
{
    if(real_malloc == NULL){
        real_malloc = dlsym(RTLD_NEXT, "malloc");
    }
    void *p = NULL;
    p = real_malloc(size);
    if(check == 1)
        add_node(size, p);
    return p;
}


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

void remove_node(void* ptr){
    list* prev = head;
    list* current = head;

    if(head->next != NULL){ 
        current = current->next;
    }
    if(head->address == ptr && head->next != NULL){
        list* temp = head;
        head = head->next;
        real_free(temp);
    }else if(head->address == ptr && head->next == NULL){
        real_free(head);
        head = NULL;
    }else{
        while(current != NULL && ptr != current->address){
            prev = current;
            current = current->next;
        }
        prev->next = current->next;
        real_free(current);
    }
}
