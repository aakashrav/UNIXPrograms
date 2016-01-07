#include "smtp_client.h"


void
error(const char * message)
{
	perror(message);
	printf("Errno: %d", errno);
	exit(1);
};

void
set_smtp_destination(struct sockaddr_in * server_address, const char * destination_addr,
	const long port)
{
	memset((char *) server_address, 0, sizeof(server_address));

	char * port_cstr[5];
	sprintf(port_cstr,port,"%d");
	struct addrinfo hints;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	struct addrinfo * results;

	int i = getaddrinfo(destination_addr, port_cstr, &hints, &results);

	server_address->sin_family = AF_INET;
	server_address->sin_port = htons(port);

	if (inet_aton(destination_addr, &(server_address->sin_addr)) == 0)
	{
		error("Error on network bye torder translation");
	}

	freeaddrinfo(results);
}

void
smtp_client_send_message(struct sockaddr_in * server_address, const char * message, int sock_fd)
{
	socklen_t len = (socklen_t)sizeof(*server_address);
	int i = connect(sock_fd, (struct sockaddr *)server_address, len);
	if (i == -1)
		error("Error on connecting to server address");

	char buf[strlen(message)];

	sprintf(buf, message);
	send(sock_fd, buf, strlen(message),0);
}

// HELO SKOZINA
// MAIL FROM:stanislav.kozina@gmail.com
// RCPT TO:stanislav.kozina@gmail.com
// DATA
// Subject: Test email

// body of the email
// .

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
