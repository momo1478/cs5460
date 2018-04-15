#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <assert.h>

typedef struct t_info 
{
	int type;
} t_info;

pthread_mutex_t mu;
pthread_cond_t cv;

int num_birds;
int num_cats;
int num_dogs;
int total_animals;

volatile int c_box;
volatile int d_box;
volatile int b_box;

volatile int cat_play;
volatile int dog_play;
volatile int bird_play;

t_info* infos;
pthread_t* threads;

void* cat_enter(void*);
void* dog_enter(void*);
void* bird_enter(void*);

void cat_exit();
void dog_exit();
void bird_exit();

void play(void);

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

	if(num_cats == 0 || num_dogs == 0 || num_birds == 0 ||
	   num_cats < 0  || num_dogs < 0  || num_birds < 0   )	
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

	if(pthread_mutex_init(&mu) || pthread_cond_init(&cv))
	{
		fprintf(stderr,"mutex or condition variable cannot be initialized.\n");
		return 1;
	}

 	for(i = 0; i < num_cats; i++)
 	{
 		if(pthread_create(&threads[i], NULL, cat_enter, NULL))
 		{
 			fprintf(stderr, "thread could not be created.\n");
 			return 1;
 		}
 	}

 	for(i = num_cats; i < num_cats + num_dogs; i++)
 	{
 		if(pthread_create(&threads[i], NULL, dog_enter, NULL))
 		{
 			fprintf(stderr, "thread could not be created.\n");
 			return 1;
 		}
 	}

 	for(i = num_cats + num_dogs; i < total_animals; i++)
 	{
 		if(pthread_create(&threads[i], NULL, bird_enter, NULL))
 		{
 			fprintf(stderr, "thread could not be created.\n");
 			return 1;
 		}
 	}

 	sleep(10);
 	
 	for(i = 0; i < total_animals; i++)
 	{
 		if(pthread_join(threads[i], NULL))
 		{
 			fprintf(stderr, "Could not join thread.\n");
 			return 1;
 		}	
 	}
 	printf("cat play = %d, dog play = %d, bird play = %d\n", cat_play, dog_play, bird_play);
 	return 0;
}

void* cat_enter(void* info)
{
	pthread_mutex_lock(&mu);
 	while (d_box != 0 && b_box != 0)
 	{
 		pthread_cond_wait(&cv, &mu);
 	} 
 	c_box++;
 	play();
 	cat_play++;
 	cat_exit();
 	pthread_mutex_unlock(&mu);
	return NULL;
}

void* dog_enter(void* info)
{
	pthread_mutex_lock(&mu);
 	while (c_box != 0)
 	{
 		pthread_cond_wait(&cv, &mu);
 	} 
 	d_box++;
 	play();
 	dog_play++;
 	dog_exit();
 	pthread_mutex_unlock(&mu);
	return NULL;
}

void* bird_enter(void* info)
{
	pthread_mutex_lock(&mu);
 	while (c_box != 0)
 	{
 		pthread_cond_wait(&cv, &mu);
 	} 
 	b_box++;
 	play();
 	bird_play++;
 	bird_exit();
 	pthread_mutex_unlock(&mu);
	return NULL;
}

void cat_exit()
{
	c_box--;
	pthread_cond_signal(&cv);
}

void dog_exit()
{
	d_box--;
	pthread_cond_signal(&cv);
}

void bird_exit()
{
	b_box--;
	pthread_cond_signal(&cv);
}

void play(void)
{
  for (int i=0; i<10; i++) {
    assert(c_box >= 0 && c_box <= num_cats);
    assert(d_box >= 0 && d_box <= num_dogs);
    assert(b_box >= 0 && b_box <= num_birds);
    assert(c_box == 0 || d_box == 0);
    assert(c_box == 0 || b_box == 0);
   }
}