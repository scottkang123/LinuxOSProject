#include "runtime.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void scheduler_fifo(int time_slice)
{
    int num_jobs = get_num_jobs();
    int arrival = get_current_time();

    float total_turnaround = 0.0f, total_response = 0.0f;
    for (int i = 0; i < num_jobs; i++) {
        int firstrun = get_current_time();
        printf("job %d started at %d\n", i, firstrun);
        total_response += firstrun - arrival;

        int r;
        while ((r = schedule_job(i, time_slice)) >= 0)
            printf("job %d run for %d at %d\n", i, r, get_current_time());

        int completion = get_current_time();
        printf("job %d finished at %d\n", i, completion);

        total_turnaround += completion - arrival;
    }

    printf("Average turnaround time was %f\n", total_turnaround / num_jobs);
    printf("Average response time was %f\n", total_response / num_jobs);
}

void scheduler_rr(int time_slice)
{
    int num_jobs = get_num_jobs();
    int arrival = get_current_time();
    
    float total_turnaround = 0.0f, total_response = 0.0f;
    
    //Initialize the queue
    int queue_size = num_jobs;
    int queue[get_max_num_jobs()];
    int completed_jobs = 0;
    
    int head = 0;
    int tail = num_jobs;
    
    int start_check[get_max_num_jobs()];
    
    for(int i = 0; i < num_jobs; i++){
        start_check[i] = 1; 
        queue[i] = i;
    }
    
    while (completed_jobs < num_jobs) {
        
        int job_id = queue[head];
        head = (head+1)%num_jobs; //increase the head by 1
        
        if(start_check[job_id]){
             int firstrun = get_current_time();
             printf("job %d started at %d\n", job_id, firstrun);
             total_response += firstrun - arrival;
             start_check[job_id] = 0;
        }
        
        int r = schedule_job(job_id, time_slice);
      
        if(r >= 0){
            printf("job %d run for %d at %d\n", job_id, r, get_current_time());
            queue[tail] = job_id; //job is not finished, add it to the end of the queue
            tail = (tail + 1) % num_jobs; //increase the tail by 1
        }else if(r < 0){
            int completion = get_current_time(); 
            printf("job %d finished at %d\n", job_id, completion);
            total_turnaround += completion - arrival;
            completed_jobs++; 
        }
        
    }
    printf("Average turnaround time was %f\n", total_turnaround / num_jobs);
    printf("Average response time was %f\n", total_response / num_jobs);     
}

//helper function to check if there is -1 inside an array. If there is non -1 (1), return 1
int check_if_remaining(int arr[], int size){
    for (int i = 0; i < size; i++) {
        if (arr[i] != -1) {
            return 1;
        }
    }
    return 0;
}

void scheduler_mlfq(int time_slice)
{   
    int num_jobs = get_num_jobs();
    int arrival = get_current_time();

    float total_turnaround = 0.0f, total_response = 0.0f;
    
    //Assign 4 priority queues
    int arr_0[get_max_num_jobs()], arr_1[get_max_num_jobs()], arr_2[get_max_num_jobs()], arr_3[get_max_num_jobs()];
    
    //Initialize them with -1
    //-1 represents there is no corresponding index job. 1 represents there is the corresponding index job
    for(int i = 0; i < get_max_num_jobs(); i++){
        arr_0[i] = -1;
        arr_1[i] = -1;
        arr_2[i] = -1;
        arr_3[i] = -1;
    }
    
    //Array to check if the job has started or not
    int start_check[get_max_num_jobs()];
    
    //Assign all the jobs at the top priority queue
    for(int i = 0; i < num_jobs; i++){
        arr_3[i] = 1;
        start_check[i] = 1;
    }
    
    //initialize the variables
    int completed_jobs = 0;
    int* current_queue = arr_3; //Initialize the currentq ueue to the top priority queue
    int current_level = 3;
    int current_index = 0;
    
    while(completed_jobs < num_jobs){
        
        //Stay in the same priority queue as long as there is 1 (non -1)
        while(check_if_remaining(current_queue, num_jobs)){ 
        
            //Print start statement if the job is processing for the first time       
            if(start_check[current_index] == 1){
                int firstrun = get_current_time();
                printf("job %d started at %d\n", current_index, firstrun);
                total_response += firstrun - arrival;
                start_check[current_index] = 0;
            }
            
            //If the value is 1, then there is a job to do 
            if(current_queue[current_index] == 1){
		int r = schedule_job(current_index, time_slice);
		if(r >= 0){
		    printf("job %d run for %d at %d\n", current_index, r, get_current_time());
		    //If gives up cpu before the time slice then it remains in the same priority queue
		    if(r < time_slice){
		        current_queue[current_index] = 1;
		    }else{
		        if(current_level == 3){
		            current_queue[current_index] = -1;
                            arr_2[current_index] = 1; 
		        }else{
		            current_queue[current_index] = -1;
		            arr_1[current_index] = 1;
		        }
		    } 
		}else if(r < 0){
		    int completion = get_current_time();
		    printf("job %d finished at %d\n", current_index, completion);
		    total_turnaround += completion - arrival;
		    completed_jobs++; 
		    current_queue[current_index] = -1;
		}
            }
            //Increment the current index
            current_index = (current_index+1)% num_jobs;       
        }
        
        //Change the current queue to the one below
        if(current_level == 3){
            current_level = 2;
            current_queue = arr_2;
        }else{
            current_level = 1;
            current_queue = arr_1;
        }
    }
    
    printf("Average turnaround time was %f\n", total_turnaround / num_jobs);
    printf("Average response time was %f\n", total_response / num_jobs);   
}

