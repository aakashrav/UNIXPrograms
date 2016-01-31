#include "inet_client.h"

int
error(const char * message)
{
	perror(message);
	printf("Errno: %d\n", errno);
	exit(1);
}

int
send_signal_to_server(const char * port, const int sig)
{
	int err;
	struct addrinfo hints; struct addrinfo * res; struct addrinfo * res_original;
	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	err = getaddrinfo("127.0.0.1", port, &hints, &res_original);
	if (err != 0)
	{
		error(gai_strerror(err));
	}

	for (res = res_original; res!=NULL; res=res->ai_next)
	{
		if ( (res->ai_family != AF_INET) && (res->ai_family != AF_INET6) )
			continue;

		if (res->ai_family == AF_INET)
		{
			int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
			if (sock_fd == -1)
				error("Error on getting socket");
			err = connect(sock_fd, res->ai_addr, res->ai_addrlen);
			if (err == -1)
			{
				error("Error on connecting to server");
			}

			char buf[4096];
			memset(buf, 0, sizeof(buf));
			char * message = buf;
			pid_t pid = getpid();

			sprintf(message,"%d %d",pid,sig);
			ssize_t nwrote = write(sock_fd, message, strlen(message));
			if (nwrote == -1)
				error("Error on sending message to server!");
			close(sock_fd);
			break;
		}

		if (res->ai_family == AF_INET6)
		{
			int sock_fd = socket(AF_INET6, SOCK_STREAM, 0);
			if (sock_fd == -1)
				error("Error on getting socket");
			err = connect(sock_fd, res->ai_addr, res->ai_addrlen);
			if (err == -1)
			{
				error("Error on connecting to server");
			}

			char buf[4096];
			memset(buf, 0, sizeof(buf));
			char * message = buf;
			pid_t pid = getpid();

			sprintf(message,"%d\n%d",pid,sig);
			ssize_t nwrote = write(sock_fd, message, strlen(message));
			if (nwrote == -1)
				error("Error on sending message to server!");
			close(sock_fd);
			break;
		}
	}

	freeaddrinfo(res_original);
	return 0;
}