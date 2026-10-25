OBJS 	= server_main.o circular_buffer.o linked_list.o
SOURCE	= server_main.c circular_buffer.c linked_list.c
HEADER  = server.h circular_buffer.h linked_list.h
OUT  	= poller
CC		= gcc
FLAGS   = -g -c
BKFLAGS = -lpthread
# -g option enables debugging mode 
# -c flag generates object code for separate files

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(BKFLAGS) 

# create/compile the individual files >>separately<<
linked_list.o: linked_list.c linked_list.h
	$(CC) $(FLAGS) linked_list.c 

circular_buffer.o: circular_buffer.c circular_buffer.h
	$(CC) $(FLAGS) circular_buffer.c $(BKFLAGS)

server_main.o: server_main.c server.h
	$(CC) $(FLAGS) server_main.c $(BKFLAGS)	 

# clean house
clean:
	rm -f $(OBJS) $(OUT)

