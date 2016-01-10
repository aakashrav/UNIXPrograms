#include "password_quality.h"
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int (*password_check)(const char *);

extern pwq_t password_check_plugin;

int is_directory(const char * path)
{
	struct stat library_file_stat;
	stat(path, &library_file_stat);
	return S_ISDIR(library_file_stat.st_mode);
}

void 
load_plugins()
{
	password_check = password_check_plugin.some_password_check;
}

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

	load_plugins();

	password_check(password);
	return 0;

}
