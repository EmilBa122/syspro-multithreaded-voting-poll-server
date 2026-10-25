#include "circular_buffer.h"

int initialize_circular_buffer(circular_buffer *circ_buff, int bufferSize)
{
    circ_buff -> start = 0;
    circ_buff -> end = -1;
    circ_buff -> count = 0;
	circ_buff -> bufferSize = bufferSize;
    circ_buff -> data = malloc(sizeof(int) * bufferSize);
	//And I initialize here the mutex and conditions variables
	pthread_mutex_init(&mtx, 0);
	pthread_cond_init(&cond_nonempty, 0);
	pthread_cond_init(&cond_nonfull, 0);
    return 0;
}

int destroy_circular_buffer(circular_buffer *circ_buff)
{
    free (circ_buff -> data);
    circ_buff -> data = NULL;
	//Also I destroy here the mutex and conditions variables
	pthread_cond_destroy(&cond_nonempty);
	pthread_cond_destroy(&cond_nonfull);
	pthread_mutex_destroy(&mtx);
    return 0;
}

void place(circular_buffer *cb, int sock) 
{
	pthread_mutex_lock(&mtx);
	//In case the buffer is full we use the condition variable and wait
	while (cb->count >= cb->bufferSize)
    {
		printf(">> Found Buffer Full \n");
		pthread_cond_wait(&cond_nonfull, &mtx);
	}
	cb->end = (cb->end + 1) % cb->bufferSize;
	cb->data[cb->end] = sock;
	cb->count++;
	pthread_mutex_unlock(&mtx);
	//signal that the buffer is not empty in case if a worker is waiting
	pthread_cond_signal(&cond_nonempty);
}

int obtain(circular_buffer *cb) 
{
	int client_socket = 0;
	pthread_mutex_lock(&mtx);
	//If we have an empty buffer we use the condition variable and wait 
	while (cb->count <= 0) 
	{
		printf(">> Found Buffer Empty \n");
		pthread_cond_wait(&cond_nonempty, &mtx);
	}
	client_socket = cb->data[cb->start];
	cb->start = (cb->start + 1) % cb->bufferSize;
	cb->count--;
	//we signal that the buffer has an empty space to place in case the master thread 
	//was blocked because of that
	pthread_cond_signal(&cond_nonfull);
	pthread_mutex_unlock(&mtx);
	return client_socket;
}


