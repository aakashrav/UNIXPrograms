/*
 * This file implements functions defined in average_funcitons.h. It helps provide recursive
 * directory searching and printing of file names.
 */

#include "average_functions.h"

const long MAX_NUMBER_RECURSIVE_DIRECTORIES =2;
long CURRENT_RECURSION_COUNT=0;

/*
 * print_files_with_long_names(const char * pathname, int average)
 * 
 * Print all files that have filename length above the average
 */
int
print_files_with_long_names(const char * pathname, int average)
{
	// Reset current recursion level to 0
	CURRENT_RECURSION_COUNT =0;

	DIR * directory;
	directory = opendir(pathname);

	if (directory == NULL)
	{
		perror("Error fetching directory");
		return (-1);
	}

	struct dirent * temp;

	while ((temp = readdir(directory)) != NULL)
	{
		// Prevent infinite recursion by disallowing special directory names
		if ( !(strcmp(temp->d_name,".")) || !(strcmp(temp->d_name,"..")) )
			continue;

		struct stat file;

		// Create a local copy of the pathname for local usage
		char * pathname_copy = (char *)calloc(strlen(pathname) +1, 1);
		strcpy(pathname_copy, pathname);

		// Create a string for the full path of the file or directory
		char * fullpath = calloc(strlen(pathname_copy) + strlen(temp->d_name) +1,1);
		strcat(fullpath, pathname_copy);
		strcat(fullpath, "/");
		strcat(fullpath, temp->d_name);

		if ( stat(fullpath, &file) < 0 )
		{
			perror("Error reading file: ");
			printf("File name : %s\n", fullpath);
			free(fullpath);
			free(pathname_copy);
			continue;
		}

		if (S_ISDIR(file.st_mode) && CURRENT_RECURSION_COUNT < MAX_NUMBER_RECURSIVE_DIRECTORIES)
		{
			print_files_with_long_names(fullpath, average);
			free(fullpath);
			free(pathname_copy);
			CURRENT_RECURSION_COUNT++;
			continue;
		}

		if (strlen(temp->d_name) > average)
			printf("%s\n", temp->d_name);

		free(fullpath);
		free(pathname_copy);

	}

	closedir(directory);
	return (0);
}

/*
 * recursive_search(const char * pathname, int * average_info)
 *
 * Recursively search the directory specified by pathname and find the average
 * length of file names in that directory.
 */
int
recursive_search(const char * pathname, int * average_info)
{
	// Reset the current recursion level to 0
	CURRENT_RECURSION_COUNT=0;

	DIR * directory;
	directory = opendir(pathname);
	if (directory == NULL)
	{
		perror("ERROR loading directory");
		printf("Directory name: %s\n", pathname);
		return (-1);
	}

	struct dirent * temp;

	while ((temp = readdir(directory)) != NULL)
	{
		struct stat file;

		// Prevent infinite recursion by disallowing special directory names
		if ( !(strcmp(temp->d_name,".")) || !(strcmp(temp->d_name,"..")) )
			continue;

		// Create a local copy of the pathname for local usage
		char * pathname_copy = (char *)calloc(strlen(pathname) +1,1);
		strcpy(pathname_copy, pathname);

		// Create a string for the full path of the file/directory
		char * fullpath = calloc(strlen(pathname_copy) + strlen(temp->d_name) + 2,1);
		strcat(fullpath, pathname_copy);
		strcat(fullpath, "/");
		strcat(fullpath, temp->d_name);

		// printf("Fetching file %s\n", fullpath);

		if ( stat(fullpath, &file) < 0 )
		{
			perror("Error reading file: ");
			printf("File name : %s\n", fullpath);
			free(fullpath);
			free(pathname_copy);
			continue;
		}

		if (S_ISDIR(file.st_mode) && CURRENT_RECURSION_COUNT < MAX_NUMBER_RECURSIVE_DIRECTORIES)
		{
			recursive_search(fullpath, average_info);
			free(fullpath);
			free(pathname_copy);
			CURRENT_RECURSION_COUNT++;
			continue;
		}

		average_info[0] += strlen(temp->d_name);
		average_info[1]++;

		free(fullpath);
		free(pathname_copy);
	}

	closedir(directory);
	return (0);
}