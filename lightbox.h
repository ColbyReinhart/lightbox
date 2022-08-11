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

#include "response.h"

// General
#define QSIZE 10				// Max amount of incoming calls to queue
#define MAX_REQ_LENGTH 8192		// Maximum length for a request

#ifndef TEST
#define WEB_PORT 5000
#else
#define WEB_PORT 5050
#endif

int make_server_socket(int port);
int send_empty_response(int sock_fd, struct response_entry response);

#endif // LIGHTBOX_H