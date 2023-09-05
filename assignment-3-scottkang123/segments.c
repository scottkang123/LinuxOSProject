#include "segments.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//global variables for data segment
char* global_init_string_1 = "data 1";
char* global_init_string_2 = "data 2"; 

//global variables for bss segment
char global_unit_string_1[6];
char global_unit_string_2[6];

void init_pointers()
{

    //text pointer
    text_ptr_1 = *text_pointer_1;
    text_ptr_2 = *text_pointer_2;

    //data pointer
    data_ptr_1 = global_init_string_1;
    data_ptr_2 = global_init_string_2;
  
    //bss pointer (uninitialized data)
    bss_ptr_1 = global_unit_string_1;
    strcpy(bss_ptr_1, "bss 1");
    bss_ptr_2 = global_unit_string_2;
    strcpy(bss_ptr_2, "bss 2");
    
    //heap pointer
    heap_ptr_1 = malloc(7);
    strcpy(heap_ptr_1, "heap 1");
    heap_ptr_2 = malloc(7);
    strcpy(heap_ptr_2, "heap 2");
    
    //stack pointer
    stack_pointer_1();
    
}

char* text_pointer_1(){
    return "text 1";
}

char* text_pointer_2(){
    return "text 2";
}

void stack_pointer_1(){
    char stack1[8];
    stack_ptr_1 = stack1;
    strcpy(stack_ptr_1, "stack 1");
    stack_pointer_2();
    //call stack_pointer_2 inside stack_pointer_1 
    //to prevent reverse ordering
}

void stack_pointer_2(){
    char stack2[8];
    stack_ptr_2 = stack2;
    strcpy(stack_ptr_2, "stack 2");
}

void free_pointers()
{
    //only have to manually free the data in heap segment
    free(heap_ptr_1);
    free(heap_ptr_2);
}
