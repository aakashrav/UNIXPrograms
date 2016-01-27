SOURCES=main.c pipe_functions.c
OBJECTS=$(SOURCES:.c=.o)
CC=gcc
CFLAGS=-c -Wall -std=gnu99 -I./include -MP -MMD

all: simple_shell

.c.o:
	$(CC) $(CFLAGS) $<

simple_shell: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS)

clean:
	-rm simple_shell
	-rm $(OBJECTS)
	-rm $(SOURCES:.c=.d)
	-rm services
