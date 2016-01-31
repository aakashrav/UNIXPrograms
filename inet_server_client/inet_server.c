#include "inet_server.h"

int
error(const char * message)
{
	perror(message);
	printf("Errno: %d\n", errno);
	exit(1);
}

int
bind_and_listen(const char * port)
{
	int err,sock_fd;
	struct addrinfo hints; struct addrinfo * res; struct addrinfo * res_original;
	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	err = getaddrinfo("127.0.0.1", port, &hints, &res_original);
	if (err != 0)
	{
		error(gai_strerror(err));
	}

	for (res = res_original; res!=NULL; res = res->ai_next)
	{
		if ( (res->ai_family !=  AF_INET) && (res->ai_family != AF_INET6) )
			continue;

		if (res->ai_family == AF_INET)
		{
			sock_fd = socket(AF_INET, SOCK_STREAM, 0);
			if (sock_fd == -1)
				error("Error on getting a socket!");
			err = bind(sock_fd, res->ai_addr, res->ai_addrlen);
			if (err == -1)
				error("Error on binding to socket!");
			err = listen(sock_fd, 5);
			if (err == -1)
				error("Error on listening to socket!");
		}

		if (res->ai_family == AF_INET6)
		{
			sock_fd = socket(AF_INET6, SOCK_STREAM, 0);
			if (sock_fd == -1)
				error("Error on getting a socket!");
			err = bind(sock_fd, res->ai_addr, res->ai_addrlen);
			if (err == -1)
				error("Error on binding to socket!");
			err = listen(sock_fd, 5);
			if (err == -1)
				error("Error on listening to socket!");
		}
	}

	return sock_fd;

}