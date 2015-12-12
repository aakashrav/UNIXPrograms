MAKE=make
TESTER_SOURCES=tester.c
TESTER_OBJECTS=$(TESTER_SOURCES:.c=.o)
CC=gcc
CFLAGS=-I./include/ -MP -MMD

all: libraries tester

tester: $(TESTER_SOURCES) $(PLUGIN_LIBRARIES)
	$(CC) $(CFLAGS) $< -o $@
	@Echo finished building tester!

libraries:
	$(MAKE) -C ./src/
	@Echo finished building libraries!

clean:
	rm tester
	rm tester.d
	rm ./lib/password_checker.so
	rm ./lib/plugins/dict_plugin.so

.PHONY: all clean
