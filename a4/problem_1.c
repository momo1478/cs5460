#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>

int num_threads;
int num_seconds;

int* entering; //boolean array
int* tickets;

volatile int in_cs;

int sleepFlag;

void lock(int);
void unlock(int);
void* Thread(void*);

typedef struct t_info 
{
	pthread_t pid;
	int n;
} t_info;

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

	t_info threads[num_threads]; 

	entering = malloc(num_threads * sizeof(int));
	tickets  = malloc(num_threads * sizeof(int));

	if(entering == NULL || tickets == NULL)
	{
		printf("malloc failed \n");
		return 1;
	}
 
 	memset(entering, 0, num_threads * sizeof(int));
 	memset(tickets, 0, num_threads * sizeof(int));
 	
 	in_cs = 0;
 	sleepFlag = 0;

 	int i;
 	for(i = 0; i < num_threads; i++)
 	{
 		pthread_create(&(threads[i].pid), NULL, &Thread, &threads[i]);
 	}
 	
 	sleep(num_seconds);
 	

 	for(i = 0; i < num_threads; i++)
 	{
 		pthread_join((threads[i].pid), NULL);
 		printf("Thread %d entered the critical section %d times!\n", i, (int)threads[i].n );
 	}
 	sleepFlag = 1;
 	

 	return 0;
}

void lock(int pid)
{
	 entering[pid] = true;
     
     int max = 0;
     int i;
     for (i = 0; i < num_threads; i++)
     {
         max = tickets[i] > max ? tickets[i] : max;
     }
     tickets[pid] =  1 + max; 
     
     entering[pid] = false;

     int j;
     for (j = 0; j < num_threads; ++j)
     {
         if (j != pid)
         {
             while (entering[j]) 
             { /* nothing */ }

             while ( (tickets[j] != 0) && tickets[j] < tickets[j] )
             { /* nothing */ }

         }
     }
}

void unlock(int pid)
{
	tickets[pid] = 0;
}

void * Thread(void * info)
{
	int ipid = ((t_info*)info)->pid;
	while(sleepFlag)
	{	
		lock(ipid);
			(((t_info*)info)->n)++;
			assert (in_cs==0);
	 		in_cs++;
	 		assert (in_cs==1);
	 		in_cs++;
	 		assert (in_cs==2);
	 		in_cs++;
	 		assert (in_cs==3);
	 		in_cs=0;
		unlock(ipid);
	}
	return NULL;
}