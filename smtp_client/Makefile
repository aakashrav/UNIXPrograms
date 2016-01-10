SOURCES=walk_directory.c smtp_client.c main.c
OBJECTS=$(SOURCES:.c=.o)
CC=gcc
CFLAGS=-Wall -std=c99 -I./include -MP -MMD

DEBUG=true

all: clean send_mails

.c.o:
ifeq(DEBUG,true):
	$(CC) $(CFLAGS) -DDEBUG -o $<
else:
	$(CC) $(CFLAGS) -o $<

send_mails: $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

clean:
	rm -rf $(OBJECTS)
	rm -rf $(SOURCES:.c=.d)
	rm -rf send_mails

.PHONY: all clean
