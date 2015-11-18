/*
 * Main logic behind the averageLSAdvanced program. First calls recursive_search to find
 * information on the average length of file names, then calls
 * print_files_with_long_names to print file names that are above the average.
 */

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

	int i = recursive_search(pathname, FIFONAME,0,0);
	if (i < 0)
	{
		perror("Error on recursive search");
		return -1;
	}

	FILE * stream = fdopen(fd, "r");
	char * buf = malloc(5);
	size_t buf_size = 5;
	size_t read;

	int count = 0;
	int average = 0;
	int temp = 0;

	while ( (read = getline(&buf, &buf_size, stream)) != -1)
	{
		temp = atoi(buf);
		average+=temp;
		count++;
	}

	average /= count;
	printf("\n\n");
	printf("Average length of file names: %D\n", average);
	printf("Starting printing of file names above average...\n");

	i = recursive_search(pathname, FIFONAME, 1, average);
	if (i < 0)
	{
		perror("Error on printing recursion");
		return -1;
	}


	free(buf);

}