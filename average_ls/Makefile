SOURCES=main.c average_functions.c
OBJECTS=$(SOURCES:.c=.o)
CFLAGS=-c -Wall -I./include -MP -MMD
CC=gcc

all: average_ls

average_ls: $(OBJECTS)
	$(CC) $(OBJECTS) -o averageLS
	@echo Successful Linking

.c.o:
	$(CC) $(CFLAGS) $<
	@echo Successful Compilation

-include $(SOURCES:.c=.d)

clean:
	@rm $(OBJECTS) $(SOURCES:.c=.d) ./averageLS
	@echo Successful clean

.PHONY: all clean