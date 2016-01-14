#include "smtp_client.h"


void
error(const char * message)
{
	perror(message);
	printf("Errno: %d", errno);
	exit(1);
};

/*
 * This function takes a specific server destination address, service number (port),
 * a message, and a socket file descriptor, and sends the requested message to the
 * server
 */
void
set_smtp_destination_and_send(const char * destination_addr, const char * port,
	const char * message, int sock_fd)
{
	struct addrinfo hints;
	memset((void *)&hints, 0, sizeof(hints));
	// struct sockaddr_in * sock_ipv4 = NULL;
	// struct sockaddr_in6 * sock_ipv6 = NULL;
	int ipv4=0;
	int ipv6=0;

	struct addrinfo * result_iterator, * res_original;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	int error_val = getaddrinfo(destination_addr, port, &hints, &res_original);

	if (error_val!=0)
	{
		printf("%s\n", gai_strerror(error_val));
		error("Error on getaddrinfo");
	}

	for (result_iterator = res_original; result_iterator!=NULL; result_iterator = result_iterator->ai_next)
	{
		if ( (result_iterator->ai_family != AF_INET) && (result_iterator->ai_family != AF_INET6) )
			continue;
		else if (result_iterator->ai_family == AF_INET)
			ipv4=1;
		else
			ipv6=1;

		break;

		// if (result_iterator->ai_family == AF_INET)
		// {
		// 	sock_ipv4 = (struct sockaddr_in *)result_iterator->ai_addr;
		// }
		// if (result_iterator->ai_family == AF_INET6)
		// {
		// 	sock_ipv6 = (struct sockaddr_in6 *)result_iterator->ai_addr;
		// }

	}

	if (ipv4 == 1)
	{
		socklen_t len = (socklen_t)sizeof(struct sockaddr_in);
		int i = connect(sock_fd, result_iterator->ai_addr, len);
		if (i == -1)
			error("Error on connecting to server address");

		char buf[strlen(message)];

		sprintf(buf, message);
		send(sock_fd, buf, strlen(message),0);	
	}
	else if (ipv6 == 1)
	{
		socklen_t len = (socklen_t)sizeof(struct sockaddr_in6);
		int i = connect(sock_fd, result_iterator->ai_addr, len);
		if (i == -1)
			error("Error on connecting to server address");

		char buf[strlen(message)];

		sprintf(buf, message);
		send(sock_fd, buf, strlen(message),0);	
	}
	else
	{
		printf("%s\n", destination_addr);
		error("SMTP Server can't be found");
	}

	freeaddrinfo(res_original);
}

/*
 * Returns a message in the SMTP format
 */
char *
return_smtp_formatted_message(const char * MESSAGE, const char * SENDER_DOMAIN,
	const char * MAIL_FROM, const char * RCPT_TO, const char * SUBJECT)
{
	char * smtp_message = calloc(strlen(MESSAGE) + strlen(SENDER_DOMAIN) +
		strlen(MAIL_FROM) + strlen(RCPT_TO) + strlen(SUBJECT) + SMTP_PROTOCOL_LENGTH,1);

	sprintf(smtp_message, "HELLO %s\nMAILFROM:%s\nRCPT TO:%s\nDATA\nSubject: %s\n%s\n.", 
		SENDER_DOMAIN, MAIL_FROM, RCPT_TO, SUBJECT, MESSAGE);

	return smtp_message;
}


/*
 * Seprates file names of type u2-0.ms.mff.cuni.cz@2526 into u2-0.ms.mff.cuni.cz
 * and 2526, which are placed in the input parameters ip_addr, and port, respectively.
 */
void
separate_ip_and_port(char * ip_addr, char * port, const char * combined_addr)
{
	char * port_loc= strchr(combined_addr, '@');
	// Go back till we see a '/' character, this is the start of the server address
	char * ip_start_location = port_loc;
	while (*ip_start_location != '/')
	{
		ip_start_location--;
	}
	// Go to the starting location of ip address
	ip_start_location++;
	// Get the length of the server address by pointer arithmetic
	long len_of_address = port_loc - ip_start_location;
	// Go to the starting point of the port
	port_loc+=1;
	// Copy the port first
    strcpy(port, port_loc);
    // Then copy the server address
    strncpy(ip_addr, ip_start_location, (size_t)len_of_address);

}
