/*
 * Header file designed to automate the process of formatting SMTP messages and sending
 * them.
 */
 
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

/*
 * This function takes a specific server destination address, service number (port),
 * a message, and a socket file descriptor, and sends the requested message to the
 * server
 */
void
set_smtp_destination_and_send(const char * destination_addr,const char * port, const char * message,
	int sock_fd);

/*
 * Returns a message in the SMTP format
 */
char *
return_smtp_formatted_message(const char * MESSAGE, const char * SENDER_DOMAIN,
	const char * MAIL_FROM, const char * RCPT_TO, const char * SUBJECT);

/*
 * Seprates file names of type u2-0.ms.mff.cuni.cz@2526 into u2-0.ms.mff.cuni.cz
 * and 2526, which are placed in the input parameters ip_addr, and port, respectively.
 */
void
separate_ip_and_port(char * ip_addr, char * port, const char * combined_addr);

#endif

