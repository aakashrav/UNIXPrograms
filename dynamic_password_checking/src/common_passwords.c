#include "password_quality.h"

const char * COMMON_PASSWORDS[] = {"password","1234","abcd","badpassword"};
const int NUM_COMMON_PASSWORDS = 4;

static int 
check_common_password(const char * password)
{
	int common_password_check = 0;

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

pwq_t common_password_check_plugin = {check_common_password};
