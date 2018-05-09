/*
  Devroop Banerjee
  V00837868
  CSC 360 - Assignment 2
  References - http://www.codingdevil.com/2014/04/c-program-for-reader-writer-problem.html
  https://www.youtube.com/watch?v=kn2Okc2cAaQ
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include "rw.h"
#include "resource.h"

/*
 * Declarations for reader-writer shared variables -- plus concurrency-control
 * variables -- must START here.
 */ 

static resource_t data;
sem_t semaphore1, semaphore2;
int count = 0;
	
void initialize_readers_writer() {
    /*
     * Initialize the shared structures, including those used for
     * synchronization.
     */
	init_resource(&data, "Readers/Writers Problem");
	sem_init(&semaphore1, 0, 1);
	sem_init(&semaphore2, 0, 1);
}


void rw_read(char *value, int len) {
	sem_wait(&semaphore1);
	count++;
	if(count == 1){
		sem_wait(&semaphore2);
	}
	sem_post(&semaphore1);
	
	read_resource(&data, value, len);
	
	sem_wait(&semaphore1);
	count--;
	if(count == 0){
		sem_post(&semaphore2);
	}
	sem_post(&semaphore1);
}


void rw_write(char *value, int len) {
	sem_wait(&semaphore1);
	count++;
	if(count == 1){
		sem_wait(&semaphore2);
	}
	sem_post(&semaphore1);

	write_resource(&data, value, len);

	sem_wait(&semaphore1);
	count--;
	if(count == 0){
		sem_post(&semaphore2);
	}
	sem_post(&semaphore1);
}
