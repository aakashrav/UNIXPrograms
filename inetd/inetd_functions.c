#include "inetd_functions.h"

void 
error(const char * message)
{
	perror(message);
	printf("errno: %d\n", errno);
	exit(1);
}

int
set_services_from_config(service_t * services, const char * config_file,
	int * total_services)
{
	int fd = open(config_file, O_RDONLY);
	if (fd == -1)
		error("Error opening config file");

	char buf[4096];
	char * bf_ptr = buf;
	memset(bf_ptr, 0, sizeof(buf));

	ssize_t nread;
	int num_services = 5;
	int current_service = 0;
	service_t * service_array = calloc(num_services, sizeof(struct service_structure));

	while ( (nread = read(fd, bf_ptr, sizeof(buf))) > 0)
	{
		if (current_service > num_services)
		{
			num_services += 5;
			service_array = realloc(service_array, num_services);
		}

		current_service++;
		strncpy(service_array[current_service].port, bf_ptr, nread);
		memset(bf_ptr, 0, sizeof(buf));

		// Now get the service
		nread = read(fd, bf_ptr, sizeof(buf));
		if (nread <= 0)
			break;
		strncpy(service_array[current_service].service, bf_ptr, nread);
		memset(bf_ptr, 0, sizeof(buf));
	}

	if (nread == -1)
		error("Error on reading config file");

	*total_services = num_services;
	return 0;

}

int
initiate_all_services(service_t * services, struct pollfd * fds, int num_services)
{
	int err, fd;
	struct addrinfo hints;struct addrinfo * res;struct addrinfo * res_original;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	fds = calloc(num_services, sizeof(struct pollfd));

	memset(&hints, 0, sizeof(struct addrinfo));

	for (int i=0; i < num_services; i++)
	{	
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
	return 0;
}