#include "thread_helper.h"
#include <signal.h>

volatile sig_atomic_t quit =0;

void
sig_handler(int signal)
{
  if (signal == SIGUSR1)
    quit=1;
}

void
init()
{
  CUR_NUM_THREADS=0;
  lock_count = calloc(1, sizeof(pthread_mutex_t));
  signal_count_decrement = calloc(1, sizeof(pthread_cond_t));
}


void
destroy()
{
  free(lock_count);
  free(signal_count_decrement);
}

int
main(int argc, const char *argv[]) {

  const char * filename;
  printf("%d",getpid());

  if (argc <2)
  {
    printf("Usage ./multithread_spammer <input_file>");
    exit(1);
  }
  else
  {
    filename = argv[1];
  }

  struct sigaction signal_container = {sig_handler,0,0};

  if (sigaction(SIGUSR1, &signal_container,NULL) == -1)
    error("Error registering a signal!");

  FILE * stream = fopen(filename, "r");
  if (stream == NULL)
    error("Error on opening input file");


  init();

  while(!quit)
  { 
    pthread_mutex_lock(lock_count);
    while (CUR_NUM_THREADS == NUM_THREADS)
    {
      pthread_cond_wait(signal_count_decrement, lock_count);
    }

    printf("Incrementing number of threads!\n");
    fflush(stdout);

    // arg_wrapper_t thread_args = {"127.0.0.1","7080"};
    arg_wrapper_t thread_args;

    char * line = calloc(4096, 1);
    size_t linecapp = 0;

    int read = getline(&line, &linecapp, stream);
    if (read == -1)
    {
      free(line);
      line = NULL;
      break;
    }

    char * hashtag_occurence = strchr(line, '#');
    int len_hostname = (hashtag_occurence) - line;
    char * hostname= calloc(len_hostname+1,1);
    strncat(hostname, line, len_hostname);

    // Remove the newline character, unless we are on the final line,
    // in which case we don't do anything since there is no newline character
    if (feof(stream) == 0)
      read = read-1;

    int len_port = (line + read) - (hashtag_occurence+1);
    char * port = calloc(len_port+1,1);
    strncat(port, hashtag_occurence+1, len_port);

    thread_args.hostname = hostname;
    thread_args.port = port;

    pthread_t dummy_var;

    pthread_create(&(dummy_var), NULL,
      thread_function, (void *)&thread_args);

    if (line != NULL)
    {
      free(line);
      line = NULL;
    }
    CUR_NUM_THREADS++;

    pthread_mutex_unlock(lock_count);

    sleep(1);

  }

  printf("I finished spamming the server!\n");
  fflush(stdout);
  destroy();
}
