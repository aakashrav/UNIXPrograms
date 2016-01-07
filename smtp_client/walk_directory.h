#ifndef _WALK_DIRECTORY_H
#define _WALK_DIRECTORY_H

#include <dirent.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

void
perform_on_current_directory(const char * dir_name, void (*directory_function)(const char *), 
	void (*file_function)(const char *, const char *));

#endif
