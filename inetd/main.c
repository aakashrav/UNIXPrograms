#include "inetd_functions.h"


const char * config_file = "config_file";

int
main(int argc, char * argv[])
{
	int err;
	service_t * services = NULL;
	int num_services;

	set_services_from_config(services, config_file, &num_services);

	struct pollfd * fds = NULL;
	initiate_all_services(services, fds, num_services);

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
					//Read in arguments from client
					ssize_t nread;
					nread = read(client_fd, buf_ptr, sizeof(buf));
					if (nread < 0)
					{
						const char * err_msg = "Error occured!";
						write(client_fd, err_msg, strlen(err_msg));
						error("Error on read");
					}

					char * args[] = {services[i].service, buf_ptr, NULL};
					dup2(client_fd, 1);
					err = execve(services[i].service, args, NULL);
					if (err == -1)
						error("Error on execve!");
				}

				int status;
				err = wait(&status);
				if (err == -1)
					error("Error on wait!");
				close(client_fd);


			}
		}
	}


	free(services);
	services = NULL;
	free(fds);
	fds = NULL;
}