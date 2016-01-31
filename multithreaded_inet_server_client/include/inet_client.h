#ifndef _INET_CLIENT_H
#define _INET_CLIENT_H

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
send_signal_to_server(const char * port, const int signal);


#endif
