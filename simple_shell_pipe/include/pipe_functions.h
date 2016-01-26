#ifndef _PIPE_FUNCTIONS_H
#define _PIPE_FUNCTIONS_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>

const char * file_pathname = "/etc/services";
const char * local_file_pathname = "services";

void
error(const char * message);

void
pipe_and_exec(int pipes[], const char * command_input);

void
copy_file(const char * source,  const char * destination);

#endif