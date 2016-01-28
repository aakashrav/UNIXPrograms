#include "inetd_functions.h"

void
error(const char * message)
{
	perror(message);
	printf("errno: %d\n", errno);
	exit(1);
}

service_t *
set_services_from_config(const char * config_file,
	int * total_services)
{
	int fd = open(config_file, O_RDONLY);
	if (fd == -1)
		error("Error opening config file");

	char buf[4096];
	char * bf_ptr = buf;
	memset(bf_ptr, 0, sizeof(buf));

	/*
	 * Not allowed to use any standard library functions to process file
	 * so this gets kind of complicated, but works.
	 */

	ssize_t nread;
	int num_services = 5;
	int current_service = 0;
	service_t * services = calloc(num_services, sizeof(struct service_structure));
	int port_or_service = 0;
	char read_char_buf[1];
	char * readchar = read_char_buf;

	while ( (nread = read(fd, readchar, 1) > 0))
	{
		if (*readchar != '\n')
		{
			strncat(bf_ptr, readchar, 1);
			continue;
		}

		if ( (port_or_service % 2) == 0)
		{
			if (current_service > num_services)
			{
				num_services += 5;
				services = realloc(services, num_services);
			}

			services[current_service].port = calloc(strlen(bf_ptr) +1, 1);
			strcpy(services[current_service].port, bf_ptr);
			printf("Read Port: %s\n", services[current_service].port);
			fflush(stdout);
			// Next read will be a service
			port_or_service++;
			memset(bf_ptr, 0, sizeof(buf));
			continue;
		}

		if ( (port_or_service % 2) == 1)
		{
			services[current_service].service = calloc(strlen(bf_ptr)+1, 1);
			strcpy(services[current_service].service, bf_ptr);

			printf("Read Service: %s\n", services[current_service].service);
			fflush(stdout);

			memset(bf_ptr, 0, sizeof(buf));
			port_or_service++;
			current_service++;
		}
	}

	//One final service copy, since we reached the end of file
	if (strlen(bf_ptr) != 0)
	{
		services[current_service].service = calloc(strlen(bf_ptr)+1, 1);
		strcpy(services[current_service].service, bf_ptr);

		printf("Read Service: %s\n", services[current_service].service);
		fflush(stdout);

		memset(bf_ptr, 0, sizeof(buf));
		port_or_service++;
		current_service++;
	}

	if (nread == -1)
		error("Error on reading config file");

	*total_services = current_service;
	return services;

}

struct pollfd *
initiate_all_services(service_t * services, int num_services)
{
	int err, fd;
	struct addrinfo hints;struct addrinfo * res;struct addrinfo * res_original;
	memset(&hints, 0, sizeof(struct addrinfo));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	struct pollfd * fds = calloc(num_services, sizeof(struct pollfd));

	for (int i=0; i < num_services; i++)
	{
		printf("%s\n", services[i].port);
		fflush(stdout);
		err = getaddrinfo("127.0.0.1", services[i].port, &hints, &res_original);
		if (err)
		{
			error(gai_strerror(err));
		}

		for (res = res_original; res!=NULL; (res=res->ai_next))
		{
			if ( (res->ai_family != AF_INET) && (res->ai_family != AF_INET6) )
				continue;
			else
				break;
		}

		if (res->ai_family == AF_INET)
		{
			fd = socket(AF_INET, SOCK_STREAM, 0);
			if (fd == -1)
				error("Error on socket");

			err = bind(fd, res->ai_addr, res->ai_addrlen);
			if (err == -1)
				error("Error on binding");

			err = listen(fd, 3);
			if (err == -1)
				error("Error on listen");

			fds[i].fd = fd;
			fds[i].events = POLLIN;
		}

		if (res->ai_family == AF_INET6)
		{
			fd = socket(AF_INET6, SOCK_STREAM, 0);
			if (fd == -1)
				error("Error on socket");

			err = bind(fd, res->ai_addr, res->ai_addrlen);
			if (err == -1)
				error("Error on binding");

			err = listen(fd, 3);
			if (err == -1)
				error("Error on listen");

			fds[i].fd = fd;
			fds[i].events = POLLIN;
		}
	}

	freeaddrinfo(res_original);
	res_original = NULL;
	return fds;
}
