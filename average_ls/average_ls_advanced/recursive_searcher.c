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


static int MAXIMUM_RECURSION = 3;
static int CURRENT_RECURSION_COUNT;


int main(int argc, char * argv[])
{
	char * pathname = argv[1];
	char * fifo = argv[2];
	if (argc > 3)
		CURRENT_RECURSION_COUNT = atoi(argv[3]);
	else
		CURRENT_RECURSION_COUNT = 0;

	int fd = open(fifo, O_WRONLY);
	if (fd < 0)
	{
		perror("Error opening fifo for recursive write");
		exit(1);
	}
	FILE * stream = fdopen(fd, "a");
	if (!stream)
	{
		perror("Error stream for recursive write");
		exit(1);
	}
	setvbuf(stream, NULL, _IOFBF, 50);

	DIR * directory;
	directory = opendir(pathname);
	if (directory == NULL)
		{
			perror("ERROR loading directory");
			printf("Directory name: %s\n", pathname);
			return -1;
		}

	struct dirent * temp;
	
	printf("Successfully opened recursive directory, starting analysis on %s\n", pathname);
	fflush(stdout);

	while ((temp = readdir(directory)) != NULL)
	{
		struct stat file;

		// Prevent infinite recursion by disallowing special directory names
		if ( !(strcmp(temp->d_name,".")) || !(strcmp(temp->d_name,"..")) )
			continue;

		// Create a local copy of the pathname for local usage
		char * pathname_copy = (char *)malloc(strlen(pathname) +1 );
		strcpy(pathname_copy, pathname);

		// Create a string for the full path of the file/directory
		char * fullpath = malloc(strlen(pathname_copy) + strlen(temp->d_name) + 2);
		strcat(fullpath, pathname_copy);
		strcat(fullpath, "/");
		strcat(fullpath, temp->d_name);

		// printf("Fetching file %s\n", fullpath);

		if ( stat(fullpath, &file) < 0 )
		{
			perror("Error reading file: ");
			printf("File name : %s\n", fullpath);
			free(fullpath);
			free(pathname_copy);
			continue;
		}

		if (S_ISDIR(file.st_mode) && (CURRENT_RECURSION_COUNT <= MAXIMUM_RECURSION))
		{
			char * exec_path = malloc(strlen(getenv("PWD")) + strlen("/recursive_searcher") +1);
			strcat(exec_path, getenv("PWD"));
			strcat(exec_path, "/recursive_searcher");
			char int_to_str[5];
			sprintf(int_to_str, "%d", CURRENT_RECURSION_COUNT);
			char * arg[] = {"recursive_searcher", pathname, fifo, int_to_str};

			pid_t pid = fork();

			if (pid == 0)
			{
				if ( execv(exec_path, arg) < 0)
				{
					exit(1);
					printf("Error on exec recursive directories");
					fflush(stdout);
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
					free(exec_path);
					CURRENT_RECURSION_COUNT++;
					continue;
				}
				else
				{
					printf("Error on recursive directory reading\n");
					fflush(stdout);
					free(fullpath);
					free(pathname_copy);
					free(exec_path);
					continue;
				}
			}	
		}

		char buf[50];
		sprintf(buf, "%lu\n", strlen(temp->d_name));
		fputs(buf,stream);
		fflush(stream);

		free(fullpath);
		free(pathname_copy);
	}

	unlink(fifo);
	closedir(directory);
}