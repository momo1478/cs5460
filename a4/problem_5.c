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

struct spin_lock_t
{
	volatile int counter;
	volatile int turn;
} spin_lock_t;

struct spin_lock_t* lock;

int num_threads;
int num_seconds;

t_info* infos;
pthread_t* threads;

volatile int in_cs = 0;
volatile int sleepFlag;

void spin_lock (struct spin_lock_t *s);
void spin_unlock (struct spin_lock_t *s);

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

/*
 * atomic_xadd
 *
 * equivalent to atomic execution of this code:
 *
 * return (*ptr)++;
 * 
 */
static inline int atomic_xadd (volatile int *ptr)
{
  register int val __asm__("eax") = 1;
  asm volatile ("lock xaddl %0,%1"
  : "+r" (val)
  : "m" (*ptr)
  : "memory"
  );  
  return val;
}

void spin_lock (struct spin_lock_t *s)
{
	int me;  
	me = atomic_xadd(&(s->counter));
	while (me != s->turn)    
		sched_yield();
}

void spin_unlock (struct spin_lock_t *s)
{
	atomic_xadd(&(s->turn));
}

void * Thread(void * info)
{
	while(sleepFlag)
	{			
		spin_lock(lock);
		(((t_info*)info)->n)++;
			assert (in_cs==0);
	 		in_cs++;
	 		assert (in_cs==1);
	 		in_cs++;
	 		assert (in_cs==2);
	 		in_cs++;
	 		assert (in_cs==3);
	 		in_cs=0;
		spin_unlock(lock);
	}
	return NULL;
}

int main(int argc, char **argv)
{

	if(argc != 3)
	{
		fprintf(stderr, "Incorrect number of args \n.");
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
	lock   = malloc(sizeof(spin_lock_t));
	

	if(threads == NULL || infos == NULL || lock == NULL)
	{
		fprintf(stderr, "malloc failed \n");
		return 1;
	}
 	lock->turn = 0;
	lock->counter = 0;

 	int i;
 	for(i = 0; i < num_threads; i++)
 	{
 		infos[i].n = 0;
 		infos[i].tnum = i;
 	}

 	sleepFlag = 1;

 	for(i = 0; i < num_threads; i++)
 	{
 		if(pthread_create(&threads[i], NULL, Thread, &infos[i]))
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
 		printf("Thread %d entered the critical section %d times!\n", i, (int)(infos[i].n) );
 	}
 	return 0;
}