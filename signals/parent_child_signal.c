#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>

volatile sig_atomic_t QUIT_FLAG=0;

void handler(int signal)
{
	QUIT_FLAG=1;
}

void error(const char * message)
{
	perror(message);
	printf("ERROR NO.: %d", errno);
	exit(errno);
}

int main(int argc, char * argv[])
{
	if (signal(SIGUSR1, handler) == SIG_ERR)
	{
		error("Error registering signal handler");
	}
	
	pid_t pid;

	if ( (pid = fork()) < 0)
	{
		error("Error forking!");
	}

	if (pid ==0)
	{
		for (int i =0; i < 25; i++)
		{
			if (QUIT_FLAG)
				break;
			printf("Hello world!");
			fflush(stdout);
			sleep(1);
		}
	}
	else
	{	
		sleep(5);
		if ( (kill(pid, SIGUSR1)) < 0)
		{
			error("Error sending signal to child!");
		}
		else
		{
			printf("Successfully sent child signal!");
			fflush(stdout);
		}
	}

	return 0;

}