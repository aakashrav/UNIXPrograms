#include "inetd_functions.h"


const char * config_file = "config_file";

void
free_service_structures(service_t * services, int num_services)
{
	for (int i=0; i < num_services; i++)
	{
		free(services[i].port);
		services[i].port = NULL;
		free(services[i].service);
		services[i].service = NULL;
	}
	free(services);
	services = NULL;
}

int
main(int argc, char * argv[])
{
	int err;
	service_t * services = NULL;
	int num_services;

	services = set_services_from_config(config_file, &num_services);

	struct pollfd * fds = NULL;
	fds=initiate_all_services(services, num_services);

	while(1)
	{
		err = poll(fds, num_services, 0);
		if (err == -1)
			error("Error on poll!");

		for (int i=0; i < num_services; i++)
		{
			if (fds[i].revents & POLLIN)
			{
				int client_fd;
				struct sockaddr_storage client_addr;
				socklen_t cli_len = sizeof(struct sockaddr_storage);
				client_fd = accept(fds[i].fd, (struct sockaddr *)&client_addr, &cli_len);

				if (client_fd == -1)
					error("Error on accept");

				pid_t child_executor;

				child_executor = fork();
				if (child_executor < 0)
					error("Error on fork!");

				if (child_executor == 0)
				{
					//Entire process space is copied, so just retreive the pointer from heap
					char buf[4096];
					char * buf_ptr = buf;
					memset(buf_ptr, 0, sizeof(buf));
					//Read in arguments from client
					ssize_t nread;
					nread = read(client_fd, buf_ptr, sizeof(buf));
					if (nread < 0)
					{
						const char * err_msg = "Error occured!";
						write(client_fd, err_msg, strlen(err_msg));
						error("Error on read");
					}
					// Remove trailing newline
					strtok(buf_ptr, "\n");
					char * args[] = {services[i].service, buf_ptr, NULL};
					// char * args[] = {"/bin/cat", "main.c", NULL};
					dup2(client_fd, 1);
					err = execve("/bin/cat", args, NULL);
					// err = execve("/bin/cat", args, NULL);
					if (err == -1)
					{
						error("Error on execve!");
					}
				}

				int status;
				err = wait(&status);
				if (err == -1)
					error("Error on wait!");
				close(client_fd);


			}
		}
	}


	free_service_structures(services, num_services);
	free(fds);
	fds = NULL;
}
