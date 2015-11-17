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


	free(buf);

}