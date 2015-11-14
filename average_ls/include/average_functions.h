/*
 * This header file provides helper function definitions for the average_ls program
 */

#ifndef AVERAGE
#define	AVERAGE

#include <stdio.h>
#include <sys/types.h>
#include <stat.h>
#include <dirent.h>
#include <stdlib.h>

int
print_files_with_long_names(const char * pathname, int average);

int
recursive_search(const char * pathname);

