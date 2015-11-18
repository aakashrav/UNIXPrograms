/*
 * This header file provides helper function definitions for the average_ls program
 */

#ifndef _AVERAGE_H
#define		_AVERAGE_H

#include <stdio.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>


/*
 * Recursively print files with long names- names that are above the
 * average length.
 */
int
print_files_with_long_names(const char * pathname, int average);


/*
 * Recursively search the current directory and aggreagate information
 * about average length of file name
 *
 */
int
recursive_search(const char * pathname, int * average_info);

#endif /* _AVERAGE_H */

