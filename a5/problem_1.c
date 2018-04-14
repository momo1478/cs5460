#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>

typedef struct t_info 
{
	int tnum;
	int bn;
	int cn;
	int dn;
} t_info;

int num_birds;
int num_cats;
int num_dogs;
int total_animals;

t_info* infos;
pthread_t* threads;

volatile int sleepFlag;

void* Thread(void*);

int main(int argc, char **argv)
{

	if(argc != 4)
	{
		fprintf(stderr,"Incorrect number of args \n.");
		return 1;
	}

    num_cats = strtol(argv[1], NULL, 10);
	num_dogs = strtol(argv[2], NULL, 10);
	num_birds = strtol(argv[3], NULL, 10);

	if(num_cats == 0 || num_dogs == 0 || num_birds == 0)	
	{		
		fprintf(stderr,"Invalid arguments.\n");
		return 1;
	}

	total_animals = num_dogs + num_cats + num_birds;
	if(total_animals > 99 || total_animals < 0)
	{
		fprintf(stderr,"total number of animals must be between 0-99.\n");
		return 1;
	}

	infos    = malloc(total_animals * sizeof(t_info));
	threads  = malloc(total_animals * sizeof(pthread_t));

	if(infos == NULL || threads == NULL)
	{
		fprintf(stderr,"malloc failed \n");
		return 1;
	}
 	
 	int i;
 	for(i = 0; i < total_animals; i++)
 	{
 		infos[i].bn = 0;
 		infos[i].cn = 0;
 		infos[i].dn = 0;
 		infos[i].tnum = i;
 	}

 	sleepFlag = 1;

 	for(i = 0; i < total_animals; i++)
 	{
 		if(pthread_create(&threads[i], NULL, Thread, &infos[i]))
 		{
 			fprintf(stderr, "thread could not be created.\n");
 			return 1;
 		}
 	}

 	sleep(10);

 	sleepFlag = 0;
 	
 	for(i = 0; i < total_animals; i++)
 	{
 		if(pthread_join(threads[i], NULL))
 		{
 			fprintf(stderr, "Could not join thread.\n");
 			return 1;
 		}
 		//printf("Thread %d entered the critical section %d times!\n", i, (int)(infos[i].n) );
 	}
 	return 0;
}

void* Thread(void* info)
{
	return NULL;
}