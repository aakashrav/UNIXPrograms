#include "pipe_functions.h"

int
main(int argc, char * argv)
{
	int err;
	copy_file(file_pathname, local_file_pathname);

	int pipes[];

	err = pipe(pipes);
	if (err == -1)
		error("Error on creation of pipe");

	// First command
	const char * args[] = {"cat","/bin/cat", }
}