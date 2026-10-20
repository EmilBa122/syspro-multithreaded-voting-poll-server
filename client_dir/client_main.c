#include "client_functions.h"

int main(int argc, char *argv[]) 
{
    int i = 0;
    char buf[BUFFSIZE];
    int nread;
    int fd;
    int count = 0;
    int err;
    char *temp;
    //function that counts how many lines (i.e. voters) we have in the file argv[3]
    int thread_num = count_lines(argv[3]);
    printf("%d\n", thread_num);

    if (thread_num == -1)
    {
        printf("file %s does not exist\n", argv[3]);
        return -1;
    }
    FILE *fp = fopen(argv[3], "r");
    //we will have thread_num threads 
    pthread_t *threads = malloc(sizeof(pthread_t) * thread_num);
    
    for (int i = 0; i < thread_num; i++)
    {
        memset(buf, 0, BUFFSIZE);
        fgets(buf, BUFFSIZE, fp);
        if (buf[strlen(buf) -1] == '\n')
        {
            buf[strlen(buf) -1] = 0;
        }
        THREAD_INFO *temp = malloc(sizeof(THREAD_INFO));
        strcpy(temp->voter, buf);
        strcpy(temp->server_name, argv[1]);
        temp->port = atoi(argv[2]); 
        if ((err = pthread_create(&threads[i], NULL, connection_thread, temp))) 
        { 
                printf("err = %d\n", err);
                printf("error in pthread_create for consumers\n"); return -1; 
        }

    }
   
    fclose(fp);

    /* Wait for threads termination */
    for (i = 0; i < thread_num; i++)
    {
        pthread_join(threads[i], NULL);
    }

    free(threads);
   
    return 0;
}			     

