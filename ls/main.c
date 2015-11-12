#include "ls.h"

static const char * optstring = "l";

long CURRENT_DIRECTORY_SIZE=0;

int main(int argc, char * argv[])
{
	char opt;
	opt = getopt(argc, argv, optstring);
	int l_flag;

	while (opt!=-1) 
	{
		switch(opt) {
			case 'l':
				l_flag=1;
				break;
			}
		opt = getopt(argc, argv, optstring);
	} 

	const char * pathname;
	// Get the last argument - the pathname
	if (argc >= 3)
		pathname = argv[argc-1];
	else if ( (argc == 2) & l_flag )
		pathname = getenv("PWD");
	else if (argc == 1)
		pathname = getenv("PWD");
	else
		pathname = argv[argc-1];

	struct dirent * directory_files = 
		get_directory_files(pathname);

	if (!directory_files)
	{
		perror("Error opening directory");
		return -1;
	}

	if (l_flag)
	{
		for (int j=0; j< CURRENT_DIRECTORY_SIZE; j++)
		{
			fflush(stdout);
			if ( print_file_information_long(pathname, directory_files[j].d_name) < 0)
			{
				perror("Error reading file: ");
				printf("File name: %s%c%s\n", pathname, '/', directory_files[j].d_name);
				continue;
			}
		}
	}
	else
	{
		for (int j=0; j < CURRENT_DIRECTORY_SIZE; j++)
		{
			printf("%s\n",directory_files[j].d_name);
		}
	}

	free(directory_files);

	return (0);
}
