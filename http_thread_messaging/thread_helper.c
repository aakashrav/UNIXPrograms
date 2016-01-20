#include "thread_helper.h"

int error(const char * message)
{
  perror(message);
  printf("Error number: %d", errno);
  exit(1);
}

void *
thread_function(void * arg)
{
  arg_wrapper_t * my_arg_ptr = (arg_wrapper_t *)arg;
  arg_wrapper_t my_args = *my_arg_ptr;

  char * my_hostname = my_args.hostname;
  char * my_port = my_args.port;

  struct addrinfo hints;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  // hints.ai_family = IPPROTO_TCP;
  int ipv4 = 0;
  int ipv6 = 0;
  struct addrinfo * res, * res_original;

  int i = getaddrinfo(my_hostname, my_port,
  &hints, &res_original);

  if (i != 0)
  {
    gai_strerror(i);
    printf("Error on getaddrinfo!\n");
    fflush(stdout);
    exit(1);
  }

  for (res = res_original; res!=NULL; res=res->ai_next)
  {
    if ( (res->ai_family != AF_INET) && (res->ai_family != AF_INET6) )
      continue;

    if (res->ai_family == AF_INET)
    {
      ipv4=1;
      break;
    }
    else
    {
      ipv6=1;
      break;
    }

  }

  if (ipv4 == 1)
  {
    int fd=socket(AF_INET,SOCK_STREAM,0);
    if (fd == -1)
    {
      error("Error on socket");
    }
    i = connect(fd, res->ai_addr, res->ai_addrlen);
    if (i == -1)
    {
      error("Error on connecting");
    }
    char * buf = "Hello World!\n";
    i = send(fd, buf, strlen(buf),0);
    if (i==-1)
    {
      error("Error on sending message");
    }
    printf("Sent to server!\n");
    fflush(stdout);
  }
  else if (ipv6 == 1)
  {
    int fd=socket(AF_INET6,SOCK_STREAM,0);
    if (fd == -1)
    {
      error("Error on socket");
    }

    i = connect(fd, res->ai_addr, res->ai_addrlen);
    if (i == -1)
    {
      error("Error on connecting");
    }
    char * buf = "Hello World!\n";
    i = send(fd, buf, strlen(buf),0);
    if (i==-1)
    {
      error("Error on sending message");
    }
    printf("Sent to server!\n");
    fflush(stdout);
  }
  else
  {
    error("No addersses found.. ");
  }

  freeaddrinfo(res_original);

  pthread_mutex_lock(lock_count);

  printf("Decrementing number of threads!\n");
  fflush(stdout);

  CUR_NUM_THREADS--;

  pthread_mutex_unlock(lock_count);
  pthread_cond_signal(signal_count_decrement);

  free(my_args.hostname);
  my_args.hostname = NULL;
  free(my_args.port);
  my_args.port = NULL;

  return 0;

}
