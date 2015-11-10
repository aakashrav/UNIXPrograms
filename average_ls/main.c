#include "average_functions.h"

int main(int argc, char * argv[])
{
	if (argc < 2)
	{
		printf("Usage averageLS <pathname>");
		return 1;
	}

	long numFiles;
	long totalFileNameLength;

	int total_info[2] = recursive_search(argv[1]);

	int average = total_info[1]/total_info[2];

	print_files_with_long_names(argv[1], average);


}