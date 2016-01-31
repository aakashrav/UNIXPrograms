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

	init();

	pthread_t arr[NUM_THREADS];
	for (int i=0; i < NUM_THREADS; i++)
	{
		pthread_create(&arr[i], NULL, thread_signal_sender, NULL);
	}

	int sock_fd = bind_and_listen(port);

	while (1)
	{
		struct sockaddr_storage client_addr;
		socklen_t len = sizeof(client_addr);
		int client_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &len);
		if (client_fd == -1)
			error("Error on accepting client connection");
		
		// Lock the job queue
		err = pthread_mutex_lock(job_queue_lock);

		if (err != 0)
		{
			destroy();
			error("Error on locking mutex");
		}

		// Enqueue a new job
		enqueue(head, client_fd);

		// Increment the job counter
		available_jobs++;

		// Unlock the job queue
		err = pthread_mutex_unlock(job_queue_lock);
		if (err != 0)
			error("Error on unlocking thread pool");

		//Signal to any waiting threads that a new job is available
		err = pthread_cond_signal(job_available);

	}

	destroy();
}