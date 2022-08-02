#ifndef LIGHTBOX_H
#define LIGHTBOX_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>

// General
#define QSIZE 10				// Max amount of incoming calls to queue (arbitrary)
#define MAX_REQ_LENGTH 8192		// Maximum length for a request
#define ROOT_PATH "/webserver"
#define WEB_PORT 5000
#define PING_INTERVAL 900		// Every 15 minutes (900 seconds)

#ifdef TEST
#define WEB_PORT 5050
#else
#define WEB_PORT 5000
#endif

struct response_entry
{
	int code;
	char* message;
};
const struct response_entry http_200;
const struct response_entry http_400;
const struct response_entry http_404;
const struct response_entry http_500;

int make_server_socket(int port);
int send_empty_response(int sock_fd, struct response_entry response);

#endif // LIGHTBOX_H