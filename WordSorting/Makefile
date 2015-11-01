SOURCE=main.c wordsort.c
OBJECTS=$(SOURCE:.c=.o)
CC=gcc
CFLAGS=-c -I./include -MP -MMD

all: wordsort install

wordsort: $(OBJECTS)
	@$(CC) $(OBJECTS) -o wordsort
	@echo Successful linking

.c.o:
	@$(CC) $(CFLAGS) $<
	@echo Successful compilation

-include $(SOURCE:.c=.d)

#install:
#	@mkdir wordsortbin
#	@install -m 0755 wordsort ./wordsortbin
#	@rm ./wordsort
#	@echo Successfully installed program

clean:
	@rm $(OBJECTS) $(SOURCE:.c=.d)
#	@rm -rf ./wordsortbin
	@rm ./wordsort
	@echo Successful clean

.PHONY: all clean install
