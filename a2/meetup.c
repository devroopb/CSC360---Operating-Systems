/*
  Devroop Banerjee
  V00837868
  CSC 360 - Assignment 2
  Reference - Lecture Slides
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include "meetup.h"
#include "resource.h"

/*
 * Declarations for barrier shared variables -- plus concurrency-control
 * variables -- must START here.
 */

int count; 
int generation;
static resource_t storage;
int hipsters;
int passcode;


pthread_mutex_t m; 
pthread_cond_t BarrierQueue;

void initialize_meetup(int n, int mf) {
	passcode = mf;
	hipsters = n;
	count = 0;

    if (n < 1) {
        fprintf(stderr, "Who are you kidding?\n");
        fprintf(stderr, "A meetup size of %d??\n", n);
        exit(1);
    }

    /*
     * Initialize the shared structures, including those used for
     * synchronization.
     */
	init_resource(&storage, "Meetup");
	pthread_mutex_init(&m, NULL);
	pthread_cond_init(&BarrierQueue, NULL);
}


void join_meetup(char *value, int len) {
	
	if(passcode == 1){
		pthread_mutex_lock(&m);
		
		count++;
		if(hipsters == 1){
			write_resource(&storage, value, len);
			read_resource(&storage, value, len);
		}else if((count == 1) || (count < hipsters)){
			if(count == 1){
				write_resource(&storage, value, len);
			}
			int my_generation = generation; 
			while (my_generation == generation) {
				read_resource(&storage, value, len);
				pthread_cond_wait(&BarrierQueue, &m);
			}
		}else{
			pthread_cond_broadcast(&BarrierQueue);
			read_resource(&storage, value, len);
			count = 0;			
			generation++;
		}
		pthread_mutex_unlock(&m);

	}else if(passcode == 0){
		pthread_mutex_lock(&m);

		count++;
		if(hipsters == 1){
			write_resource(&storage, value, len);
			read_resource(&storage, value, len);
		}else if(count < hipsters){
			int my_generation = generation; 
			while (my_generation == generation) {
				pthread_cond_wait(&BarrierQueue, &m);
				read_resource(&storage, value, len);
			} 
		}else{
			pthread_cond_broadcast(&BarrierQueue);
			write_resource(&storage, value, len);
			read_resource(&storage, value, len);
			count = 0;			
			generation++;
		}
		pthread_mutex_unlock(&m);
	}
}
