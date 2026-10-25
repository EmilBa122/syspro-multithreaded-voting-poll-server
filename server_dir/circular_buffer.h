#pragma once
#include <stdio.h>
#include <stdlib.h>	 
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>

#include "linked_list.h"

#define PERMS 0644 //set access permissions

////////////////////////////////////////////////////////////////////////////////////
//mutex and conditions variables that we use when the master thread place socket descriptors
//on the circular buffer or when the workers take from this structure to synchronize them
pthread_mutex_t mtx;
pthread_cond_t cond_nonfull;
pthread_cond_t cond_nonempty;
////////////////////////////////////////////////////////////////////////////////

typedef struct circular_buffer {
    int *data;
    int start;
    int end;
    int count;
    int bufferSize;
} circular_buffer;

int initialize_circular_buffer(circular_buffer *circ_buff, int bufferSize);
int destroy_circular_buffer(circular_buffer *circ_buff);

//Only the master thread use this function to place socket descriptors on the 
//circular buffer
void place(circular_buffer *cb, int sock);  
//Only workers use this function to obtain a socket descriptor from the circular buffer
int obtain(circular_buffer *cb);