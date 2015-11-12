/*
 * This module provides implementations of helper functions for the ls program
 */

#include "ls.h"

/*
 * Indicate to the compiler that there is an external directory
 * size variable
 */
 extern long CURRENT_DIRECTORY_SIZE;

/*
 * get_directory_files()
 *
 * Helper function to retrieve a list of dirent structs 
 * and add them to dir_files
 */
struct dirent *
get_directory_files(const char * dir_path)
{
	size_t size = INITIAL_DIRECTORY_SIZE;
	struct dirent * dir_files = 
		malloc(sizeof(struct dirent) * INITIAL_DIRECTORY_SIZE);

	DIR * directory = opendir(dir_path);
	struct dirent * temp;

	if (directory)
	{
		int index = 0;
		while((temp = readdir(directory)) != NULL)
		{
			if (index < size)
			{
				dir_files[index] = *temp;
				CURRENT_DIRECTORY_SIZE++;
			}
			else 
			{
				dir_files = realloc(dir_files, (sizeof(dir_files[0]) * (size + REALLOC_SIZE)) );
				dir_files[index] = *temp;
				size += REALLOC_SIZE;
				// Cache the new size so that main can know what the size is
				CURRENT_DIRECTORY_SIZE = size;
			}

			index++;
		}

	}
	else
		return NULL;

	closedir(directory);
	return (dir_files);
}

/*
 * print_file_information_long(const char * dir_file)
 *
 * Prints information about the file a la 'ls -l'
 */
int
print_file_information_long(const char * dir_name, const char * dir_file)
{
	struct stat file;

	char * fullpath = malloc(strlen(dir_name) + strlen(dir_file) +1);
	strcat(fullpath, dir_name);
	strcat(fullpath, "/");
	strcat(fullpath, dir_file);

	if ( stat(fullpath, &file) < 0)
	{
		return (-1);
	}

	//Print file mode
	printf("%c", (S_ISDIR(file.st_mode) ? 'd' : '-'));
	printf("%c", (file.st_mode & S_IRUSR ? 'r' : '-'));
	printf("%c", (file.st_mode & S_IWUSR ? 'w' : '-'));
	printf("%c", (file.st_mode & S_IXUSR ? 'x' : '-'));
	printf("%c", (file.st_mode & S_IRGRP ? 'r' : '-'));
	printf("%c", (file.st_mode & S_IWGRP ? 'w' : '-'));
	printf("%c", (file.st_mode & S_IXGRP ? 'x' : '-'));
	printf("%c", (file.st_mode & S_IROTH ? 'r' : '-'));
	printf("%c", (file.st_mode & S_IWOTH ? 'w' : '-'));
	printf("%c", (file.st_mode & S_IXOTH ? 'x' : '-'));
	printf(" ");

	//Get number of hard links to this file
	printf("%5d ", file.st_nlink);

	/* 
	 * Get uid, and determine who this uid belongs to 
	 * by looking at the user database file
	 */
	uid_t uid = file.st_uid;
	struct passwd * user = getpwuid(uid);
	//Print the user
	printf("%s ", user->pw_name);
	//Get the group
	gid_t gid = user->pw_gid;
	//Get the group name
	struct group * group_info = getgrgid(gid);
	//Print the group name
	printf("%s ", group_info->gr_name);

	//Print the size in bytes
	printf("%8lld ", file.st_size);

	//Print the time of last modification
	time_t time_mod = file.st_mtime;
	/* 
	 * time_mod stores seconds since epoch, so retrieve the 
	 * corresponding local time
	 */
	struct tm * time_mod_local = localtime(&time_mod);
	//Print the modification time month
	printf("%2d ", time_mod_local->tm_mon);
	//Print the modification time day
	printf("%2d ", time_mod_local->tm_mday);
	//Print minutes and seconds
	printf("%2d:%2d ", time_mod_local->tm_min, time_mod_local->tm_sec);

	//Finally, print file path
	printf("%s\n", dir_file);
	
	return (0);


}
