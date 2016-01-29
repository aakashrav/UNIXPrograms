#ifndef _PIPE_FUNCTIONS_H
#define _PIPE_FUNCTIONS_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>

void
error(const char * message);

void
pipe_and_exec(int pipes[], const char * command, char * const command_input[],
	int direct_output);

int
copy_file(const char * source,  const char * destination);

#endif
