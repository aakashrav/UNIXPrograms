#include "password_quality.h"

char * TESTER_PLUGIN;

void load_plugin_names()
{
	char * CWD = getenv("PWD");
	char * fullpath = calloc(strlen(CWD) + 
		+ strlen("/lib/password_checker.so") + 1,1);
	strcat(fullpath, CWD);
	strcat(fullpath, "/lib/password_checker.so");
	TESTER_PLUGIN = fullpath;
}

void free_plugin_names()
{
	free(TESTER_PLUGIN);
}


void dynliberror(const char * message)
{
	free_plugin_names();
	printf("%s",dlerror());
	exit(1);
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

	load_plugin_names();

	void * tester_handle =  dlopen(TESTER_PLUGIN, RTLD_LAZY);

	if (tester_handle == 0)
	{
		printf("%s\n", TESTER_PLUGIN);
		dynliberror("Error opening password checker");
	}

	pwq_t * test_plugin = (pwq_t *)dlsym(tester_handle, "check_pass");

	int (*check)(const char *) = 
		test_plugin->some_password_check;

	if (!check)
	{
		dynliberror("Error loading password checker");
	}

	check(password);
	free_plugin_names();

}
