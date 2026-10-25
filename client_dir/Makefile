OBJS 	= client_functions.o client_main.o
SOURCE	= client_functions.c client_main.c
HEADER  = client_functions.h
OUT  	= pollSwayer
CC		= gcc
FLAGS   = -g -c
BKFLAGS = -lpthread
# -g option enables debugging mode 
# -c flag generates object code for separate files

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(BKFLAGS) 

# create/compile the individual files >>separately<< 
client_functions.o: client_functions.c client_functions.h
	$(CC) $(FLAGS) client_functions.c

client_main.o: client_main.c
	$(CC) $(FLAGS) client_main.c

# clean house
clean:
	rm -f $(OBJS) $(OUT)
