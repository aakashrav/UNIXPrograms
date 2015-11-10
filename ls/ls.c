/*
 * This module provides implementations of helper functions for the ls program
 */

#include "ls.h"

/*
 * get_directory_files()
 *
 * Helper function to retrieve a list of dirent structs 
 * and add them to dir_files
 */
int
get_directory_files(const char * dir_path,
	struct dirent * dir_files,
	size_t * size)
{
	DIR * directory = opendir(dir_path);
	if (directory == NULL)
		return -1;
	struct dirent * temp;
	if (directory)
	{
		int index = 0;
		while((temp = readdir(directory)) != NULL)
		{
			if (index < *size)
			{
				dir_files[index] = *temp;
			}
			else 
			{
				dir_files = realloc(dir_files, (sizeof(dir_files[0]) * (*size + REALLOC_SIZE)) );
				dir_files[index] = *temp;
				*size += REALLOC_SIZE;
			}

			index++;
		}

	}
	closedir(directory);
	return (0);
}

/*
 * print_file_information_long(const char * dir_file)
 *
 * Prints information about the file a la 'ls -l'
 */
int
print_file_information_long(const char * dir_file)
{
	struct stat file;
	if ( stat(dir_file, &file) < 0)
		return -1;

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
	printf("%d ", file.st_nlink);

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
	printf("%lld ", file.st_size);

	//Print the time of last modification
	time_t time_mod = file.st_mtime;
	/* 
	 * time_mod stores seconds since epoch, so retrieve the 
	 * corresponding local time
	 */
	struct tm * time_mod_local = localtime(&time_mod);
	//Print the modification time month
	printf("%d ", time_mod_local->tm_mon);
	//Print the modification time day
	printf("%d ", time_mod_local->tm_mday);
	//Print minutes and seconds
	printf("%d:%d ", time_mod_local->tm_min, time_mod_local->tm_sec);

	//Finally, print file path
	printf("%s\n", dir_file);

	return (0);


}
