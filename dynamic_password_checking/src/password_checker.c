#include "password_quality.h"

char * PLUGINS[3];

void load_plugin_names()
{
	char * CWD = getenv("PWD");

	char * fullpath = calloc(strlen(CWD) + strlen("/lib/plugins/dict_plugin.so") + 1,1);
	strcat(fullpath, CWD);
	strcat(fullpath, "/lib/plugins/dict_plugin.so");
	PLUGINS[0] = fullpath;
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

int check(const char * password)
{
	load_plugin_names();

	void * dict_plugin_handle =  dlopen(PLUGINS[0], RTLD_LAZY);

	if (dict_plugin_handle == 0)
	{
		printf("%s\n", PLUGINS[0]);
		dynliberror("Error opening common password checker");
	}

	pwq_t * dict_plugin = (pwq_t *) dlsym(dict_plugin_handle, "dict_plugin");

	int (*check_common_passwords)(const char *) = 
		dict_plugin->some_password_check;

	if (!check_common_passwords)
	{
		dynliberror("Error loading common password checker");
	}

	//Check for common passwords
	int check = check_common_passwords(password);
	if (check < 0)
	{
		printf("Your chosen password is a very common password, please try again!\n");
		fflush(stdout);
		free_plugin_names();
		return 1;
	}

	int i = dlclose(dict_plugin_handle);
	if (i != 0)
	{
		dynliberror("Error closing common password checker");
	}

	printf("Password approved!\n");
	return 0;
}

pwq_t check_pass = {check};
