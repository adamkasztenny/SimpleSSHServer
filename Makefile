CC=gcc
CFLAGS=-Wall -g

all: simplesshserver

simplesshserver: 
	$(CC) $(CFLAGS) simplesshserver.c -o $@

clean:
	rm -f simplesshserver

