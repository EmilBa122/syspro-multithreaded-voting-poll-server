#include "client_functions.h"

void perror_exit(char *message)
{
    printf("error\n");
    perror(message);
    pthread_exit(NULL);
}

//Function that counts how many lines (i.e. voters) we have in the file inputFile.txt
int count_lines(char *name_file)
{
    //Open the file
    FILE *fp = fopen(name_file, "r");
 
    //Check if file exists
    if (fp == NULL)
    {
        printf("Could not open file %s", name_file);
        return 0;
    }
    int c;
    int count = 0, count_char_line = 0;
    //Extract characters from file and store in character c
    for (c = getc(fp); c != EOF; c = getc(fp))
    {
        count_char_line++;
        if (c == '\n') //Increment count if this character is newline
        {
            count = count + 1;
            count_char_line = 0;
        }
    
    }
    if (count_char_line > 1 )//For the last line that doesnt has '\n'
    {
        count++;
    }
    //Close the file
    fclose(fp);
    return count;
}

void *connection_thread(void *args)
{
    THREAD_INFO *thread_info = (THREAD_INFO *)args;

    int port, sock, i;
    struct sockaddr_in server;
    struct sockaddr *serverptr = (struct sockaddr*)&server;
    struct hostent *rem;
	/* Create socket */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        free(thread_info);
    	perror_exit("socket");
    }
    /* Find server address */
    if ((rem = gethostbyname(thread_info->server_name)) == NULL) 
    {
        free(thread_info);
	    herror("gethostbyname"); exit(1);
    }
    port = thread_info->port; /* port number */
    server.sin_family = AF_INET;       /* Internet domain */
    memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
    server.sin_port = htons(port);         /* Server port */
    /* Initiate connection */
    if (connect(sock, serverptr, sizeof(server)) < 0)
    {
        free(thread_info);
	    perror_exit("connect");
    }   
    
    char voter_name[BUFFSIZE];
    char party[BUFFSIZE];
    char buf[BUFFSIZE];

    char *context = NULL;
 

    char * token = strtok_r(thread_info->voter, " ", &context);
    
    sprintf(voter_name, "%s ", token);
    token = strtok_r(NULL, " ", &context);
    strcat(voter_name, token);
    token = strtok_r(NULL, " \n\0", &context);
    strcpy(party, token);
    if (read(sock, buf, BUFFSIZE) < 0)
    {
        printf("cant read from server\n");
        free(thread_info);
        close(sock); 
        pthread_exit(NULL);
    }
    if (strcmp(buf, "SENT NAME PLEASE"))
    {
        printf("server sent wrong message\n");
        free(thread_info);
        close(sock); 
        pthread_exit(NULL);
    }
    if (write(sock, voter_name, strlen(voter_name) + 1) < 0)
    {
        fprintf(stderr, "cant write to server\n");
        free(thread_info);
        close(sock); 
        pthread_exit(NULL);
    }
    if (read(sock, buf, BUFFSIZE) < 0)
    {
        fprintf(stderr, "cant read from server\n");
        free(thread_info);
        close(sock); 
        pthread_exit(NULL);
    }
    if (!strcmp(buf, "ALREADY VOTED"))
    {
        free(thread_info);
        printf("eimai mesa\n");
        printf("%s\n", voter_name);
        close(sock); 
        pthread_exit(NULL);
    }
    else
    {
        if (write(sock, party, strlen(party) + 1) < 0)
        {
            fprintf(stderr, "cant write to server\n");
            free(thread_info);
            close(sock); 
            pthread_exit(NULL);
        }  
    }
    if (read(sock, buf, 10000) < 0)
    {
        fprintf(stderr, "cant read from server\n");
    }  
    printf("%s\n", voter_name);  
    free(thread_info);
    close(sock);                 /* Close socket and exit */
    printf("ending thread\n");
    pthread_exit(NULL);
}
