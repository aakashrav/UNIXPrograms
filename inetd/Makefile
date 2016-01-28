SOURCES=inetd_functions.c main.c
OBJECTS=$(SOURCES:.c=.o)
CC=gcc
CFLAGS=-c -g -Wall -std=gnu99 -I./include -MP -MMD

all: inetd_server

.c.o:
	$(CC) $(CFLAGS) $<

inetd_server: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS)

clean:
	-rm inetd_server
	-rm $(SOURCES:.c=.d)
	-rm $(OBJECTS)
