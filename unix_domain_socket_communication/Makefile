SOURCES= main.c unix_socket_functions.c
OBJECTS=$(SOURCES:.c=.o)
CC=gcc
CFLAGS=-c -Wall -I./include -MP -MMD

all: unix_domain_socket_communication

.c.o:
	$(CC) $(CFLAGS) $<

unix_domain_socket_communication: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS)

clean:
	-rm $(OBJECTS)
	-rm $(SOURCES:.c=.d)
	-rm unix_domain_socket_communication