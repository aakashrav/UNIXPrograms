#include "pipe_functions.h"

void
error(const char * message)
{
	perror(message);
	printf("Errno: %d\n", errno);
	fflush(stdout);
	exit(1);
}

void
pipe_and_exec(int pipes[], const char * command, char * const command_input[],
	int direct_output)
{
	pid_t pid = fork();

	if (pid == 1)
		error("Error on fork");

	if (pid == 0)
	{
		int i;
		// Close the reader
		close(pipes[0]);
		// Duplicate standard output to the pipe, if not direct outputting
		if (!direct_output)
		{
			i = dup2(pipes[1],1);
			if (i == -1)
				error("Error on dup");
		}

		// Execute shell call
		i = execve(command, command_input, NULL);
		if (i==-1)
			error("Error on execve");

		exit(0);
	}

	close(pipes[1]);
}

int
copy_file(const char * source, const char * destination)
{
	int err;
	int fd_from= open(source, O_RDONLY);
	if (fd_from == -1)
		error("Errror on opening actual file");

	// Unlink any existing copy (if it exists)
	err = unlink(destination);

	// Create destination copy
	int fd_to = open(destination, O_CREAT | O_RDWR, 0766);
	if (fd_to == -1)
		error("Error on creating local copy");

	ssize_t nread;
	char buf[4096];

	// 0 indicates EOF, -1 indicates error
	while ( (nread = read(fd_from, buf, sizeof(buf))) > 0)
    {
        char *out_ptr = buf;
        ssize_t nwritten;

        nwritten = write(fd_to, out_ptr, nread);

        if ( (nwritten == -1) || (nwritten!=nread) )
        	error("Error on writing to destination file");
    }

    // Successfully reached end of file
    if (nread == 0)
    {
      return 0;
    }
    //error
    else
    	error("Error on reading from source file");

    return 0;

}