#include "average_functions_advanced.h"

const char * FIFONAME = "/tmp/averageLSfifo";

int main(int argc, char * argv[])
{
	char * pathname;

	if (argc >= 2)
	{
		pathname = argv[1];
	}
	else
		pathname = getenv("PWD");

	mkfifo(FIFONAME, 0664);
	int fd = open(FIFONAME, O_RDONLY | O_NONBLOCK);

	int i = recursive_search(pathname, FIFONAME);
	if (i < 0)
	{
		perror("Error on recursive search");
		return -1;
	}

	FILE * stream = fdopen(fd, "r");
	char * buf = malloc(5);
	size_t buf_size = 5;
	size_t read;

	while ( (read = getline(&buf, &buf_size, stream)) != -1)
	{
		printf("%s", buf);
	}

	free(buf);


	// int average = info[0]/info[1];

	// printf("Starting printing of files, average length of name: %d\n", average);
	// fflush(stdout);

	// if ( (i = print_files_with_long_names(pathname, average)) < 0)
	// {
	// 	free(info);
	// 	return 1;
	// }
}