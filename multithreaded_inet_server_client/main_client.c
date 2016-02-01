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
	if (err!=0)
		error("Error on sending signal to server!");
	/*
	 * We first must block the signal that we want to receive to prevent the
	 * default handler from getting run. The blocked signal will then get put in our
	 * signal queue (one per signal), which will then unblock sigwait(). However, if we set
	 * SIG_IGN, the signal never gets delivered to our signal queue at all, leading to an indefinite
	 * wait of sigwait().
	 */
	sigset_t blocked_signals;
	sigemptyset(&blocked_signals);
	sigaddset(&blocked_signals, sig);

	 /* We don't bother with the 
        oldset argument. */
	sigprocmask(SIG_BLOCK, &blocked_signals, NULL);

	/*
	 * Now set up sigwait()
	 */
	sigset_t desired_signals;
	int recv_signal;

	sigemptyset(&desired_signals);
	sigaddset(&desired_signals, sig);

	printf("Waiting for signals..\n");
	printf("My process id: %d\n", getpid());
	fflush(stdout);

	sigwait(&desired_signals, &recv_signal);

	if (recv_signal == sig)
	{
		printf("Received Signal!\n");
		fflush(stdout);
	}

}