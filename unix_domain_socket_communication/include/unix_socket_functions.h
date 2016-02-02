#ifndef _UNIX_SOCKET_FUNCTIONS_H
#define _UNIX_SOCKET_FUNCTIONS_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/un.h>
#include <string.h>

char * pathname;

void
error(const char * message);

int
bind_and_listen(char * pathname);

#endif