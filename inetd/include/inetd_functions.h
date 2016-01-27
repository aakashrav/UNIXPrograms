#ifndef _INETD_FUNCTIONS_H
#define _INETD_FUNCTIONS_H

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <poll.h>

void 
error(const char * message);

typedef struct service_structure {
	char * service;
	char * port; 
} service_t;

int
set_services_from_config(service_t * services, const char * config_file,
	int * total_services);

int
initiate_all_services(service_t * serv_struct, struct pollfd * fds, int num_services);

#endif

