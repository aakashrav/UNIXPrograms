/*
 * Extensible header file designed to provide functions to walk the directory
 * and perform certain actions on subdirectories and files
 */

#ifndef _WALK_DIRECTORY_H
#define _WALK_DIRECTORY_H

#include <dirent.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

/*
 * Extensible funciton that takes as input a directory name, and for each file encountered,
 * calls (*file_function), and for each directory encountered, calls (*directory_function)
 */
void
perform_on_current_directory(const char * dir_name, int (*directory_function)(const char *), 
	int (*file_function)(const char *, const char *));

#endif
