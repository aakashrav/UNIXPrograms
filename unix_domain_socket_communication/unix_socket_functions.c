#include "unix_socket_functions.h"

void
error(const char * message)
{
	perror(message);
	printf("Errno: %d", errno);
	fflush(stdout);
	exit(1);
}

int
bind_and_listen(char * pathname)
{
	int server_fd;
	int err;
	unlink(pathname);

	// Create the socket address structure
	struct sockaddr_un serv_addr;
	serv_addr.sun_family = AF_UNIX;
	strcpy(serv_addr.sun_path, pathname);
	socklen_t len = (socklen_t)sizeof(struct sockaddr_un);

	// Bind pathname to a socket
	server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (server_fd < 0)
		error("Error getting socket");

	err = bind(server_fd, (struct sockaddr *)&serv_addr, len);
	if (err < 0)
	{
		unlink(pathname);
		error("Error binding socket to pathname");
	}

	err = listen(server_fd, 5);
	if (err < 0)
	{
		unlink(pathname);
		error("Error on listening to socket");
	}

	return server_fd;

}
