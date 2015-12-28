#include "password_quality.h"
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#define NUM_PLUGINS 1

// List of plugins to load (can add more later if needed)
static const char * PLUGINS[] = {"common_password_check_plugin"};

// Plugin functions
static int (*common_password_check)(const char *);

/* Array of functions that correspond to the respective plugins,
 * so we can initialize them later
 */
static int (*list_of_password_checks[1])(const char *);

static void 
dynliberror(const char * message)
{
	printf("%s",dlerror());
	exit(1);
}

static int 
is_directory(const char * path)
{
	struct stat library_file_stat;
	stat(path, &library_file_stat);
	return S_ISDIR(library_file_stat.st_mode);
}

static void
return_appropriate_plugin(pwq_t * plugin, void * dynamic_library)
{
	for (int i =0; i < NUM_PLUGINS; i++)
	{
		plugin = (pwq_t *)dlsym(dynamic_library, PLUGINS[i]);

		// Proper plugin not found yet
		if (plugin == 0)
			continue;
		else
		{
			list_of_password_checks[i] = plugin->some_password_check;
			if (!list_of_password_checks[i])
			{
				dynliberror("Error loading password checker");
			}
		}
	}
}

static void 
load_plugins()
{
	char * CWD = getenv("PWD");
	char * fullpath = calloc(strlen(CWD) + 
		+ strlen("/lib/plugins") + 1,1);
	strcat(fullpath, CWD);
	strcat(fullpath, "/lib/plugins");

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

			pwq_t plugin;
			return_appropriate_plugin(&plugin, dynamic_library);

			free(full_library_path);

		}
	}

	free(fullpath);
}

static int 
check(const char * password)
{
	//Initialize the password checks
	list_of_password_checks[0] = common_password_check;

	load_plugins();

	//Check for common passwords
	int check = list_of_password_checks[0](password);
	if (check < 0)
	{
		printf("Your chosen password is a very common password, please try again!\n");
		fflush(stdout);
		return 1;
	}

	printf("Password approved!\n");
	return 0;
}

pwq_t password_check_plugin = {check};
