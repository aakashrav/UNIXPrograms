#include "smtp_client.h"
#include "walk_directory.h"

#define SERVER "127.0.0.1"
#define PORT 7080
#define MAX_DIRECTORY_NUMBER 10

char * DIRECTORY_QUEUE[MAX_DIRECTORY_NUMBER];
int CURRENT_NUM_DIRECTORIES =0;
int added_something =1;

void
init(char * email_directory)
{
	DIRECTORY_QUEUE[0] = email_directory;

	for (int i=1; i < MAX_DIRECTORY_NUMBER; i++)
	{
		DIRECTORY_QUEUE[i] = calloc(257,1);
	}
}

void
destroy()
{
	for (int i=1; i < MAX_DIRECTORY_NUMBER; i++)
	{
		free(DIRECTORY_QUEUE[i]);
	}
}

void
do_directory(const char * directory_path)
{
	if (CURRENT_NUM_DIRECTORIES < MAX_DIRECTORY_NUMBER)
	{
		printf("Adding directory: %s", directory_path);
		fflush(stdout);

		strcat(DIRECTORY_QUEUE[CURRENT_NUM_DIRECTORIES],directory_path);
		added_something =1;
		CURRENT_NUM_DIRECTORIES++;
	}
}

void
do_file(const char * file_name, const char * file_path)
{
	int fd;
	fd = socket(AF_INET, SOCK_STREAM,0);
	if (fd == -1)
	{
		error("Error initializing socket");
	}

	struct sockaddr_in server_addr;
	set_smtp_destination(&server_addr, SERVER, PORT);

	printf("Done with smtp destination\n");
	fflush(stdout);

	FILE * stream = fopen(file_path, "r");
	char buf[50];
	char message[300];
	int num_chars = 0;
	while (fgets(buf, 50, stream) != NULL)
	{
		if (num_chars > 300)
			break;
		sprintf(message, "%s", buf);
		num_chars+=50;
	}

	char * formatted_message = return_smtp_formatted_message(message,"ARAVI",
		"aakash_ravi@hotmail.com",file_name,"New Message!");

	smtp_client_send_message(&server_addr, formatted_message, fd);

	printf("Done with sending message\n");
	fflush(stdout);

	free(formatted_message);
}


int
main(int argc, char * argv[])
{
	const char * cwd = getenv("PWD");
	char * email_directory = malloc(strlen(cwd) +  strlen("/email_directory"));
	strcat(email_directory, cwd);
	strcat(email_directory, "/email_directory");
	init(email_directory);
	CURRENT_NUM_DIRECTORIES = 1;

	int current_processed_directory =0;

	while (current_processed_directory != CURRENT_NUM_DIRECTORIES)
	{
		perform_on_current_directory(DIRECTORY_QUEUE[current_processed_directory], 
			do_directory, do_file);
		current_processed_directory++;
	}

	destroy();
	free(email_directory);

}
