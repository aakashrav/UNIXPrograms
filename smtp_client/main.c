#include "smtp_client.h"
#include "walk_directory.h"

// #define DEBUG

#ifdef DEBUG
#define SERVER "127.0.0.1"
#define PORT "7080"
#endif

#define MAX_DIRECTORY_NUMBER 10

char ** DIRECTORY_QUEUE = NULL;
int CURRENT_NUM_DIRECTORIES =0;
int current_processed_directory =0;

void
init(char * email_directory)
{
	DIRECTORY_QUEUE = calloc(MAX_DIRECTORY_NUMBER,sizeof(char *));
	DIRECTORY_QUEUE[0] = email_directory;

	for (int i=1; i < MAX_DIRECTORY_NUMBER; i++)
	{
		DIRECTORY_QUEUE[i] = calloc(256,1);
	}
}

void
destroy()
{
	for (int i=0; i < MAX_DIRECTORY_NUMBER; i++)
	{
		free(DIRECTORY_QUEUE[i]);
	}

	free(DIRECTORY_QUEUE);
}

int
do_directory(const char * directory_path)
{
	if (CURRENT_NUM_DIRECTORIES < MAX_DIRECTORY_NUMBER)
	{
		#ifdef DEBUG
		printf("Adding directory to queue: %s\n", directory_path);
		fflush(stdout);
		#endif

		strcat(DIRECTORY_QUEUE[CURRENT_NUM_DIRECTORIES],directory_path);
		CURRENT_NUM_DIRECTORIES++;
	}
	else
	{
		DIRECTORY_QUEUE = realloc(DIRECTORY_QUEUE, CURRENT_NUM_DIRECTORIES
			+ MAX_DIRECTORY_NUMBER);

		#ifdef DEBUG
		printf("Adding directory to queue: %s\n", directory_path);
		fflush(stdout);
		#endif

		strcat(DIRECTORY_QUEUE[CURRENT_NUM_DIRECTORIES], directory_path);
		CURRENT_NUM_DIRECTORIES++;
	}

	return 0;
}

int
do_file(const char * file_name, const char * file_path)
{
	int fd;
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
	{
		error("Error initializing socket");
	}

	FILE * stream = fopen(file_path, "r");
	if (!stream)
	{
		printf("Could not open email file: %s\n", file_path);
		// Continue on with next files
		return 0;
	}

	char * line_buf = calloc(4096,1);
	char * message = calloc(4096,1);
	size_t line_size = 4096;

	ssize_t num_chars = 0;
	ssize_t num_read = 0;
	while ( (num_read = getline(&line_buf, &line_size, stream)) != -1 )
	{
		num_chars+= num_read;
		if (num_read > line_size)
			line_size += (num_read - line_size);

		if (num_chars >= strlen(message))
		{
			message=realloc(message, num_chars);
		}

		strcat(message, line_buf);
	}

	char * formatted_message = return_smtp_formatted_message(message,"ARAVI",
		"aakash_ravi@hotmail.com",file_name,"New Message!");


	char * host_name = calloc(NI_MAXHOST, 1);
	char * service_name = calloc(NI_MAXSERV, 1);

	/* The directory name contains the IP address and port of the mailserver
	 * together, so we separate them.
	 */
	separate_ip_and_port(host_name, service_name, 
		DIRECTORY_QUEUE[current_processed_directory]);

	#ifdef DEBUG
	set_smtp_destination_and_send(SERVER, PORT, formatted_message, fd);
	#endif

	#ifndef DEBUG
	set_smtp_destination_and_send(host_name, service_name, formatted_message, fd);
	#endif

	printf("Done with sending message\n");
	fflush(stdout);

	fclose(stream);
	close(fd);
	free(formatted_message);
	free(line_buf);
	free(host_name);
	free(message);
	free(service_name);

	return 0;
}


int
main(int argc, char * argv[])
{
	if (argc < 2)
	{
		printf("Usage ./email-sender <mail_directory>\n");
		exit(1);
	}

	const char * cwd = getenv("PWD");
	char * email_directory = malloc(strlen(cwd) +  strlen(argv[1]) +1);
	strcat(email_directory, cwd);
	strcat(email_directory, "/");
	strcat(email_directory, argv[1]);
	init(email_directory);
	CURRENT_NUM_DIRECTORIES = 1;

	current_processed_directory =0;

	while (current_processed_directory != CURRENT_NUM_DIRECTORIES)
	{
		perform_on_current_directory(DIRECTORY_QUEUE[current_processed_directory], 
			do_directory, do_file);
		current_processed_directory++;
	}

	printf("I finished sending your mails, anything else I can help with?\n");
	fflush(stdout);

	destroy();

}
