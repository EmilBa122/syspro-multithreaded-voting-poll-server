#pragma once
#include <stdio.h>
#include <sys/types.h>	        /* sockets */
#include <sys/socket.h>	        /* sockets */
#include <netinet/in.h>	        /* internet sockets */
#include <netdb.h>	            /* gethostbyaddr */		
#include <stdlib.h>	            /* exit */
#include <pthread.h>            /* For threads */
#include <string.h>
#include <signal.h>             /* signal */

#include "circular_buffer.h"

//////////////////////////////////////////////////////////////////////////////////
//mutexes that the workers use when they interact with the linked lists and the file pollLog
//to synchronize them
pthread_mutex_t mtx_list;
pthread_mutex_t mtx_pollLog;
////////////////////////////////////////////////////////////////////////////////

typedef struct struct_worker_thread {
    circular_buffer *circ_buff;
    char *pollLog;
} struct_worker_thread;

//list that has the names of the voters and the party they have voted in this poll
Node *list;
//list that has all the different names of the parties we found in this poll
party_list *party_voters_list;

//flag that we use when the user will give SIGINT to handle this signal 
int flag_termination;