#include <stdio.h>
#include <dlfcn.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct pwq
{
	int(*some_password_check)(const char *);
} pwq_t;
