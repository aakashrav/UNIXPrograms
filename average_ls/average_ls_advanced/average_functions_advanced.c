#include "average_functions_advanced.h"

/*
 * recursive_search(char *, const char *, int, int)
 *
 * This function is the intial point for the recursive logic for
 * averageLSAdvanced. It intializes the FIFO for the recursion 
 * (the local var RECURSION_FIFO_PATH), sets the PRINT_FLAG
 * (if needed), and then calls the recursive_searcher logic
 * to perform the real hardwork. When the recursive_searcher returns,
 * this function takes the data from the searcher and gives it back
 * to the caller in a separate fifo specified by final_fifo_name.
 * This allows the separation of recurser logic with the caller's logic.
 */
int
recursive_search(char * pathname, const char * final_fifo_name, 
	int print_flag, int average_filename_length)
{
	char * RECURSION_FIFO_PATH = "/tmp/averageLSfifoRECURSION";

	// Unlink any existing fifo
	unlink(RECURSION_FIFO_PATH);
	mkfifo(RECURSION_FIFO_PATH, 0664);

	int recursive_fd;

	if ( (recursive_fd = open(RECURSION_FIFO_PATH, O_RDONLY | O_NONBLOCK)) < 0)
	{
		perror("Error opening fifo for final recursive output");
		fflush(stdout);
		return -1;
	}

	char * cwd = getenv("PWD");
	char * executable = "/recursive_searcher";
	char * fullpath = calloc((strlen(cwd) + strlen(executable) +1),1);

	strcat(fullpath, cwd);
	strcat(fullpath, executable);

	char temp_average_buf[5];
	sprintf(temp_average_buf, "%d", average_filename_length);

	pid_t pid = fork();

	if (pid == 0)
	{
		char * arg[8];
		arg[0] = executable;
		arg[1] = fullpath;
		arg[2] = pathname;
		arg[3] = RECURSION_FIFO_PATH;
		arg[4] = "0";
		arg[6] = temp_average_buf;
		arg[7] = NULL;
		if (print_flag)
			arg[5] = "1";
		else
			arg[5] = "0";

		if (execv(fullpath, arg) < 0)
		{
			perror("Error execing initial recursion :");
			printf("\nexecpath: %s%s\n%s \n", cwd, executable, fullpath);
			free(fullpath);
			fflush(stdout);
			exit(1);
		}
		
	}
	else
	{
		int status;
		pid_t pid_temp;

		pid_temp = waitpid(pid, &status, 0);
		if (status == 0)
		{
			if (!print_flag)
			{
				// First create a stream for the recursive file info
				FILE * file_info = fdopen(recursive_fd, "r");

				// Create a buffer for reading lines from the file
				char * buf = malloc(5);
				size_t buf_size = 5;
				size_t read;

				// Then create a stream for the file we will be outputting to
				int output_fd = open(final_fifo_name, O_WRONLY);
				FILE * output = fdopen(output_fd, "a");

				/* 
				 * Output the information from the recuser's logic to the 
				 * caller's logic.
				 */
				while ( (read = getline(&buf, &buf_size, file_info)) != -1)
				{
					fputs(buf,output);
					fflush(output);
				}
				free(buf);
			}

			free(fullpath);
			unlink(RECURSION_FIFO_PATH);
			return 0;	
		}
		else
		{
			free(fullpath);
			unlink(RECURSION_FIFO_PATH);
			perror("Recursive function failed");
			fflush(stdout);
			return -1;
		}
	}

	//Final unlink just to be safe
	unlink(RECURSION_FIFO_PATH);
	return 0;
}