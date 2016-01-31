#include "inet_server.h"

int
error(const char * message)
{
	perror(message);
	printf("Errno: %d\n", errno);
	exit(1);
}

// Initlialize mutexes, conditional variables, and job queue
int
init()
{
	int err;
	available_jobs = 0;

	job_queue_lock = calloc(1, sizeof(pthread_mutex_t));
	if (job_queue_lock == NULL)
		error("Error on allocaitng job queue lock");
	err = pthread_mutex_init(job_queue_lock, NULL);
	if (err != 0)
		error("Error on initializing job queue mutex");

	job_available = calloc(1,sizeof(pthread_cond_t));
	if (job_available == NULL)
		error("Error on allocating job available condition!");
	err = pthread_cond_init(job_available, NULL);
	if (err != 0)
		error("Error on initializing job_available condition");

	head = calloc(1, sizeof(struct job));
	if (head == NULL)
		error("Error on allocating job queue");

	return 0;
}

// Destroy mutexes, conditional variables, and job queue
int
destroy()
{
	pthread_mutex_destroy(job_queue_lock);
	free(job_queue_lock);
	job_queue_lock = NULL;

	pthread_cond_destroy(job_available);
	free(job_available);
	job_available = NULL;

	free_jobs(head);

	return 0;
}

// Enqueue job into job queue
int
enqueue(job_t *head, int fd)
{
	job_t * jb = head;
	while (jb->next != NULL)
		jb= jb->next;

	job_t * new_job = calloc(1, sizeof(struct job));
	new_job->fd = fd;
	new_job->previous = jb;
	new_job->next = NULL;
	jb->next = new_job;

	return 0;
}

// Dequeue job from job queue
int
dequeue(job_t *head)
{
	job_t * jb = head;
	jb = jb->next;

	if (jb != NULL)
	{
		int fd = jb->fd;

		job_t * nxt_job = jb->next;
		jb->previous->next = nxt_job;
		if (nxt_job != NULL)
			nxt_job->previous = jb->previous;
		
		free(jb);
		return fd;
	}
	else
	{
		printf("No jobs available!\n");
		fflush(stdout);
		return -1;
	}
}

// Free all the remaining jobs in the queue, cleanup function
void 
free_jobs(job_t * head)
{
	while (head!= NULL)
	{
		job_t * next = head->next;
		free(head);
		head = next;
	}

	// Extra safety, don't want double free problems later
	head = NULL;
}

// Thread function that processes jobs
void *
thread_signal_sender(void * args)
{
	int err;
	while (1)
	{
		// Lock the job queue
		err = pthread_mutex_lock(job_queue_lock);

		if (err != 0)
		{
			destroy();
			error("Error on locking job queue thread");
		}

		while (available_jobs == 0)
		{
			pthread_cond_wait(job_available, job_queue_lock);
		}

		// Dequeue the latest job and process
		int client_fd = dequeue(head);

		// Decrement the number of jobs available
		available_jobs--;

		// Unlock the job queue and continue processing
		err = pthread_mutex_unlock(job_queue_lock);
		if (err != 0)
		{
			destroy();
			error("Error unlocking job queue");
		}

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

// Helper function to help the server bind to a port and listen
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