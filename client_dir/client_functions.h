#include <stdio.h>
#include <sys/types.h>	     /* sockets */
#include <sys/socket.h>	     /* sockets */
#include <netinet/in.h>	     /* internet sockets */
#include <unistd.h>          /* read, write, close */
#include <netdb.h>	         /* gethostbyaddr */
#include <stdlib.h>	         /* exit */
#include <string.h>	         /* strlen */
#include <pthread.h>
#include <fcntl.h>           /* open */

#define BUFFSIZE 2048

typedef struct {
    char voter[BUFFSIZE];
    char server_name[BUFFSIZE];//argv[1]
    int port;//argv[2]
} THREAD_INFO;

void perror_exit(char *message);
int count_lines(char *name_file);
void *connection_thread(void *args);