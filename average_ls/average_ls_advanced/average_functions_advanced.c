#include "average_functions_advanced.h"

// int
// print_files_with_long_names(const char * pathname, int average)
// {
// 	// Reset current recursion level to 0
// 	CURRENT_RECURSION_COUNT =0;

// 	DIR * directory;
// 	directory = opendir(pathname);

// 	if (directory == NULL)
// 	{
// 		perror("Error fetching directory");
// 		return -1;
// 	}

// 	struct dirent * temp;

// 	while ((temp = readdir(directory)) != NULL)
// 	{
// 		// Prevent infinite recursion by disallowing special directory names
// 		if ( !(strcmp(temp->d_name,".")) || !(strcmp(temp->d_name,"..")) )
// 			continue;

// 		struct stat file;

// 		// Create a local copy of the pathname for local usage
// 		char * pathname_copy = (char *)malloc(strlen(pathname) +1 );
// 		strcpy(pathname_copy, pathname);

// 		// Create a string for the full path of the file or directory
// 		char * fullpath = malloc(strlen(pathname_copy) + strlen(temp->d_name) +1);
// 		strcat(fullpath, pathname_copy);
// 		strcat(fullpath, "/");
// 		strcat(fullpath, temp->d_name);

// 		if ( stat(fullpath, &file) < 0 )
// 		{
// 			perror("Error reading file: ");
// 			printf("File name : %s\n", fullpath);
// 			free(fullpath);
// 			free(pathname_copy);
// 			continue;
// 		}

// 		if (S_ISDIR(file.st_mode) && CURRENT_RECURSION_COUNT < MAX_NUMBER_RECURSIVE_DIRECTORIES)
// 		{
// 			print_files_with_long_names(fullpath, average);
// 			free(fullpath);
// 			free(pathname_copy);
// 			CURRENT_RECURSION_COUNT++;
// 			continue;
// 		}

// 		if (strlen(temp->d_name) > average)
// 			printf("%s\n", temp->d_name);

// 		free(fullpath);
// 		free(pathname_copy);

// 	}

// 	closedir(directory);
// 	return (0);
// }

int
recursive_search(char * pathname, const char * final_fifo_name)
{
	char * RECURSION_FIFO_PATH = "/tmp/averageLSfifoRECURSION";

	// Unlink any existing fifo
	unlink(RECURSION_FIFO_PATH);
	mkfifo(RECURSION_FIFO_PATH, 0664);

	int recursive_fd;

	if ( (recursive_fd = open(RECURSION_FIFO_PATH, O_RDONLY | O_NONBLOCK)) < 0)
		return -1;

	char * cwd = getenv("PWD");
	char * executable = "/recursive_searcher";
	char * fullpath = malloc(strlen(cwd) + strlen(executable) +1);
	strcat(fullpath, cwd);
	strcat(fullpath, executable);

	pid_t pid = fork();

	if (pid == 0)
	{

		char * arg[] = {executable, fullpath, pathname, RECURSION_FIFO_PATH, NULL};
		if (execv(fullpath, arg) < 0)
			exit(1);
		return 0;
		
	}
	else
	{
		int status;
		pid_t pid_temp;

		pid_temp = waitpid(pid, &status, 0);
		if (status == 0)
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

			while ( (read = getline(&buf, &buf_size, file_info)) != -1)
			{
				fputs(buf,output);
				fflush(output);
			}

			free(fullpath);
			free(buf);
			unlink(RECURSION_FIFO_PATH);
			return 0;	
		}
		else
		{
			free(fullpath);
			unlink(RECURSION_FIFO_PATH);
			return -1;
		}
	}

}