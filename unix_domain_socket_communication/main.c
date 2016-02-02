#include "unix_socket_functions.h"

int
main(int argc, char * argv[])
{
	int err;
	unlink(pathname);

	pathname = "unixendpoint";

	int serverfd = bind_and_listen(pathname);

	pid_t child = fork();

	if (child < 0)
	{
		error("Error on forking server");
	}

	if (child == 0)
	{
		int client_fd;
		client_fd = socket(AF_UNIX, SOCK_STREAM,0);
		if (client_fd <0)
		{	
			unlink(pathname);
			error("Error on creating client socket");
		}

		struct sockaddr_un serv_addr;
		serv_addr.sun_family = AF_UNIX;
		strcpy(serv_addr.sun_path, pathname);
		socklen_t len = sizeof(struct sockaddr_un);

		err = connect(client_fd, (struct sockaddr *)&serv_addr, len);
		if (err < 0)
		{
			unlink(pathname);
			error("Error on connecting to server");
		}


		for (int i=0; i < 100; i++)
		{
			char * buf = "Hello world!";
			char * buf_ptr = buf;
			ssize_t n_wrote_read = write(client_fd, buf_ptr, strlen(buf));
			if (n_wrote_read < 0)
			{	
				unlink(pathname);
				error("Error writing to server!");
			}

			char buf_read[4096];
			char * buf_read_ptr = buf_read;
			memset(buf_read_ptr, 0, sizeof(buf_read));

			n_wrote_read = read(client_fd, buf_read_ptr, sizeof(buf_read));
			if (n_wrote_read < 0)
			{	
				unlink(pathname);
				error("Error on reading from server");
			}

			printf("Read from server! : %s\n", buf_read_ptr);

		}

		return 0;
	}

	struct sockaddr_un clientaddr;
	socklen_t len = (socklen_t)sizeof(struct sockaddr_un);

	int client_fd = accept(serverfd, (struct sockaddr *)&clientaddr, &len);
	if (client_fd < 0)
	{	
		unlink(pathname);
		error("Error on accepting client!");
	}

	for (int i=0; i < 100; i++)
	{
		char * buf = "Hello world!\n";
		char * buf_ptr = buf;
		ssize_t n_wrote_read = write(client_fd, buf_ptr, strlen(buf));
		if (n_wrote_read < 0)
		{
			unlink(pathname);
			error("Error writing to server!");
		}

		char buf_read[4096];
		char * buf_read_ptr = buf_read;
		memset(buf_read_ptr, 0, sizeof(buf_read));

		n_wrote_read = read(client_fd, buf_read_ptr, sizeof(buf_read));
		if (n_wrote_read < 0)
		{
			unlink(pathname);
			error("Error on reading from server");
		}

		printf("Read from client! : %s\n", buf_read_ptr);

	}

	// Prevent zombies
	int status;
	pid_t p = wait(&status);
	if (p < 0)
		error("Error on waiting for process");

	unlink(pathname);
}

