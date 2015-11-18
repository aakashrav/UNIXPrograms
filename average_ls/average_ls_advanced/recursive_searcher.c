/*
 * The core logic behind the averageLSProgram handled as a separate module.
 * This single thread of execution recursively parses the working directory
 * and either aggregates average information or prints out file names that are
 * above the average. Each time it encounters a directory it forks itself and repeats
 * the process.
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>


/* 
 * Set some maximum bounds for the recursion so we don't
 * end up creating a huge amount of child processes. Can be adjusted
 * as needed.
 */
static int MAXIMUM_RECURSION = 3;
static int CURRENT_RECURSION_COUNT;

// If this flag is set the program will print files rather than aggregating
// information about the filename length.
int PRINT_FLAG = 0;


int main(int argc, char * argv[])
{
	char * EXEC_PATH = argv[1];
	char * pathname = argv[2];
	char * fifo = argv[3];
	CURRENT_RECURSION_COUNT = atoi(argv[4]);
	PRINT_FLAG = atoi(argv[5]);
	int AVERAGE_FILE_LENGTH = atoi(argv[6]);

	int fd;
	FILE * stream;

	// If the print flag is not set, open the given fifo for writing
	// information about the filename lengths
	if (!PRINT_FLAG)
	{
		fd = open(fifo, O_WRONLY);
		if (fd < 0)
		{
			perror("Error opening fifo for recursive write");
			fflush(stdout);
			exit(1);
		}
		stream = fdopen(fd, "a");
		if (!stream)
		{
			perror("Error stream for recursive write");
			fflush(stdout);
			exit(1);
		}
		setvbuf(stream, NULL, _IOFBF, 50);
	}

	DIR * directory;
	directory = opendir(pathname);

	if (directory == NULL)
	{
		perror("ERROR loading directory");
		printf("Directory name: %s\n", pathname);
		fflush(stdout);
		return -1;
	}

	struct dirent * temp;
	
	printf("Successfully opened recursive directory, starting analysis on %s...\n", pathname);
	fflush(stdout);

	while ((temp = readdir(directory)) != NULL)
	{
		struct stat file;

		// Prevent infinite recursion by disallowing special directory names
		if ( !(strcmp(temp->d_name,".")) || !(strcmp(temp->d_name,"..")) )
			continue;

		// Create a local copy of the pathname for local usage
		char * pathname_copy = (char *)calloc((strlen(pathname) +1),1);
		strcpy(pathname_copy, pathname);

		// Create a string for the full path of the file/directory
		char * fullpath = calloc( (strlen(pathname_copy) + strlen(temp->d_name) + 2),1);
		strcat(fullpath, pathname_copy);
		strcat(fullpath, "/");
		strcat(fullpath, temp->d_name);

		if ( stat(fullpath, &file) < 0 )
		{
			perror("Error reading file: ");
			printf("File name : %s\n", fullpath);
			fflush(stdout);
			free(fullpath);
			fullpath = 0;
			free(pathname_copy);
			pathname_copy =0;
			continue;
		}

		// If we encounter a directory and we haven't recursed too much already,
		// we fork another child process to recurse.
		if (S_ISDIR(file.st_mode) && (CURRENT_RECURSION_COUNT <= MAXIMUM_RECURSION))
		{
			char int_to_str[5];
			sprintf(int_to_str, "%d", CURRENT_RECURSION_COUNT);

			char * arg[8];
			arg[0] = "recursive_searcher";
			arg[1] = EXEC_PATH;
			arg[2] = fullpath;
			arg[3] = fifo;
			arg[4] = int_to_str;
			arg[6] = argv[6];
			arg[7] = NULL;
			if (PRINT_FLAG)
				arg[5] = "1";
			else
				arg[5] = "0";


			pid_t pid = fork();

			if (pid == 0)
			{
				if ( execv(EXEC_PATH, arg) < 0)
				{
					perror("Error on exec recursive directories");
					fflush(stdout);
					exit(1);
				}
			}
			else
			{
				int status;
				waitpid(pid, &status, 0);
				if (status == 0)
				{
					free(fullpath);
					free(pathname_copy);
					CURRENT_RECURSION_COUNT++;
					continue;
				}
				else
				{
					printf("Error on recursive directory reading\n");
					fflush(stdout);
					free(fullpath);
					free(pathname_copy);
					continue;
				}
			}	
		}

		/* 
		 * Either print the file information to the fifo, or if 
		 * PRINT_FLAG is set, print the filename to std output
		 * if the name is longer than the average.
		 */
		char buf[50];
		sprintf(buf, "%lu\n", strlen(temp->d_name));
		if (PRINT_FLAG == 0)
		{
			fputs(buf,stream);
			fflush(stream);
		}
		else
		{
			if (strlen(temp->d_name) > AVERAGE_FILE_LENGTH)
				printf("%s\n", temp->d_name);
		}

		free(fullpath);
		free(pathname_copy);
	}

	closedir(directory);
}