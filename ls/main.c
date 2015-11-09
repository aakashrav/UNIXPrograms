#include "ls.h"

static const char * optstring = "l";

int main(int argc, char * argv[])
{
	if (argc < 2)
	{
		printf("Usage: ./ls [-l] <path_name>");
		exit(1);
	}

	char opt;
	opt = getopt(argc, argv, optstring);
	int l_flag;

	while (opt!=-1) {
		switch(opt) {
			case 'l':
				l_flag=1;
				break;
			}
		opt = getopt(argc, argv, optstring);
	}

	struct dirent * directory_files = 
		malloc(sizeof(struct dirent) * INITIAL_DIRECTORY_SIZE);
	int size = INITIAL_DIRECTORY_SIZE;

	// Get the last argument - the pathname
	char * pathname = argv[argc-1];

	int i = get_directory_files(pathname, directory_files, &size);

	if (l_flag)
	{
		for (int j=0; i < size; i++)
		{
			fflush(stdout);
			if ( print_file_information_long(directory_files[j].d_name) < 0)
			{
				perror("Error fetching file info");
			}
		}
	}
	else
	{
		for (int i=0; i < size; i++)
		{
			printf("%s\n",directory_files[i].d_name);
		}
	}

	free(directory_files);

	return (0);
}
