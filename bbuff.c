#include "bbuff.h"
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdbool.h>

int count_candy;
void* bounded_buffer[BUFFER_SIZE];

sem_t num_space;
sem_t num_candy;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;



void bbuff_init(void){
	sem_init(&num_space, 0, BUFFER_SIZE);

	sem_init(&num_candy, 0, 0);	

	count_candy = 0;
}

void bbuff_blocking_insert(void* item){
	sem_wait(&num_space); //wait till num of space > 0 then decrement
	
	pthread_mutex_lock(&lock);

	bounded_buffer[count_candy++] = item;

	pthread_mutex_unlock(&lock);

	sem_post(&num_candy);
}

void* bbuff_blocking_extract(void){
	sem_wait(&num_candy); 

	pthread_mutex_lock(&lock);

	void* item = bounded_buffer[count_candy-1];
	
	count_candy--;

	pthread_mutex_unlock(&lock);

	sem_post(&num_space);

	return item;
}

_Bool bbuff_is_empty(void){
	if(count_candy == 0)		
		return true;	
	else
		return false;
}