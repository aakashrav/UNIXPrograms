SOURCE=file_communication.c
OBJECTS=$(SOURCE:.c=.o)
CC= gcc
CFLAGS= -c -Wall -std=c99 

all: file_communication

file_communication: file_communication.c
	$(CC) $(CFLAGS) $<
	$(CC) $(OBJECTS) -o $@
	@echo Succesful creation of program!


clean:
	rm -f file_communication
	rm -f $(OBJECTS)

.PHONY: all clean