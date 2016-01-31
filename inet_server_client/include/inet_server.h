#ifndef _INET_SERVER_H
#define _INET_SERVER_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

int 
error(const char * message);

int
bind_and_listen(const char * port);

#endif