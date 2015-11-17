/*
 * This header file provides helper function definitions for the average_ls program
 */

#ifndef AVERAGE_FUNCTIONS_H
#define	AVERAGE_FUNCTIONS_H

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

int
print_files_with_long_names(const char * pathname, int average);

int
recursive_search(char * pathname, const char * final_fifo_name);


#endif
