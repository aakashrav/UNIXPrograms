#include <stdio.h>
#include <string.h>

const char * DICTIONARY_PASSWORD_FILE = "common_passwords";

int main(int argc, char * argv[])
{
	char * password = argv[0];
	FILE * stream = fopen(DICTIONARY_PASSWORD_FILE, "r");

	char * buf = NULL;
	size_t size = 0;
	size_t read;
	int common_password_check = 0;

	while ((read = getline(&buf, &size, stream)) != -1)
	{
		int final_pos = strlen(buf);
		if (buf[final_pos-1] == '\n')
			buf[final_pos-1] = '\0';

		if ((strcmp(buf, password) == 0))
		{
			common_password_check = 1;
			break;
		}
	}

	if (common_password_check)
	{
		return -1;
	}

	return 0;

}