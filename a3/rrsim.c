/*
    Devroop Banerjee
    V00837868
    CSC 360 - Assignment 3 - Round Robin Scheduler
    Credits - Tutorial + Lecture slides + Dr.Zastre's offfice hours
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedlist.h"

#define MAX_BUFFER_LEN 80

taskval_t *event_list = NULL;

void print_task(taskval_t *t, void *arg) {
    printf("id = %05d\treq = %5.2f\tused = %5.2f\n",
        t->id,
        t->cpu_request,
        t->cpu_used
    );
}


void increment_count(taskval_t *t, void *arg) {
    int *ip;
    ip = (int *)arg;
    (*ip)++;
}


void run_simulation(int qlen, int dlen) {
    taskval_t *ready_q = NULL;
    taskval_t *temp;
    int i, quantum, id, count = 0;
    float waiting_time, turnaround = 0.0;

        //infitine loop only breaks when both event_list &
        //ready_q are NULL ie:- no tasks waiting/remaining
    for(;;){
        if(ready_q == NULL && event_list == NULL){
            break;
        }

            //if system is idle, add it to ready-q to be processed later
        if((ready_q != NULL && event_list != NULL) && (event_list->arrival_time > quantum)){
            while(event_list != NULL){
                temp = new_task();
                temp->id = event_list->id;
                temp->arrival_time = event_list->arrival_time;
                temp->cpu_request = event_list->cpu_request;
                temp->cpu_used = event_list->cpu_used;
                temp->finish_time = event_list->finish_time;

                ready_q = add_end(ready_q, temp);
                event_list = remove_front(event_list);
            }
        }
            //when there are no tasks queued and the next task
            //hasn't arrived yet, print "IDLE"
        if((ready_q == NULL) && (event_list != NULL && event_list->arrival_time > quantum)){
        	printf("[%05d]\t\tIDLE\n", quantum);
        	quantum++;
            continue;
        }

        if((event_list != NULL) && (id == 0 || id != event_list->id)){
            for(i = 0; i < dlen; i++){
        		printf("[%05d]\t\tDISPATCHING\n", quantum);
                count = 0;
        		quantum++;
        	}
        }

            //process event_list first
        if(event_list != NULL){
            id = event_list->id;
                //process tasks on event_list for the number of quanta specified

            if(event_list->cpu_used < event_list->cpu_request){
                printf("[%05d]\t\t", quantum);
            	print_task(event_list, (int *)MAX_BUFFER_LEN);
            	event_list->cpu_used++;
            	quantum++;
                count++;
                event_list->finish_time++;
                    //if a task arrives when another task is being processed,
                    //add the both tasks to the ready_q
                if(event_list->next != NULL && event_list->next->arrival_time == quantum){
                    while(event_list != NULL){
                        temp = new_task();
                        temp->id = event_list->id;
                        temp->arrival_time = event_list->arrival_time;
                        temp->cpu_request = event_list->cpu_request;
                        temp->cpu_used = event_list->cpu_used;
                        temp->finish_time = event_list->finish_time;

                        ready_q = add_end(ready_q, temp);
                        event_list = remove_front(event_list);
                    }
                    //if task runs for qlen and doesn't finish then
                    //it's added to the ready_q and removed from event_list
                }else if(count == qlen){
        			temp = new_task();
        			temp->id = event_list->id;
        			temp->arrival_time = event_list->arrival_time;
        			temp->cpu_request = event_list->cpu_request;
        			temp->cpu_used = event_list->cpu_used;
                    temp->finish_time = event_list->finish_time;

        			ready_q = add_end(ready_q, temp);
        			event_list = remove_front(event_list);
        		}
                //if task finished before running for qlen quanta, print "EXIT"
            }else if(event_list->cpu_used >= event_list->cpu_request){
                waiting_time = ((float)quantum - event_list->arrival_time)
                                + (event_list->cpu_used - event_list->cpu_request)
                                - event_list->finish_time;
        		turnaround = waiting_time + event_list->cpu_request;

                printf("[%05d]\t\tid = %05d\tEXIT\tw = %5.2f\tta = %5.2f\n",
        			quantum,
        			event_list->id,
        			event_list->arrival_time,
        			waiting_time,
        			turnaround
        		);
        		event_list = remove_front(event_list);
            }
        }

            //process ready_q
        if(event_list == NULL && ready_q != NULL){
            if((event_list == NULL) && (ready_q->arrival_time > quantum)){
                printf("[%05d]\t\tIDLE\n", quantum);
            	quantum++;
                continue;
            }

            if(id == 0 || id != ready_q->id){
                for(i = 0; i < dlen; i++){
                    printf("[%05d]\t\tDISPATCHING\n", quantum);
                    count = 0;
                    quantum++;
                }
            }
            id = ready_q->id;

            if(ready_q->cpu_used < ready_q->cpu_request){
                printf("[%05d]\t\t", quantum);
            	print_task(ready_q, (int *)MAX_BUFFER_LEN);
            	ready_q->cpu_used++;
            	quantum++;
                count++;
                ready_q->finish_time++;

                    //if task runs for qlen and doesn't finish then
                    //it's added to the ready_q and removed from event_list
                if(count == qlen){
            	    temp = new_task();
            	    temp->id = ready_q->id;
            		temp->arrival_time = ready_q->arrival_time;
            		temp->cpu_request = ready_q->cpu_request;
            		temp->cpu_used = ready_q->cpu_used;
                    temp->finish_time = ready_q->finish_time;

            		ready_q = add_end(ready_q, temp);
            		ready_q = remove_front(ready_q);
                }
                //if task finished before running for qlen quanta, print "EXIT"
            }else if(ready_q->cpu_used >= ready_q->cpu_request){
                waiting_time = ((float)quantum - ready_q->arrival_time)
                                + (ready_q->cpu_used - ready_q->cpu_request)
                                - ready_q->finish_time;
        		turnaround = waiting_time + ready_q->cpu_request;

                printf("[%05d]\t\tid = %05d\tEXIT\tw = %5.2f\tta = %5.2f\n",
        			quantum,
        			ready_q->id,
        			ready_q->arrival_time,
        			waiting_time,
        			turnaround
        		);
        		ready_q = remove_front(ready_q);
            }
        }
    }
}


int main(int argc, char *argv[]) {
    char   input_line[MAX_BUFFER_LEN];
    int    i;
    int    task_num;
    int    task_arrival;
    float  task_cpu;
    int    quantum_length = -1;
    int    dispatch_length = -1;

    taskval_t *temp_task;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--quantum") == 0 && i+1 < argc) {
            quantum_length = atoi(argv[i+1]);
        }
        else if (strcmp(argv[i], "--dispatch") == 0 && i+1 < argc) {
            dispatch_length = atoi(argv[i+1]);
        }
    }

    if (quantum_length == -1 || dispatch_length == -1) {
        fprintf(stderr,
            "usage: %s --quantum <num> --dispatch <num>\n",
            argv[0]);
        exit(1);
    }


    while(fgets(input_line, MAX_BUFFER_LEN, stdin)) {
        sscanf(input_line, "%d %d %f", &task_num, &task_arrival,
            &task_cpu);
        temp_task = new_task();
        temp_task->id = task_num;
        temp_task->arrival_time = task_arrival;
        temp_task->cpu_request = task_cpu;
        temp_task->cpu_used = 0.0;
        event_list = add_end(event_list, temp_task);
        printf("%5d  %5d  %5.1f\n", task_num, task_arrival, task_cpu);
    }

#ifdef DEBUG
    int num_events = 0;
    apply(event_list, increment_count, &num_events);
    printf("DEBUG: # of events read into list -- %d\n", num_events);
    printf("DEBUG: value of quantum length -- %d\n", quantum_length);
    printf("DEBUG: value of dispatch length -- %d\n", dispatch_length);
#endif

    run_simulation(quantum_length, dispatch_length);

    return (0);
}
