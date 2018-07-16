//gcc -std=c99 candykids.c bbuff.c stats.c -lpthread -lrt -D_POSIX_C_SOURCE=199309L

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

#include "bbuff.h"
#include "stats.h"



_Bool stop_thread = false;


typedef struct{
	int factory_number;
	double time_stamp_in_ms;
}candy_t;



double current_time_in_ms(void){
	struct timespec now;
	clock_gettime(CLOCK_REALTIME, &now);
	return now.tv_sec * 1000.0 + now.tv_nsec/1000000.0;
}



void* kid_function(){
	while(1){
		candy_t *candy = bbuff_blocking_extract();
		int nap = rand() % 2;

		stats_record_consumed(candy->factory_number, 
			current_time_in_ms() - candy->time_stamp_in_ms);

		sleep(nap);
		free(candy);
	}
	pthread_exit(NULL);
}

void* factory_function(void* num){
	int factory_num = 0;
	while(stop_thread != true){				
		int *count_ptr = (int*) num;
		int count = *count_ptr;
		factory_num = count;		

		int random = rand() % 4;
		printf("\tFactory %d ships candy & waits %ds\n", count, random);

		candy_t *candy = malloc(sizeof(candy_t));
		if(candy==NULL){
			printf("malloc failed\n");
			exit(-1);
		}
		candy->factory_number = count;
		candy->time_stamp_in_ms = current_time_in_ms();

		bbuff_blocking_insert(candy);
		stats_record_produced(factory_num);

		sleep(random);				

	}
	printf("Candy-factory %d done\n", factory_num);
	pthread_exit(NULL);
}



int main(int argc, char *argv[]){
	if (argc < 4) {
		printf("number of arguments cannot be less than 4\n");
		exit(-1);
	}

	int num_factories = atoi(argv[1]);
	int num_kids = atoi(argv[2]);
	int num_seconds = atoi(argv[3]);

	if(num_factories <= 0 || num_kids <= 0 || num_seconds <= 0){
		printf("ERROR: arguments cannot be less than 1\n");
		exit(-1);
	}

	bbuff_init();
	stats_init(num_factories);



	pthread_t factory_array[num_factories];
	int factory_number_array[num_factories];
	for(int i=0; i<num_factories; i++){
		factory_number_array[i] = i;
		pthread_create(&factory_array[i], NULL, factory_function, 
			&factory_number_array[i]);
	}
	

	pthread_t kid_array[num_kids];
	for(int l=0; l<num_kids; l++)
		pthread_create(&kid_array[l], NULL, kid_function, NULL);


	for(int time=0; time<num_seconds; time++){
		sleep(1);
		printf("Time %ds\n", time);
	}


	stop_thread = true;

	for(int j=0; j<num_factories; j++)
		pthread_join(factory_array[j], NULL);

	while(bbuff_is_empty() != true){
		printf("“Waiting for all candy to be consumed\n");
		sleep(1);
	}

	for(int k=0; k<num_kids; k++){
		pthread_cancel(kid_array[k]);
		pthread_join(kid_array[k], NULL);	
	}
	
	stats_display();
	stats_cleanup();


	return 0;
}

