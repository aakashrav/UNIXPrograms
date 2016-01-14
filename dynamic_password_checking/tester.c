#include "password_quality.h"
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int
main (int argc, char * argv[])
{
	if (argc < 2)
	{
		printf("Usage: tester <password>\n");
		fflush(stdout);
		return 1;
	}

	char * password = argv[1];

	check(password);
	return 0;
}
