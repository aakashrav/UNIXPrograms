/*
 * Main logic behind the averageLS program. First calls recursive_search to find
 * information on the average length of file names, then calls
 * print_files_with_long_names to print file names that are above the average.
 *
 */

#include "average_functions.h"

int main(int argc, char * argv[])
{
	const char * pathname;

	if (argc >= 2)
	{
		pathname = argv[1];
	}
	else
		pathname = getenv("PWD");

	int * info = malloc(2*sizeof(int));
	info[0] = 0;
	info[1] = 0;

	int i = recursive_search(pathname, info);
	if (i < 0)
	{
		return (-1);
	}

	int average = info[0]/info[1];

	printf("Starting printing of files, average length of name: %d\n", average);
	fflush(stdout);

	if ( (i = print_files_with_long_names(pathname, average)) < 0)
	{
		free(info);
		return (1);
	}

	free(info);
}