#include "average_functions.h"

int
print_files_with_long_names(const char * pathname, int average)
{
	DIR * directory;
	directory = opendir(pathname);

	while ((temp = readdir(directory)) != NULL)
	{
		struct stat * file;
		if ( stat(temp.d_name, &file) < 0 )
		{
			perror("Error reading file %s", temp.d_name);
			continue;
		}

		if (S_ISDIR(file.st_mode))
		{
			int temp_info[2] = print_files_with_long_names(temp.d_name, average);
			continue;
		}

		if (sizeof(temp.d_name) > average)
			printf("%s\n", temp.d_name);

	}

	return (0);
}

int
recursive_search(const char * pathname, int * average_info)
{
	DIR * directory;
	directory = opendir(pathname);

	while ((temp = readdir(directory)) != NULL)
	{
		struct stat * file;
		if ( stat(temp.d_name, &file) < 0 )
		{
			perror("Error reading file %s", temp.d_name);
			continue;
		}

		if (S_ISDIR(file.st_mode))
		{
			int temp_info[2] = recursive_search(temp.d_name, average_info);
			continue;
		}

		average_info[1] += sizeof(temp.d_name);
		average_info[2]++;
	}

	return (0);
}