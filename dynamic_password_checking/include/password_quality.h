#include <stdio.h>
#include <dlfcn.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct pwq
{
	int(*some_password_check)(const char *);
	const char * error_message;
} pwq_t;

int
check(const char * password);
