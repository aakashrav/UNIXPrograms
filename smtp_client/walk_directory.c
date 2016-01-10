#include "walk_directory.h"
#include "smtp_client.h"

/*
 * Extensible funciton that takes as input a directory name, and for each file encountered,
 * calls (*file_function_t), and for each directory encountered, calls (*directory_function_t)
 */
void
perform_on_current_directory(const char * dir_name, int (*directory_function_t)(const char *), 
	int (*file_function_t)(const char *, const char *))
{
	DIR * d;

	d = opendir(dir_name);

	if (d == 0)
	{
		printf("%s: \n", dir_name);
		error("Error opening directory");
	}

	struct dirent * cur_dir_entry;
	cur_dir_entry = readdir(d);

	while (cur_dir_entry != NULL)
	{

		// Ignore control directories
		if (!(strcmp(cur_dir_entry->d_name,".")) || !(strcmp(cur_dir_entry->d_name,"..")))
		{
			cur_dir_entry = readdir(d);
			continue;
		}

		char * name = calloc(strlen(dir_name) + strlen(cur_dir_entry->d_name) +1
			,1);

		// Clear the contents just to be safe
		memset(name, '\0', strlen(dir_name) + strlen(cur_dir_entry->d_name)+1);
		strcat(name, dir_name);
		strcat(name, "/");
		strcat(name, cur_dir_entry->d_name);


		printf("%s\n",name);
		fflush(stdout);

		struct stat file;
		int i = stat(name, &file);
		if (i != 0)
		{
			printf("%s : ", name);
			free(name);
			error("Error on stat");
		}

		if ( S_ISDIR(file.st_mode) )
		{
			directory_function_t(name);
		}
		else
		{
			printf("File function for: %s\n", name);
			fflush(stdout);
			file_function_t(cur_dir_entry->d_name, name);
		}

		free(name);
		cur_dir_entry = readdir(d);
	}
	closedir(d);
}