#ifndef _INET_SERVER_H
#define _INET_SERVER_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>

#define NUM_THREADS 5

// Number of available jobs for processing
int available_jobs; 
// Mutex to job queue
pthread_mutex_t * job_queue_lock;
// Conditional variable signalling that new job has become available for processing
pthread_cond_t * job_available;

// Node in kob queue linked list
typedef struct job {
	int fd;
	struct job * next;
	struct job * previous;
} job_t;

// Head of job queueu linked list
job_t * head;

int 
error(const char * message);

// Initlialize mutexes, conditional variables, and job queue
int
init();

// Destroy mutexes, conditional variables, and job queue
int 
destroy();

// Enqueue job into job queue
int
enqueue(job_t *head, int fd);

// Dequeue job from job queue
int
dequeue(job_t *head);

// Free all the remaining jobs in the queue, cleanup function
void
free_jobs(job_t * head);

// Thread function that processes jobs
void *
thread_signal_sender(void * args);

// Helper function to help the server bind to a port and listen
int
bind_and_listen(const char * port);

#endif