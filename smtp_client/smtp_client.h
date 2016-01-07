#ifndef _SMTP_CLIENT_H
#define _SMTP_CLIENT_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netdb.h>

// Upper bound on additional length added by smtp_protocol headers
#define SMTP_PROTOCOL_LENGTH 50

void
error(const char * message);

void
set_smtp_destination(struct sockaddr_in * server_address, const char * destination_addr,
	const long port);

void
smtp_client_send_message(struct sockaddr_in * server_address, const char * message, int sock_fd);

char *
return_smtp_formatted_message(const char * MESSAGE, const char * SENDER_DOMAIN,
	const char * MAIL_FROM, const char * RCPT_TO, const char * SUBJECT);

#endif

