SOURCES_CLIENT=main_client.c inet_client.c
SOURCES_SERVER=main_server.c inet_server.c
OBJECTS_CLIENT=$(SOURCES_CLIENT:.c=.o)
OBJECTS_SERVER=$(SOURCES_SERVER:.c=.o)
CC=gcc
CFLAGS=-c -Wall -I./include -std=gnu99 -MP -MMD

all: inet_client inet_server

.c.o:
	@$(CC) $(CFLAGS) $<

inet_client: $(OBJECTS_CLIENT)
	@$(CC) -o $@ $(OBJECTS_CLIENT)
	@echo Created inet client!

inet_server: $(OBJECTS_SERVER)
	@$(CC) -o $@ -pthread $(OBJECTS_SERVER)
	@echo Created inet server!

clean:
	-rm inet_client inet_server
	-rm $(SOURCES_CLIENT:.c=.d)
	-rm $(SOURCES_SERVER:.c=.d)
	-rm $(OBJECTS_CLIENT) $(OBJECTS_SERVER)

.PHONY: all clean
