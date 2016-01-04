#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <poll.h>
#include <inttypes.h>

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
const int FILE_LEN = 4000;

/* The length of the message we will write. If a process fails to write to the file
 * because the other process has locked a part of it, we will simply increment our
 * offset by this size and try and write to the next part!
 */
const int message_length = sizeof("Hello from process A!\n");

/* Pipe for interprocess communication of the current offset*/
int pipe_p1_writer[2];
int pipe_p2_writer[2];


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

	// Instantiate the pipes for IPC
	if (pipe(pipe_p1_writer) == -1)
		error("Pipe 1 error");
	if (pipe(pipe_p2_writer) == -1)
		error("Pipe 2 error");

	pid_t pid;
	if ((pid = fork()) < 0) 
	{
		error("Error forking");
	}

	if (pid == 0)
	{
		// Close the reader part of the pipe, we are only writing to it
		close(pipe_p1_writer[0]);
		// Close the writer part of the pipe, we are only reading from it
		close(pipe_p2_writer[1]);

		FILE * stream = fdopen(pipe_p2_writer[0], "r");

		struct pollfd fds[] =
		{
			{ pipe_p2_writer[0], POLLIN}
		};

		/* Open a new file descriptor for the child, so we don't have two file
		* descriptors for separate processes and avoid hard to debug errors
		*/
	 	int fd2 = open(FILENAME, O_RDWR);

		if (fd2 < 0)
		{
	 		error("Error opening file descriptor child");
		}

		off_t offset =0;
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
				if (offset >= FILE_LEN)
					break;

				/* Wait to get the new offset value from the other process.
				 * We wait for a total of .5 seconds before going ahead and writing.
				 * This prevents deadlocks.
				 */
				poll(fds, 1, 500);
				char * offset_buffer = malloc(10);
	
				//We timed out on poll- other process has finished
				if (fds[0].revents != POLLIN)
					break;

				//Get the current latest offset update from the other file
				size_t linecapp = 10;
				if (getline(&offset_buffer,&linecapp, stream) < 0)
					error("getline error");

				// Shift the offset so we can try to write to the next free location
				offset = strtol(offset_buffer, NULL, 10);

				(void)lseek(fd2, SEEK_SET, offset);
				free(offset_buffer);

				printf("Shifted child: %lld\n",offset);
				fflush(stdout);
			}

			/* Offset has gone above limit */
			if (offset >= FILE_LEN)
				break;

			if (write(fd2, buf, strlen(buf)) < 0)
			{
				error("Error writing child process A!");
			}

			/* Tell the other process about the new offest location
			 * Do this once every 3 writes, to stop excessive updates
			 */
			offset += message_length;
			if (i%3 == 0)
			{
				FILE * stream2 = fdopen(pipe_p1_writer[1], "w");
				fprintf(stream2,"%lld\n", offset);
				fflush(stream2);
				printf("Updated parent%lld\n", offset);
			}

			// /* Unlock from (SEEK_CUR - strlen(buf)) to SEEK_CUR*/
			// int negative_offset = strlen(buf) * -1;
			// f1.l_start = negative_offset;
			// f1.l_type = F_UNLCK;

			// if (fcntl(fd2, F_SETLK, &f1) < 0)
			// {
			// 	error("Error unlocking file child!");
			// }
			printf("Wrote once child%d\n",i);
			fflush(stdout);
		}
	}
	else
	{
		// Close the writer part of the pipe, we are only reading from it
		close(pipe_p1_writer[1]);
		// Close the reader part of the pipe, we are only writing to it
		close(pipe_p2_writer[0]);

		FILE * stream = fdopen(pipe_p1_writer[0], "r");

		struct pollfd fds[] =
		{
			{ pipe_p1_writer[0], POLLIN}
		};

		off_t offset = 0;

		// Similar code and logic, but for process B
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
				// Shift the offset and try to write to the next free location
				if (offset >= FILE_LEN)
					break;

				// Wait to get the offset from the other process
				poll(fds,1,500);
				char * offset_buffer = malloc(10);

				// We timed out on poll- other process has finished
				if (fds[0].revents != POLLIN)
					break;

				size_t linecapp = 10;
				if (getline(&offset_buffer,&linecapp, stream) < 0)
					error("getline error");

				// Shift the offset and try to write to the next free location
				offset = strtol(offset_buffer, NULL, 10);
				// offset += (lseek(fd, 0, SEEK_CUR) + message_length);
				(void)lseek(fd, SEEK_SET, offset);

				free(offset_buffer);
				printf("Shifted parent:%lld\n",offset);
				fflush(stdout);
			}

			if (offset >= FILE_LEN)
				break;

			if (write(fd, buf, strlen(buf)) < 0)
			{
				error("Error writing parent process B!");
			}

			/* Tell the other process about the new offest location
			 * Do this once every 3 writes, to stop excessive updates
			 */
			offset += message_length;
			if (i%3 == 0)
			{
				FILE * stream2 = fdopen(pipe_p2_writer[1], "w");
				fprintf(stream2,"%lld\n", offset);
				fflush(stream2);
				printf("Updated child%lld\n", offset);
			}


			// /* Unlock from (SEEK_CUR - strlen(buf)) to SEEK_CUR*/
			// int negative_offset = strlen(buf) * -1;
			// f1.l_start = negative_offset;
			// f1.l_type = F_UNLCK;
			// if (fcntl(fd, F_SETLK, &f1) < 0)
			// {
			// 	error("Error unlocking file parent!");
			// }
			printf("Wrote once parent%d\n", i);
			fflush(stdout);
		}

		//Wait for child to finish so parent's locks persist
		int status;
		wait(&status);
	}
}