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
	int n;
} t_info;

int num_threads;
int num_seconds;

volatile int* entering; //boolean array
volatile int* tickets;
t_info* infos;
pthread_t* threads;

volatile int in_cs = 0;
volatile int sleepFlag;

void lock(int);
void unlock(int);
void* Thread(void*);

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
		printf("Invalid arguments.\n");
		return 1;
	}

	infos    = malloc(num_threads * sizeof(t_info));
	entering = malloc(num_threads * sizeof(int));
	tickets  = malloc(num_threads * sizeof(int));
	threads  = malloc(num_threads * sizeof(pthread_t));

	if(entering == NULL || tickets == NULL)
	{
		printf("malloc failed \n");
		return 1;
	}
 	
 	int i;
 	for(i = 0; i < num_threads;i++)
 	{
 		entering[i] = 0;
 		tickets[i] = 0;
 	}
 	
 	for(i = 0; i < num_threads; i++)
 	{
 		infos[i].n = 0;
 		infos[i].tnum = i;
 	}

 	sleepFlag = 1;

 	for(i = 0; i < num_threads; i++)
 	{
 		pthread_create(&threads[i], NULL, Thread, &infos[i]);
 	}

 	sleep(num_seconds);

 	sleepFlag = 0;
 	
 	for(i = 0; i < num_threads; i++)
 	{
 		pthread_join(threads[i], NULL);
 		printf("Thread %d entered the critical section %d times!\n", i, (int)(infos[i].n) );
 	}
 	return 0;
}

int max()
{
	 int max = 0;
     int k;
     for (k = 0; k < num_threads; k++)
     {
         max = tickets[k] > max ? tickets[k] : max;
     }
     return max;
}

void lock(int i)
{
	 entering[i] = 1;
     
     tickets[i] =  1 + max(NULL); 
     
     entering[i] = 0;

     int j;
     for (j = 0; j < num_threads; j++)
     {
     	//if(i != j)
     	{
	        while (entering[j] != 0) 
	        { /* nothing */ }

	        while ( ( tickets[j] != 0) &&
	        		( tickets[j] < tickets[i]  ||
	                ( tickets[i] == tickets[j] && i < j) 
	                )
	              )
	        { /* nothing */ }
   		}
     }
}

void unlock(int i)
{
	tickets[i] = 0;
}

void * Thread(void * info)
{
	//printf("tnum = %d and n = %d and in_cs = %d\n", tn ,(((t_info*)info)->n), in_cs);
	while(sleepFlag)
	{	
		int tn = ((t_info*)info)->tnum;
		lock(tn);
		(((t_info*)info)->n)++;
			assert (in_cs==0);
	 		in_cs++;
	 		assert (in_cs==1);
	 		in_cs++;
	 		assert (in_cs==2);
	 		in_cs++;
	 		assert (in_cs==3);
	 		in_cs=0;
		unlock(tn);
	}
	return NULL;
}