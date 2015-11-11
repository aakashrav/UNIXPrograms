/*
 * This header file provides definitions of helpers for the ls program
 */

#ifndef LS_H
#define		LS_H

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>

// Some helper values
#define		TRUE 1
#define		FALSE 0
#define		REALLOC_SIZE 10
#define		INITIAL_DIRECTORY_SIZE 20

/*
 * Gets a list of files in a specified directory and returns the 
 * dirent structure for them in a clean array
 */
struct dirent *
get_directory_files(const char * dir_path);

/*
 * Simply prints the ls -l information for a given file
 */
int
print_file_information_long(const char * dir_name, const char * file);

#endif /* LS_H */
