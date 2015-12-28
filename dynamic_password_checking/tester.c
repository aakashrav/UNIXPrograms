#include "password_quality.h"
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int (*password_check)(const char *);

void dynliberror(const char * message)
{
	printf("%s",dlerror());
	exit(1);
}

int is_directory(const char * path)
{
	struct stat library_file_stat;
	stat(path, &library_file_stat);
	return S_ISDIR(library_file_stat.st_mode);
}

void 
load_plugins()
{
	char * CWD = getenv("PWD");
	char * fullpath = calloc(strlen(CWD) + 
		+ strlen("/lib") + 1,1);
	strcat(fullpath, CWD);
	strcat(fullpath, "/lib");

	DIR * libraries;
	struct dirent *dir;
	libraries = opendir(fullpath);
	if (libraries == 0)
	{
		perror("Error loading libraries!");
		printf("Error number: %d", errno);
		fflush(stdout);
		exit(1);
	}

	while ((dir = readdir(libraries))!=NULL)
	{

		if (!is_directory(dir->d_name))
		{
			/* For some reason, "/plugins" gets detected as a file, so we handle
			 * this erroneous case
			 */
			 if (!strcmp(dir->d_name, "plugins"))
			 	continue;


			char * full_library_path = calloc(strlen(fullpath) +
			strlen(dir->d_name) + 2,1);
			strcat(full_library_path, fullpath);
			strcat(full_library_path, "/");
			strcat(full_library_path, dir->d_name);

			void * dynamic_library =  dlopen(full_library_path, RTLD_LAZY);

			if (dynamic_library == 0)
			{
				printf("%s\n", dir->d_name);
				dynliberror("Error opening password checker");
			}

			pwq_t * test_plugin = (pwq_t *)dlsym(dynamic_library, "password_check_plugin");

			password_check = test_plugin->some_password_check;
			if (!password_check)
			{
				dynliberror("Error loading password checker");
			}

			free(full_library_path);

		}
	}

	free(fullpath);
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