////////////////////THE EXTRA CREDIT////////////////////////
void scheduler_mlfq_extra_credit(int time_slice)
{   
    int num_jobs = get_num_jobs();
    int arrival = get_current_time();

    float total_turnaround = 0.0f, total_response = 0.0f;
    
    //Assign 4 priority queues
    int arr_0[get_max_num_jobs()], arr_1[get_max_num_jobs()], arr_2[get_max_num_jobs()], arr_3[get_max_num_jobs()];
    
    //Initialize them with -1
    //-1 represents there is no corresponding index job. 1 represents there is the corresponding index job
    for(int i = 0; i < get_max_num_jobs(); i++){
        arr_0[i] = -1;
        arr_1[i] = -1;
        arr_2[i] = -1;
        arr_3[i] = -1;
    }
    
    //Array to check if the job has started or not
    int start_check[get_max_num_jobs()];
    
    //Assign all the jobs at the top priority queue
    for(int i = 0; i < num_jobs; i++){
        arr_3[i] = 1;
        start_check[i] = 1;
    }
    
    //initialize the variables
    int completed_jobs = 0;
    int* current_queue = arr_3; //Initialize the currentq ueue to the top priority queue
    int current_level = 3;
    int current_index = 0;
    int reset_time = 10; //After some time period, move all the jobs in the system to the topmost queue.
    int next_time_to_reset = reset_time;
    
    //Initialize an array to keep track of time remaining for each job 
    //Default is the time_slice
    int time_remaining[get_max_num_jobs()];
    for(int i = 0; i < num_jobs; i++){
        time_remaining[i] = time_slice;
    }
    
    while(completed_jobs < num_jobs){
        
        //Stay in the same priority queue as long as there is 1 (non -1)
        while(check_if_remaining(current_queue, num_jobs)){ 
        
            //CHECK THE TIME PERIOD AND RESET
            if(get_current_time() >= next_time_to_reset){
                next_time_to_reset += reset_time; //Increase the next reset time for the next update
                for(int i = 0; i < num_jobs; i++){
                    if(arr_0[i] == 1 || arr_1[i] == 1 || arr_2[i] == 1 || arr_3[i] == 1){
                        arr_0[i] = -1;
                        arr_1[i] = -1;
                        arr_2[i] = -1;
                        arr_3[i] = 1;
                    }
                }
                current_queue = arr_3;
                current_level = 3;
            } 
            
            //Print start statement if the job is processing for the first time       
            if(start_check[current_index] == 1){
                int firstrun = get_current_time();
                printf("job %d started at %d\n", current_index, firstrun);
                total_response += firstrun - arrival;
                start_check[current_index] = 0;
            }
            
            //If the value is 1, then there is a job to do 
            if(current_queue[current_index] == 1){
		int r = schedule_job(current_index, time_remaining[current_index]);	
		if(r >= 0){
		    printf("job %d run for %d at %d\n", current_index, r, get_current_time());
		    if(r < time_remaining[current_index]){
		        time_remaining[current_index] -= r; // Decrease the time_remaining for that job
		        current_queue[current_index] = 1;
		    }else{
		        time_remaining[current_index] = time_slice;
		        if(current_level == 3){
		            current_queue[current_index] = -1;
                            arr_2[current_index] = 1; 
		        }else{
		            current_queue[current_index] = -1;
		            arr_1[current_index] = 1;
		        }
		    } 
		}else if(r < 0){
		    int completion = get_current_time();
		    printf("job %d finished at %d\n", current_index, completion);
		    total_turnaround += completion - arrival;
		    completed_jobs++; 
		    current_queue[current_index] = -1;
		}
            }
            //Increment the current index
            current_index = (current_index+1)% num_jobs;       
        }
        
        //Change the current queue to the one below
        if(current_level == 3){
            current_level = 2;
            current_queue = arr_2;
        }else{
            current_level = 1;
            current_queue = arr_1;
        }
    }
    
    printf("Average turnaround time was %f\n", total_turnaround / num_jobs);
    printf("Average response time was %f\n", total_response / num_jobs);   
}

void help()
{
    printf("Usage: scheduler [-s seed] [-n jobs] [-t time slice] fifo|rr|mlfq|mlfqex\n");
    exit(1);
}

int main(int argc, char* argv[])
{
    int seed = 4;
    int jobs = 2;
    int time_slice = 9;

    signed char c;
    while ((c = getopt(argc, argv, "t:s:n:h")) != -1) {
        switch (c) {
        
        case 't':
            time_slice = atoi(optarg);
            break;
            
        case 's':
            seed = atoi(optarg);
            break;

        case 'n':
            jobs = atoi(optarg);
            break;

        default:
            help(); 
        }
    }
   
    init_simulation(seed, jobs);

    if (argv[optind] == NULL || strcmp(argv[optind], "fifo") == 0)
        scheduler_fifo(time_slice);
    else if (strcmp(argv[optind], "rr") == 0)
        scheduler_rr(time_slice);
    else if (strcmp(argv[optind], "mlfq") == 0)
        scheduler_mlfq(time_slice);
    else if (strcmp(argv[optind], "mlfqex") == 0)
        scheduler_mlfq_extra_credit(time_slice);
    else
        help();

    return 0;
}
