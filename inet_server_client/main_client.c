#include "inet_client.h"

int 
main(int argc, char * argv[])
{
	int err;

	if (argc < 3)
	{
		printf("Usage inet_client <port_number> <signal>\n");
		fflush(stdout);
		exit(1);
	}

	const char * port = argv[1];
	const int sig = atoi(argv[2]);

	err = send_signal_to_server(port, sig);

	sigset_t signals;
	int recv_signal;

	sigemptyset(&signals);
	sigaddset(&signals, sig);

	printf("Waiting for signals..\n");
	printf("My process id: %d", getpid());
	fflush(stdout);

	sigwait(&signals, &recv_signal);

	if (recv_signal == sig)
	{
		printf("Received Signal!");
		fflush(stdout);
	}

}