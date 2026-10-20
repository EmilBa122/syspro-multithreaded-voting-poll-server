#include "server.h"

void perror_exit(char *message) 
{
    perror(message);
    exit(EXIT_FAILURE);
}

void set_struct_worker_thread(struct_worker_thread *ptr_work_thr, circular_buffer *circ_buff, char *pollLog)
{
	ptr_work_thr->circ_buff = circ_buff;
	ptr_work_thr->pollLog = pollLog;
}

void *worker_thread(void *data)
{
    struct_worker_thread *ptr = (struct_worker_thread *)data; 
	int client_socket = 0;
    char buf[BUFFSIZE];
	char voter_name[BUFFSIZE];
	char party[BUFFSIZE];
    while (1)
	{
        client_socket = obtain(ptr->circ_buff);
        if (client_socket == -1)
        {
            pthread_exit(NULL);
        }
		strcpy(buf, "SENT NAME PLEASE");
		if (write(client_socket, buf, strlen(buf) + 1) < 0 )
		{
			fprintf(stderr, "cant write to client\n");
			close(client_socket); 
			pthread_exit(NULL);
		}
		if (read(client_socket, voter_name, BUFFSIZE) < 0)
		{
			fprintf(stderr, "cant read from server\n");
			close(client_socket); 
			pthread_exit(NULL);
		}
        pthread_mutex_lock(&mtx_list);
		if ( (find_voter(list, voter_name)) != -1)
		{
            pthread_mutex_unlock(&mtx_list);
			strcpy(buf, "ALREADY VOTED");
			if (write(client_socket, buf, strlen(buf) + 1) < 0 )
			{
				fprintf(stderr, "cant write to client\n");
			}
			close(client_socket);
            continue; 
		}
		strcpy(buf, "SEND VOTE PLEASE");
		if (write(client_socket, buf, strlen(buf) + 1) < 0 )
		{
			fprintf(stderr, "cant write to client\n");
            pthread_mutex_unlock(&mtx_list);
			close(client_socket); 
			pthread_exit(NULL);
		}
		if (read(client_socket, party, BUFFSIZE) < 0)
		{
			fprintf(stderr, "cant read from server\n");
            pthread_mutex_unlock(&mtx_list);
			close(client_socket); 
			pthread_exit(NULL);
		}
		push( &(list), voter_name, party);
        //we check if we see this party for the first time
        if (!exist_party(party_voters_list, party))
        {
            //if yes we add this party to list party_voters_list
            //that has all the different parties we find in this poll
            new_party_add(&party_voters_list, party);
        }
        pthread_mutex_unlock(&mtx_list);
		char temp[100000];
		sprintf(temp , "VOTE for Party %s RECORDED", party);
		if (write(client_socket, temp, strlen(temp) + 1) < 0 )
		{
			fprintf(stderr, "cant write to client\n");
			close(client_socket); 
			pthread_exit(NULL);
		}
		close(client_socket);

		/////////////////////////////////////////////////////////////////
        //For the file pollLog
        pthread_mutex_lock(&mtx_pollLog);
		int fd1;
		if ((fd1 = open(ptr->pollLog, O_APPEND | O_CREAT | O_RDWR, PERMS)) == -1)
    	{
            printf("error opening file\n");
			pthread_mutex_unlock(&mtx_pollLog);
            pthread_exit(NULL);
    	}
		sprintf(temp, "%s %s\n", voter_name, party);
		write(fd1, temp, strlen(temp));
		close(fd1);
        pthread_mutex_unlock(&mtx_pollLog);
        /////////////////////////////////////////////////////////////
		//  printf("------------------------------------");
		//  print_list(ptr->list);
		//  printf("---------------------------------------");
	}	
    pthread_exit(NULL);
}

//For the file pollStats
//This function writes the number of votes every party had and the totall votes we had
int results_to_pollStats(char *pollStats,int *parties_votes)
{
    int fd1;
    if ((fd1 = open(pollStats, O_CREAT | O_RDWR, PERMS)) == -1)
    {
        printf("error opening file\n");
        return -1;
    }
    party_list *temp_party_voters_list = party_voters_list;
    int i = 0;
    char temp[100000];
    int counter = 0;
    while (temp_party_voters_list)
    {
        counter = counter + parties_votes[i];
        sprintf(temp, "%s %d\n", temp_party_voters_list->party, parties_votes[i]);
        write(fd1, temp, strlen(temp));
        i++;
        temp_party_voters_list = temp_party_voters_list->next;
    }
    sprintf(temp, "TOTAL %d\n", counter);
    write(fd1, temp, strlen(temp));
    close(fd1);
}

//Only we make the flag_termination 1 to signal that the user gave SIGINT
void sigint_catcher(int signo) 
{
    flag_termination = 1;
}

