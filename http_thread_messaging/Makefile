SOURCES=main.c thread_helper.c
OBJECTS=$(SOURCES:.c=.o)
CC=gcc
CFLAGS=-c -std=gnu99 -MP -MMD -I./include

all: multithread_spammer

.c.o:
	$(CC) $(CFLAGS) -o $@ $<

multithread_spammer: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS)

clean:
	rm multithread_spammer
	rm $(SOURCES:.c=.d)
	rm $(OBJECTS)

.PHONY: all clean
