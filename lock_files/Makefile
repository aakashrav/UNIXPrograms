SOURCES=main.c
OBJECTS=$(SOURCES:.c=.o)
CC=gcc
CFLAGS=-c -Wall

all: lock_file_tester

.c.o:
	$(CC) $(CFLAGS) $<

lock_file_tester: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS)

clean:
	-rm lock_file_tester
	-rm $(OBJECTS)