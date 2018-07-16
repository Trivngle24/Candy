#include <stdio.h>
#include <stdlib.h>
#include "stats.h"


int num_factories;
int *num_made;
int *num_eaten;
double *min_delay;
double *avg_delay;
double *max_delay;
int *num_consumed;


void stats_init(int num_producers){
	
	num_factories = num_producers;
	num_made = (int*)calloc(num_producers, sizeof(int));
	num_eaten = (int*)calloc(num_producers, sizeof(int));
	min_delay = (double*)malloc(num_producers*sizeof(double));
	avg_delay = (double*)calloc(num_producers, sizeof(double));
	max_delay = (double*)calloc(num_producers, sizeof(double));
	num_consumed = (int*)calloc(num_producers, sizeof(int));
	if(num_made == NULL || num_eaten == NULL ||
		min_delay == NULL || avg_delay == NULL ||
		max_delay == NULL || num_consumed == NULL){
		printf("stats malloc failed\n");
		exit(-1);
	}

	for (int i = 0; i < num_producers; i++)
		min_delay[i] = 2000000;
	

}

void stats_cleanup(void){
	free(num_made);
	free(num_eaten);
	free(min_delay);
	free(avg_delay);
	free(max_delay);
	free(num_consumed);
}

void stats_record_produced(int factory_number){
	num_made[factory_number]++;
}

void stats_record_consumed(int factory_number, double delay_in_ms){
	num_eaten[factory_number]++;

	if (delay_in_ms < min_delay[factory_number]) 
		min_delay[factory_number] =  delay_in_ms;		
		
	if (delay_in_ms > max_delay[factory_number])
		max_delay[factory_number] = delay_in_ms;		
	
	num_consumed[factory_number]++;
	avg_delay[factory_number] += delay_in_ms;
	avg_delay[factory_number] = avg_delay[factory_number]/num_consumed[factory_number];

}

void stats_display(void){
	printf("%8s %7s %8s %15s %15s %15s \n", 
		"Factory#", "#Made", "#Eaten", "Min Delay[ms]", 
		"Avg Delay[ms]", "Max Delay[ms]");

	for(int i=0; i<num_factories; i++){
		printf("%4d %9d %8d %15f %15f %15f \n", 
			i, num_made[i], num_eaten[i]
			, min_delay[i], avg_delay[i], max_delay[i]);
		if (num_made[i] != num_eaten[i]){
			printf("ERROR: Mismatch between number made and eate");
			exit(-1);		
		}
		
	}
}