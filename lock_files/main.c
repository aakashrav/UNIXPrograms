#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

void
error(const char * message)
{
	perror(message);
	printf("Errno: %d", errno);
	fflush(stdout);
	exit(1);
}

void lock(char * lock_file)
{
	int fd;
	while ( (fd = open(lock_file, O_CREAT | O_RDWR | O_EXCL, 0600)) == -1)
		sleep(1);
	close(fd);
}

void unlock(char * lock_file)
{
	unlink(lock_file);
}

char * write_file = "common_resource";
char * lock_file = "lock_file.lk";

int
main(int argc, char * argv[])
{
	unlink(write_file);

	int err;
	pid_t child;

	child = fork();
	if (child == -1)
		error("Error on fork!\n");

	if (child == 0)
	{
		int child_fd = open(write_file, O_CREAT | O_RDWR | O_APPEND, 0644);
		if (child_fd == -1)
			error("Error opening real file\n");

		for (int i=0; i<100; i++)
		{
			// Critical section
			lock(lock_file);

			printf("Child writing!\n");
			fflush(stdout);

			const char * message = "Hello world from child!\n";

			err = write(child_fd, message, strlen(message));
			if (err == -1)
				error("Error on writing to file child!");

			//Critical section over
			unlock(lock_file);
		}
		return 0;

	}

	int parent_fd = open(write_file, O_CREAT | O_RDWR | O_APPEND, 0644);
	if (parent_fd == -1)
		error("Error opening real file\n");

	for (int i=0; i<100; i++)
	{
		// Critical section
		lock(lock_file);

		printf("Parent writing!\n");
		fflush(stdout);

		const char * message = "Hello world from parent!\n";

		err = write(parent_fd, message, strlen(message));
		if (err == -1)
			error("Error on writing to file child!");

		//Critical section over
		unlock(lock_file);
	}

	return 0;
}