int main(int argc, char **argv)
{
    ////////////////////////////////////////////////////////////////////////
    //To be able to handle the signal SIGINT if the user will give
    static struct sigaction act;
    
    act.sa_handler = sigint_catcher;
    sigfillset(&(act.sa_mask));

    sigaction(SIGINT, &act, NULL);

    flag_termination = 0;
    //////////////////////////////////////////////////////////////

   
    int  sock, newsock;
    struct sockaddr_in server, client;
    socklen_t clientlen = sizeof(client);
    struct sockaddr *serverptr=(struct sockaddr *)&server;
    struct sockaddr *clientptr=(struct sockaddr *)&client;
    struct hostent *rem;
    if (argc != 6)
    {
        printf("wrong arguments try again\n");
        return -1;
    }

    int portnum = atoi(argv[1]);
    int numWorkerthreads = atoi(argv[2]);
    int bufferSize = atoi(argv[3]);
    char   pollLog[BUFFSIZE];
    char   pollStats[BUFFSIZE];

    strcpy(pollLog, argv[4]);
    strcpy(pollStats, argv[5]);

    /* Create socket */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        perror_exit("socket");
    server.sin_family = AF_INET;       /* Internet domain */
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(portnum);      /* The given port */
    /* Bind socket to address */
    if (bind(sock, serverptr, sizeof(server)) < 0)
        perror_exit("bind");
    /* Listen for connections */
    if (listen(sock, 200) < 0) perror_exit("listen");

    ///////////////////////////////////////////////////////////////////
    circular_buffer circ_buff;
    //In the below function expect from initialize the circular buffer we initialize
    //the mutex and conditions variables that we will use to synchronize when the master
    //thread will place a socket descriptors to the circular buffer or when a worker 
    //will take a socket descriptors from the structure
    initialize_circular_buffer(&circ_buff, bufferSize);

    //We initialize the mutexes that we will use to synchronize the workers for the 
    //linked lists and the file pollLog
    pthread_mutex_init(&mtx_list, 0);
    pthread_mutex_init(&mtx_pollLog, 0);
    ///////////////////////////////////////////////////////////////////
    //Linked list that every node has the name of the voter and the party who voted
    initialize_linked_list(&list);
    //Linked list that has all the names of the parties we will find in this poll
    initialize_party_list(&party_voters_list);
    //A table that has threads (i.e. the workers threads we will have), 
    //the size of the table is numWorkerthreads
    pthread_t threads[numWorkerthreads];
    struct_worker_thread work_thr;

    
    set_struct_worker_thread(&work_thr ,&circ_buff, argv[4]);

    int err;
    for (int i = 0; i < numWorkerthreads; i++)
    {
        if ( err = pthread_create(&threads[i], NULL, worker_thread, (void *) &work_thr))/* Create threads */
        { 
            printf("error in pthread_create for consumers\n"); exit(1); 
        }

    }

    while (1) 
    {
        if ( flag_termination == 1) break;//we check if the user gave SIGINT
    	/* accept connection */
        if ((newsock = accept(sock, clientptr, &clientlen)) < 0) {}
        if ( flag_termination == 1) break;
        place(&circ_buff, newsock); 
        if ( flag_termination == 1) break;
            
    }
    //Here the master thread "notifies" the workers to finish
    for (int i = 0; i < numWorkerthreads; i++)
    {
        place(&circ_buff, -1); 
    }
    /* Wait for threads termination */
    for (int i = 0; i < numWorkerthreads; i++)
    {
        pthread_join(threads[i], NULL);
    }
    //print_list(list);


    int number_parties = number_of_parties(party_voters_list);
    //printf("%d\n", number_parties);
    //We create a table size number_parties that every position has the number of votes 
    //the party has according with the position the parties appear in the list party_voters_list
    //i.e. position 0 in the table has the number of votes of the first party in the list
    //party_voters_list, position 1 has the number of votes of the second party in the list
    //party_voters_list and so on 
    int *parties_votes = malloc(number_parties * sizeof(int)); 
    for (int i = 0; i < number_parties; i++)
    {
        parties_votes[i] = 0;
        //printf("%d\n", parties_votes[i]);
    }
    Node *temp_list = list;
    party_list *temp_party_voters_list = party_voters_list;
    for (int i = 0; i < number_parties; i++)
    {
        while(temp_list)
        {
            if (!strcmp(temp_party_voters_list->party, temp_list->party))
            {
                (parties_votes[i])++;
            }
            temp_list = temp_list->next;
        }
        temp_list = list;
        temp_party_voters_list = temp_party_voters_list->next;
    }

   
    // for (int i = 0; i < number_parties; i++)
    // {
    //     printf("%d\n", parties_votes[i]);
    // }

    // print_party_list(party_voters_list);
    close(sock);


    //For the file pollStats
    results_to_pollStats(argv[5], parties_votes);

    destroy_circular_buffer(&circ_buff);
    destroy_linked_list(&list);
    destroy_party_list(&party_voters_list);
    pthread_mutex_destroy(&mtx_list);
    pthread_mutex_destroy(&mtx_pollLog);
    free(parties_votes);
    //printf("eftasa");
    return 0;
}