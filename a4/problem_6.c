#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>

typedef struct t_info 
{
	int tnum;
	int n;
} t_info;

pthread_mutex_t lock;

int num_threads;
int num_seconds;

t_info* infos;
pthread_t* threads;

volatile int in_cs = 0;
volatile int sleepFlag;

volatile int total = 0;
volatile int landed = 0;

void mfence (void) {
  asm volatile ("mfence" : : : "memory");
}

/*
 * atomic_cmpxchg
 * 
 * equivalent to atomic execution of this code:
 *
 * if (*ptr == old) {
 *   *ptr = new;
 *   return old;
 * } else {
 *   return *ptr;
 * }
 *
 */

void * Thread()
{

	while(sleepFlag)
	{		
		double x = (double)rand() / (double)(RAND_MAX);
		double y = (double)rand() / (double)(RAND_MAX);
	    
	    pthread_mutex_lock(&lock);
			total++;

			if( ((x * x) + (y * y)) <= 1)
				landed++;
		pthread_mutex_unlock(&lock);
	}
	return NULL;
}

int main(int argc, char **argv)
{

	if(argc != 3)
	{
		printf("Incorrect number of args \n.");
		return 1;
	}

    num_threads = strtol(argv[1], NULL, 10);
	num_seconds = strtol(argv[2], NULL, 10);

	if(num_threads == 0 || num_seconds == 0)	
	{		
		fprintf(stderr, "Invalid arguments.\n");
		return 1;
	}

	infos    = malloc(num_threads * sizeof(t_info));
	threads  = malloc(num_threads * sizeof(pthread_t));

	if(threads == NULL || infos == NULL)
	{
		fprintf(stderr, "malloc failed \n");
		return 1;
	}
 	
 	int i;

 	if(pthread_mutex_init(&lock, NULL))
 	{
 		fprintf(stderr, "mutex init failed \n");
 		return 1;
 	}

 	sleepFlag = 1;

 	for(i = 0; i < num_threads; i++)
 	{
 		if(pthread_create(&threads[i], NULL, Thread, NULL))
 		{
 			fprintf(stderr, "thread could not be created.\n");
 			return 1;
 		}
 	}

 	sleep(num_seconds);

 	sleepFlag = 0;
 	
 	for(i = 0; i < num_threads; i++)
 	{
 		if(pthread_join(threads[i], NULL))
 		{
 			fprintf(stderr, "Could not join thread.\n");
 			return 1;
 		}
 	}

 	printf("pi approxiamately equals %lf\n", ((double)(4 * landed))/((double)(total)) );
 	return 0;
}