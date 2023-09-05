#include "memory.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

size_t round_up(size_t size)
{
    return (size + FRAME_SIZE - 1) / FRAME_SIZE * FRAME_SIZE;
}

size_t num_c_offset = 48;
size_t num_frames = 0;
void* data_ptr = NULL;
char* info_ptr = NULL;

void memory_init(void* buffer, size_t size)
{
    // UPDATE THIS
    size_t info_per_frame = sizeof(char);

    num_frames = size / FRAME_SIZE; // initial estimate
    info_ptr = buffer;
    data_ptr = (void*)info_ptr + round_up(info_per_frame * num_frames);
    num_frames = (buffer + size - data_ptr) / FRAME_SIZE; // real value
    
    char frames[num_frames];
    
    for(int i = 0; i < num_frames; i++){
        frames[i] = '0';
    }
    
    //Assigning the char array to info_ptr
    strcpy(info_ptr, frames);
    
    // FINISH HERE
}

size_t memory_avail()
{
    size_t free_space = 0; 
    for(size_t i = 0; i < num_frames; i++){
        if(info_ptr[i] == '0'){
            free_space += FRAME_SIZE;
        }
    }
    //memory available is number of free spaces ('0') * FRAME_SIZE
    return free_space;
}

void* memory_alloc(size_t size)
{
    // char value of '0' means the slot is free
    // char value of anything greater than '0' means that it is filled. 
    // The char value assigned is index + 1 converted into char
    // If the memory is contiguous then the index is filled with the same char value
    // of the starting frame.
    
    if(size <= 0){
        return NULL;   
    }else if(size > memory_avail()){
        return NULL;
    }else if(size <= FRAME_SIZE){
    
        //For storing element that can fit in one frame ( <= FRAME_SIZE)
        
        for(int i = 0; i < num_frames; i++){
            if( info_ptr[i] == '0'){
                info_ptr[i] = (char)(num_c_offset+i+1);
                return data_ptr + FRAME_SIZE*i; 
            }
        }
    }else{
    
        //For storing contiguous elements 
        
        size_t conti_frames = round_up(size) / FRAME_SIZE; 
        size_t count = 0;
        for (int i = 0; i < num_frames; i++){
        
            //look for contigous spots with count
            
            if( info_ptr[i] == '0'){
                count++;
            }else{
                count = 0;
            } 
            if(count == conti_frames){
                size_t index = i - count + 1;
                
                //starting index to assign char value
                //Assign the corressponding char values to the indexes
                
                for(int j = index; j <= i; j++){
                    info_ptr[j] = (char)(num_c_offset + index +1);   
                }
                
                return data_ptr + FRAME_SIZE*index; 
            }
        }
    }
    return NULL;
}

void memory_free(void* ptr)
{
    //No pointer to free the memory
    if(ptr == NULL){
        return; 
    }
    
    //If the given pointer is not inside the appropriate boundary, do nothing 
    //If the given pointer is not multiple of FRAME_SIZE, do nothing
    //If the given pointer is not the start of the contiguous memory segment, 
    //do nothing
    
    if(ptr > (data_ptr + FRAME_SIZE * num_frames) || (ptr < data_ptr) ){
    	return;
    }else if( ( (ptr-data_ptr) % FRAME_SIZE ) != 0){
        return;
    }else{
    
        //free the contiguous segment of memory
        
        size_t find_index = ((ptr-data_ptr) / FRAME_SIZE) + 1;
        
        for(int i = find_index - 1; i < num_frames; i++){
            if(info_ptr[i] == (char)(find_index + num_c_offset)){
                info_ptr[i] = '0';
            }else{
                //if the element does not correpond return immediately
                //because it is done freeing the memory or the given ptr is
                //not the start of the contigious memory segment.
                return;
            }
        }
    }
}

void test3()
{
    printf("TEST 3 STARTED\n");
    size_t avail_space = memory_avail();
    size_t allocate_half_cont = avail_space / 2; 
    size_t allocate_next = avail_space / 10; 
  
    //First allocate half of the total available memory
    //the first 5 consecutive frames would be filled
    
    char* ptr_first_cont = memory_alloc(allocate_half_cont);
    printf("allocated %ld memory at %p\n", allocate_half_cont, ptr_first_cont);

    //Allocate 1 frame of memory right after
    char* ptr_mid = memory_alloc(allocate_next);
    printf("allocated another %ld memory at %p\n", allocate_next, ptr_mid);
    
    //print the available memory left
    size_t avail_space_left = memory_avail();
    printf("available memory after alloc is %ld\n", avail_space_left);

    //free the first 5 consecutive frames in the memory
    memory_free(ptr_first_cont);
    printf("memory is freed at %p\n", ptr_first_cont);
    
    //There would be only one frame filled in the middle
    size_t avail_space_left_freed = memory_avail();
    printf("available memory after freeing is %ld\n", avail_space_left_freed);

    //Try to allocate memory that is 6/10 of the total available memory
    //Although there is enough memory space left, there is no consecutive segments of memory
    //left because of the middle frame being occupied
    size_t try_allocate = avail_space / 10 * 6;
    char* ptr_try = memory_alloc(try_allocate);
    
    printf("Trying to allocate memory space of %ld.\n", try_allocate);
    
    //If ptr_try is NULL then it means it failed to allocate memory, 
    //therefore confirming that the request couldn't be satisfied because the
    //memory is not contigiously available
    
    if(ptr_try == NULL){
        printf("TEST 3 PASSED\n");
        printf("Failed to allocate memory because the memory is not contiguously available\n");
    }else{
        printf("TEST 3 FAILED\n");
        memory_free(ptr_try);
    }
    
    memory_free(ptr_mid);
    
}
