#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

/**
 * The name of the file we will be playing with.
 * After running the program, please run 'cat random_file'
 * You should see that process A and process B are writing
 * to the file separately. That is, the lines from A don't get
 * mixed with the lines from B, and vice versa. The lines are
 * totally separate. Each process writes a 100 times so this should
 * be visible
 */
const char * FILENAME = "random_file";

/* The length of the file in total. This parameter is necessary or else we might
 * get race conditions on newly written bytes, since we can't lock them. Therefore,
 * we pre-write all the bytes we need, then lock the parts that we need, rewrite them
 * and unlock them
 */
const int FILE_LEN = 2000;

/* The length of the message we will write. If a process fails to write to the file
 * because the other process has locked a part of it, we will simply increment our
 * offset by this size and try and write to the next part!
 */
const int message_length = sizeof("Hello from process A!\n");


void error(const char * message)
{
	perror(message);
	printf("Error number: %d", errno);
	exit(errno);
}

int main(int argc, char * argv[])
{
	/* We first unlink the file to start with a clean slate*/
	unlink(FILENAME);
	int fd = open(FILENAME, O_CREAT | O_RDWR, 0666);

	/* Pre-extend the file length to the total length we need, then just lock the parts
	 * we require, SEEK_SET allows us to input the desired offset*/
	(void) lseek(fd, SEEK_SET, FILE_LEN-1);

	pid_t pid;
	if ((pid = fork()) < 0) 
	{
		error("Error forking");
	}

	if (pid == 0)
	{
		/* Open a new file descriptor for the child, so we don't have two file
		* descriptors for separate processes and avoid hard to debug errors
		*/
	 	 int fd2 = open(FILENAME, O_RDWR);

		 if (fd2 < 0)
		 {
	 	 	error("Error opening file descriptor child");
		 }

		for (int i = 0; i < 100; i++)
		{

			 char buf[] = "Hello from process A!\n";

			/* The child locks from the end of the file onwards.
			 * At the start, the file is 0 bytes and hence this is 
			 * equivalent to locking from the beginning onwards
			 */
			struct flock f1;
			f1.l_type = F_WRLCK;
			f1.l_whence = SEEK_CUR;
			f1.l_start = 0;
			f1.l_len = strlen(buf);
			f1.l_pid = getpid();

			int block_status; 

			while ( (block_status = fcntl(fd2, F_SETLK, &f1)) < 0)
			{
				//Shift the offset and try to write to the next free location
				(void)lseek(fd2, SEEK_SET, lseek(fd2, 0, SEEK_CUR) + message_length);
			}

			if (write(fd2, buf, strlen(buf)) < 0)
			{
				error("Error writing child process A!");
			}

			/* Unlock from (SEEK_CUR - strlen(buf)) to SEEK_CUR*/
			int negative_offset = strlen(buf) * -1;
			f1.l_start = negative_offset;
			f1.l_type = F_UNLCK;

			if (fcntl(fd2, F_SETLK, &f1) < 0)
			{
				error("Error unlocking file child!");
			}
		}
	}
	else
	{
		//Same code and logic, but for process B
		for (int i = 0; i < 100; i++)
		{
			char buf[] = "Hello from process B!\n";

			struct flock f1;
			f1.l_type = F_WRLCK;
			f1.l_whence = SEEK_CUR;
			f1.l_start = 0;
			f1.l_len = strlen(buf);
			f1.l_pid = getpid();

			int block_status;

			while ( (block_status = fcntl(fd, F_SETLK, &f1)) < 0)
			{
				//Shift the offset and try to write to the next free location
				(void)lseek(fd, SEEK_SET, lseek(fd, 0, SEEK_CUR) + message_length);
			}

			if (write(fd, buf, strlen(buf)) < 0)
			{
				error("Error writing parent process B!");
			}

			/* Unlock from (SEEK_CUR - strlen(buf)) to SEEK_CUR*/
			int negative_offset = strlen(buf) * -1;
			f1.l_start = negative_offset;
			f1.l_type = F_UNLCK;
			if (fcntl(fd, F_SETLK, &f1) < 0)
			{
				error("Error unlocking file parent!");
			}
		}
	}
}