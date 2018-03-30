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

int* entering; //boolean array
int* tickets;
t_info* infos;

volatile int in_cs;

int sleepFlag;

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
	int num_threads = atoi(argv[1]);
	int num_seconds = atoi(argv[2]);

	if(num_threads == 0 || num_seconds == 0)
	{
		printf("Invalid arguments.\n");
		return 1;
	}

	infos  = malloc(num_threads * sizeof(t_info));
	entering = malloc(num_threads * sizeof(int));
	tickets  = malloc(num_threads * sizeof(int));

	pthread_t threads[num_threads];

	if(entering == NULL || tickets == NULL)
	{
		printf("malloc failed \n");
		return 1;
	}
 
 	memset(entering, false, num_threads * sizeof(int));
 	memset(tickets, 0, num_threads * sizeof(int));
 	
 	in_cs = 0;
 	sleepFlag = 1;

 	int i;
 	for(i = 0; i < num_threads; i++)
 	{
 		infos[i].n = 0;
 		infos[i].tnum = i;
 		pthread_create(&threads[i], NULL, &Thread, (void *)&infos[i]);
 	}
 	
 	sleep(num_seconds);
 	
 	for(i = 0; i < num_threads; i++)
 	{
 		pthread_join(threads[i], NULL);
 		printf("Thread %d entered the critical section %d times!\n", i, (int)infos[i].n );
 	}

 	sleepFlag = 0;
 	
 	return 0;
}

void lock(int i)
{
	 entering[i] = true;
     
     int max = 0;
     int k;
     for (k = 0; k < num_threads; k++)
     {
         max = tickets[k] > max ? tickets[k] : max;
     }
     tickets[i] =  1 + max; 
     
     entering[i] = false;

     int j;
     for (j = 0; j < num_threads; ++j)
     {
        while (entering[j]) 
        { /* nothing */ }

        while ( (tickets[j] != 0) && tickets[j] < tickets[i] )
        { /* nothing */ }
     }
}

void unlock(int i)
{
	tickets[i] = 0;
}

void * Thread(void * info)
{
	int tn = ((t_info*)info)->tnum;
	//printf("pid = %d and n = %d\n",ipid ,(((t_info*)info)->n));

	while(sleepFlag)
	{	
		lock(tn);
			//(((t_info*)info)->n)++;
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