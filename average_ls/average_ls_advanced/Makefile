MAIN_SOURCES=main.c average_functions_advanced.c
MAIN_OBJECTS=$(MAIN_SOURCES:.c=.o)
RECURSIVE_SOURCES=recursive_searcher.c
RECURSIVE_OBJECTS=$(RECURSIVE_SOURCES:.c=.o)
CC=gcc
CFLAGS=-c -Wall -I./include -MP -MMD

all: averageLSAdvanced recursive_searcher

averageLSAdvanced: $(MAIN_OBJECTS)
	$(CC) $(MAIN_OBJECTS) -o $@
	@echo Successfully linked main!

recursive_searcher: $(RECURSIVE_OBJECTS)
	$(CC) $(RECURSIVE_OBJECTS) -o $@
	@echo Successfully linked recurser!

.c.o:
	$(CC) $(CFLAGS) $<
	@echo Successfull compilation!

-include: $(MAIN_SOURCES:.c=.d)

clean:
	@rm $(MAIN_OBJECTS) $(RECURSIVE_OBJECTS) \
	$(MAIN_SOURCES:.c=.d) $(RECURSIVE_SOURCES:.c=.d) \
	averageLSAdvanced recursive_searcher
	@echo Successfull clean

.PHONY: all clean
