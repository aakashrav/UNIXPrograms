#include "password_quality.h"
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


// static int current_number_of_plugins = 0;
// static int plugin_buffer_length = 0;

// List of plugins to load (can add more later if needed)
// static const char * PLUGINS[] = {"common_password_check_plugin"};

// // Plugin functions
// static int (*common_password_check)(const char *);

/* Array of functions that correspond to the respective plugins,
 * so we can initialize them later
 */
// static int (**list_of_password_checks)(const char *);

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

static int
perform_appropriate_plugin(pwq_t * plugin, void * dynamic_library,
	const char * password)
{	
	plugin = (pwq_t *)dlsym(dynamic_library, "password_check");

	if (plugin == NULL)
	{
		dynliberror("Error loading password structure");
	}

	if ((plugin->some_password_check(password)) != 0)
	{
		const char * current_error_message = plugin->error_message;
		printf("%s\n",current_error_message);
		return 1;
	}

	return 0;
}

static void 
perform_plugins(const char * password)
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

		if ( (!strcmp(dir->d_name,".")) ||  (!strcmp(dir->d_name,"..")) )
			continue;

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
			perform_appropriate_plugin(&plugin, dynamic_library, password);

			free(full_library_path);

		}
	}

	free(fullpath);
}

int 
check(const char * password)
{
	perform_plugins(password);

	return 0;
}
