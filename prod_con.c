#include "buffer.h"
#include "prod_con.h"
#include <pthread.h>
#include <stdio.h>

pthread_mutex_t mymutex;
pthread_cond_t condEmpty;
pthread_cond_t condFull;


void init_prod_con()
{
	// initialise lock, condition variables etc.
	pthread_mutex_init(&mymutex,  NULL);
	pthread_cond_init(&condEmpty,  NULL);
    pthread_cond_init(&condFull,NULL);
}


// Modify this function. Use mutex and condition variable
void produce()
{
    pthread_mutex_lock(&mymutex);
    while(buffer_full()) // While bcz the signal can be spurious
		pthread_cond_wait(&condEmpty,&mymutex);
	produce_to_buffer(); // <- Do not remove. Add code before and after this line
	pthread_mutex_unlock(&mymutex); // Signal waits till it unlocks the lock
	pthread_cond_signal(&condFull); // Not much change even if we interchange both the lines
}

// Modify this function. Use mutex and condition variable
void consume()
{
    pthread_mutex_lock(&mymutex);
    while(buffer_empty()) // While bcz the signal can be spurious
		pthread_cond_wait(&condFull,&mymutex);
	consume_from_buffer(); // <- Do not remove. Add code before and after this line
    pthread_mutex_unlock(&mymutex);
	pthread_cond_signal(&condEmpty);
}

void destroy_prod_con()
{
	// destroy mutex, condition variable etc.
	pthread_mutex_destroy(&mymutex);
	pthread_cond_destroy(&condFull);
	pthread_cond_destroy(&condEmpty);
}
