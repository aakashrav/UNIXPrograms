#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

const char * FILENAME = "random_file";

void error(const char * message)
{
	perror(message);
	printf("Error number: %d", errno);
	exit(errno);
}

int main(int argc, char * argv[])
{
	unlink(FILENAME);
	int fd = open(FILENAME, O_CREAT | O_RDWR);

	pid_t pid;
	if ((pid = fork()) < 0) 
	{
		error("Error forking");
	}

	if (pid == 0)
	{
		for (int i = 0; i < 10; i++)
		{
			struct flock f1;
			f1.l_type = F_WRLCK;
			f1.l_whence = SEEK_END;
			f1.l_start = 0;
			f1.l_len = 0;
			f1.l_pid = getpid();
			if (fcntl(fd, F_SETLKW, &f1) < 0)
			{
				error("Error locking file child!");
			}

			char buf[] = "Hello from process A!\n";
			if (write(fd, buf, strlen(buf)) < 0)
			{
				error("Error writing child process A!");
			}
			f1.l_type = F_UNLCK;

			/* We need to unlock the entire length of file that this process has written.
			 * SEEK_END will only look at the current file end, we need to look back before 
			 * the process began its writing.
			 */
			int negative_offset = strlen(buf) * -1;
			f1.l_start = negative_offset;
			if (fcntl(fd, F_SETLK, &f1) < 0)
			{
				error("Error unlocking file child!");
			}
		}
	}
	else
	{
		//Same code, but for process B
		for (int i = 0; i < 10; i++)
		{
			struct flock f1;
			f1.l_type = F_WRLCK;
			f1.l_whence = SEEK_END;
			f1.l_start = 0;
			f1.l_len = 0;
			f1.l_pid = getpid();
			if (fcntl(fd, F_SETLKW, &f1) < 0)
			{
				error("Error locking file parent!");
			}

			char buf[] = "Hello from process B!\n";
			if (write(fd, buf, strlen(buf)) < 0)
			{
				error("Error writing parent process B!");
			}
			f1.l_type = F_UNLCK;

			/* We need to unlock the entire length of file that this process has written.
			 * SEEK_END will only look at the current file end, we need to look back before 
			 * the process began its writing.
			 */
			int negative_offset = strlen(buf) * -1;
			f1.l_start = negative_offset;
			if (fcntl(fd, F_SETLK, &f1) < 0)
			{
				error("Error unlocking file parent!");
			}
		}
	}
}