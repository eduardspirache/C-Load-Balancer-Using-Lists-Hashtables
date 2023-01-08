CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -g
LOAD=load_balancer
SERVER=server
HASH=Hashtable
LIST=LinkedList

.PHONY: build clean

build: load-balancer

load-balancer: main.o $(LOAD).o $(SERVER).o $(HASH).o $(LIST).o
	$(CC) $^ -o $@

main.o: main.c
	$(CC) $(CFLAGS) $^ -c

$(SERVER).o: $(SERVER).c $(SERVER).h
	$(CC) $(CFLAGS) $^ -c

$(LOAD).o: $(LOAD).c $(LOAD).h
	$(CC) $(CFLAGS) $^ -c

$(HASH).o: $(HASH).c $(HASH).h
	$(CC) $(CFLAGS) $^ -c

$(LIST).o: $(LIST).c $(LIST).h
	$(CC) $(CFLAGS) $^ -c

pack:
	zip -FSr 312CA_Spirache_Eduard-Mihai.zip README Makefile *.c *.h


clean:
	rm -f *.o load-balancer *.h.gch
