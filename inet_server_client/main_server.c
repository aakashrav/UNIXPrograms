#include "inet_server.h"

int
main(int argc, char * argv[])
{
	int err;
	if (argc < 2)
	{
		printf("Usage inet_server <port_number>\n");
		fflush(stdout);
		exit(1);
	}

	const char * port = argv[1];

	int sock_fd = bind_and_listen(port);

	while (1)
	{
		struct sockaddr_storage client_addr;
		socklen_t len = sizeof(client_addr);
		int client_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &len);
		if (client_fd == -1)
			error("Error on accepting client connection");
		// Read the target process ID and signal from the child
		char buf[4096];
		char * buf_ptr = buf;
		memset(buf_ptr, 0, sizeof(buf));

		ssize_t nread;
		nread = read(client_fd, buf_ptr, sizeof(buf));
		if (nread == -1)
			error("Error on reading from client");

		// Preprocess the parameters
		char argument[nread+1];
		char * arg_pointer = argument;
		memset(arg_pointer, 0, sizeof(argument));
		strncpy(arg_pointer, buf_ptr, nread);

		char * token;
		token = strsep(&arg_pointer, " ");
		// Assumed that long is large enough to hold pid_t
		pid_t target_pid = (pid_t)strtol(token, NULL, 10);
		// Incase of error or overflow
		if (target_pid == 0)
			error("Error on getting pid");
		token = strsep(&arg_pointer, " ");
		int sig = atoi(token);

		// Send the signal
		err = kill(target_pid, sig);
		if (err == -1)
			error("Error on sending signal to target process");
		else
		{
			printf("Sent signal %d to %d!\n", sig, target_pid);
			fflush(stdout);
		}

	}
}