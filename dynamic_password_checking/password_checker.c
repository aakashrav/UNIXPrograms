#include <stdio.h>
#include <dlfcn.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

char * PLUGINS[3];

void load_plugin_names()
{
	char * CWD = getenv("PWD");

	char * fullpath = calloc(strlen(CWD) + strlen("dict_plugin.dylib") + 2,1);
	strcat(fullpath, CWD);
	strcat(fullpath, "/");
	strcat(fullpath, "dict_plugin.dylib");
	PLUGINS[0] = fullpath;

	char * fullpath_two = calloc(strlen(CWD) + strlen("length_plugin.dylib") +2, 1);
	strcat(fullpath_two, CWD);
	strcat(fullpath_two, "/");
	strcat(fullpath_two, "length_plugin.dylib");
	PLUGINS[1] = fullpath_two;

	char * fullpath_three = calloc(strlen(CWD) + strlen("username_plugin.dylib") +2, 1);
	strcat(fullpath_three, CWD);
	strcat(fullpath_three, "/");
	strcat(fullpath_three, "username_plugin.dylib");
	PLUGINS[2] = fullpath_three;
}

void free_plugin_names()
{
	for (int i =0; i < 3; i++)
	{
		free(PLUGINS[i]);
	}
}

void error(const char * message)
{
	free_plugin_names();
	perror(message);
	printf("Error number: %d\n", errno);
	exit(1);
}

void dynliberror(const char * message)
{
	free_plugin_names();
	printf("%s",dlerror());
	exit(1);
}

int main(int argc, char * argv[])
{
	if (argc < 2)
	{
		printf("Usage password_checker <password>\n");
		fflush(stdout);
		return 1;
	}

	load_plugin_names();

	char * password = argv[1];
	void * dict_plugin_handle =  dlopen(PLUGINS[0], RTLD_LAZY);

	if (dict_plugin_handle == 0)
	{
		printf("%s\n", PLUGINS[0]);
		dynliberror("Error opening common password checker");
	}

	int (*check_common_passwords)(int, char *[]) = 
		dlsym(dict_plugin_handle, "main");

	if (!check_common_passwords)
	{
		dynliberror("Error loading common password checker");
	}

	//Check for common passwords
	char * arguments[] = {password};
	int check = check_common_passwords(1, arguments);
	if (check < 0)
	{
		printf("Your chosen password is a very common password, please try again!\n");
		fflush(stdout);
		return 1;
	}

	int i = dlclose(dict_plugin_handle);
	if (i != 0)
	{
		dynliberror("Error closing common password checker");
	}

	printf("Password approved!\n");
	free_plugin_names();
}