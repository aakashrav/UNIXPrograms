#include "password_quality.h"

// const char * DICTIONARY_PASSWORD_FILE = "common_passwords";

const char * COMMON_PASSWORDS[] = {"password","1234","abcd","badpassword"};
const int NUM_COMMON_PASSWORDS = 4;

int check_common_password(const char * password)
{
	// FILE * stream = fopen(DICTIONARY_PASSWORD_FILE, "r");

	// char * buf = NULL;
	// size_t size = 0;
	// size_t read;
	int common_password_check = 0;

	// while ((read = getline(&buf, &size, stream)) != -1)
	// {
	// 	int final_pos = strlen(buf);
	// 	if (buf[final_pos-1] == '\n')
	// 		buf[final_pos-1] = '\0';

	// 	if ((strcmp(buf, password) == 0))
	// 	{
	// 		common_password_check = 1;
	// 		break;
	// 	}
	// }

	for (int i =0; i < NUM_COMMON_PASSWORDS; i++)
	{
		if (strcmp(COMMON_PASSWORDS[i],password) == 0)
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

pwq_t dict_plugin = {check_common_password};