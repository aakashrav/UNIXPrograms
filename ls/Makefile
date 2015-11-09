SOURCE=main.c ls.c
OBJECTS=$(SOURCE:.c=.o)
CC=gcc
CFLAGS=-c -I./include -MP -MMD

all: ls

ls: $(OBJECTS)
	@$(CC) $(OBJECTS) -o ls
	@echo Successful linking

.c.o:
	@$(CC) $(CFLAGS) $<
	@echo Successful compilation

-include $(SOURCE:.c=.d)

clean:
	@rm $(OBJECTS) $(SOURCE:.c=.d)
	@rm ./ls
	@echo Successful clean

.PHONY: all clean
