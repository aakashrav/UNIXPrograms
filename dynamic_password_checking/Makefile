MAKE=make
TESTER_SOURCES=tester.c
TESTER_OBJECTS=$(TESTER_SOURCES:.c=.o)
CC=gcc
CFLAGS=-Wall -std=c99 -I./include/ -MP -MMD
LDLFLAGS=-ldl -lpassword_checker

all: clean libraries tester

tester: $(TESTER_SOURCES)
	$(CC) $(CFLAGS) $< -L./lib/ $(LDLFLAGS) -o $@
	@echo finished building tester!

libraries:
	mkdir ./lib
	mkdir ./lib/plugins
	$(MAKE) -C ./src/
	@echo finished building libraries!

clean:
	rm -f tester
	rm -f tester.d
	rm -rf ./lib

.PHONY: all clean
