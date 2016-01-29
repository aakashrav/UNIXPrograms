#include "pipe_functions.h"

const char * file_pathname = "/etc/services";
const char * local_file_pathname = "services";

int
main(int argc, char * argv[])
{
	int err;
	copy_file(file_pathname, local_file_pathname);

	int pipes[2];

	err = pipe(pipes);
	if (err == -1)
		error("Error on creation of pipe");

	// First command
	char * const args[] = {"cat", "services", NULL};
	pipe_and_exec(pipes,"/bin/cat", args,0);
	// Dup the standard input as the reader side of the pipe
	err = dup2(pipes[0],0);
	if (err == -1)
		error("Error on dup");
	// Then, using that pipe as standard input, run the grep call
	// Create another pipe to hold the output of grep

	int grep_pipe[2];
	err = pipe(grep_pipe);
	if (err == -1)
		error("Error on creation of grep pipe");
	char * const args2[] = {"grep", "tcp", NULL};
	pipe_and_exec(grep_pipe, "/usr/bin/grep", args2,0);
	// Dup the standard input as the reader side of the pipe
	err = dup2(grep_pipe[0],0);
	if (err == -1)
		error("Error on dup");

	//Finally, we call wc -l
	int wc_pipe[2];
	err = pipe(wc_pipe);
	if (err == -1)
		error("Error on creation of wc pipe");
	char * const args3[] = {"wc", "-l", NULL};
	pipe_and_exec(grep_pipe, "/usr/bin/wc", args3,1);


}
