#ifndef _THREAD_HELPER_H
#define _THREAD_HELPER_H

#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 5
int CUR_NUM_THREADS;

pthread_mutex_t * lock_count;
pthread_cond_t * signal_count_decrement;

int error(const char * message);

typedef struct arg_wrapper {
  char * hostname;
  char * port;
} arg_wrapper_t;

void * thread_function(void * arg);

#endif
