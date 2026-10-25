# syspro multithreaded voting poll server

## Assignment Description
Implementation of a multithreaded voting poll server and client in C using sockets, a circular buffer and thread synchronization with mutexes and condition variables<br>

**<u>Compilation and Execution of the program</u> :**

1\) Initially the assignment is divided into two folders the folder server_dir where there the server of the assignment is implemented and the folder client_dir where there the client of the assignment is implemented

2\) Go into the folder server_dir and type make

3\) Then e.g. you can type :

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;./poller 9004 4 8 pollLog.txt pollStats.txt

&nbsp;&nbsp;&nbsp;for the execution of the server

4\) Then go into the folder client_dir and type make

5\) Then e.g. you can type :

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;./pollSwayer serverName 9004 inputFile.txt

&nbsp;&nbsp;&nbsp;for the execution of the client

**<u>Design choices and Explanation of the Implementation of the assignment:</u>**

**In the folder server_dir (in the folder of the server i.e.):**

**1)** In the files linked_list.h/.c the structures of two linked lists are implemented. In the first list each node of it consists of the full name of a voter and the party that he has voted for while in the second list each node of it has the name of the party that we encountered in the poll i.e. this list contains all the different parties that we encountered in the poll, of course with the corresponding functions with which we can interact with these two lists.

**2)** In the files circular_buffer.h/.c we have the structure of a circular buffer of size bufferSize which keeps the socket descriptors from the clients that are waiting to be served. **In circular_buffer.h I have a mutex the mtx and two conditions variables the cond_nonfull, cond_nonempty so that there is proper synchronization between master-workers threads when they interact with this structure. Specifically the master thread can only add socket descriptors to this structure** with the function void place(circular_buffer \*cb, int sock) **by initially doing lock the mtx it checks if the buffer is full if yes it does wait with the use of the condition variable cond_nonfull, otherwise it writes the socket descriptor in the structure, it does unlock the mtx and it does a signal with the condition variable cond_nonempty that the list is not empty so that if a worker was waiting to start serving a client.** Correspondingly the workers use only the function int obtain(circular_buffer \*cb) which returns to them a socket descriptor in order to start serving a client. Specifically **a worker that will call this function does lock the mtx in order for there to be synchronization with the master and the other workers. If the buffer is empty it does wait with the use of the condition variable**

**cond_nonempty otherwise it takes a socket descriptor does signal that there is an empty position in the buffer with the condition variable cond_nonfull and does unlock the mtx.**

**3)** In the files server_main.c/server.h the operation of the server is implemented. Specifically **in the file server.h I have defined two global lists, the list which I will use to store the voters and the party that they have voted for while the party_voters_list I use it to store the different parties that I have found in the poll I also have two mutexes the mtx_list and mtx_pollLog which are used when the workers will interact with the lists and the file pollLog respectively in order for there to be proper synchronization between them, also I use a flag the int flag_termination which I will use when the user gives a termination signal so that the master does not accept connections anymore and the workers write the full names and the votes of the voters up to that moment in the pollLog file. So in the file server_main.c in the function main initially I define a signal handler the function void sigint_catcher(int signo) which the only thing it does is to make 1 the flag_termination which will mean that the user gave a termination signal. Continuing in the function main** after we create and initialize properly the fields of the server, I create the circular buffer circ_buff and I initialize the mutexes mtx_list and mtx_pollLog, I initialize also the two lists list and party_voters_list respectively and **I make an array pthread_t threads of size numWorkerthreads where each element of the array represents a worker thread , then I call the pthread_create for each one worker together with the function worker_thread**. As regards the master thread it waits and accepts connections with the corresponding socket descriptors to store them in the circ_buff with the function void place(circular_buffer \*cb, int sock), **also in all this procedure the program is able to deal with the possibility that the user gave a termination signal**. The function void \*worker_thread(void \*data) is called only by the workers and through it essentially they interact with the clients sending and reading various messages, and where inside this function they call various functions for the structures of the lists. When they will interact with the lists or when they will write to the file pollLog they will do lock, unlock the mutexes mtx_list and mtx_pollLog respectively in order for there to be proper synchronization between the workers. **If the user gives a termination signal the flag_termination becomes 1 and the master thread stops accepting connections and puts in the circular buffer the value -1 numWorkerthreads times so that each worker when it takes this value from the buffer terminates and then after all the workers terminate** we make an array parties_votes of size number_parties where in each position of it you will have the number of votes that this party gathered according to the order in which the parties appear in the list party_voters_list i.e. in the first position of the array we have the votes of the first party of the list in the second position the votes of the second party of the list etc, and then after we write all these in the file pollStas **the master will also terminate.**

**In the folder client_dir (in the folder of the client i.e.):**

In the files client_functions.h/.c and client_main.c we implement the structure of the client of the assignment. Specifically in the file client_main.c initially we define the variable thread_num with the number of lines that exist in the inputFile.txt where each line will represent the full name of a voter and the party that he voted for. Then knowing the number of voters we define the pthread_t \*threads a pointer that points to an

array of size thread_num where each element of it is a thread. Therefore for each one line of the array the master thread will do pthread_create the thread threads[i] and together with the function void \* connection_thread(void \*args) this thread will do establish connection with a server and after a series of read and write it will communicate with the server according to the protocol of the assignment.

**<u>WHAT HAS BEEN IMPLEMENTED:</u>**

**master**
- [x] Writing the connections into the buffer
- [x] Creation of worker threads (multiple)
- [x] Updating the `poll-stats` file with the statistics and if ^C (SIGINT) updating the `poll-stats` before the exit

**worker thread**
- [x] Worker reading a request from the buffer
- [x] Communication worker-client
- [x] Support of `ALREADY VOTED`
- [x] Writing the vote in the `poll-log` file
- [x] Updating the structure of the server for the vote so that it can produce the statistics later
- [x] The update of the server for the vote is done with synchronization of the workers

**sync**
- [x] master block if buffer full
- [x] worker wait if buffer empty
- [x] The update of the server for the vote is done with synchronization of the workers
- [x] Support of condition variables for synchronization

**client**
- [x] Basic implementation of a multithreaded client
- [x] Communication client - server
- [x] Communication client - worker thread

**<u>WHAT HAS NOT BEEN IMPLEMENTED:</u>**

**EVERYTHING HAS BEEN IMPLEMENTED EXCEPT THE BASH SCRIPTS.**

**THANK YOU!!!!!!!**
