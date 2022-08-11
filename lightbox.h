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

// General
#define QSIZE 10				// Max amount of incoming calls to queue
#define MAX_REQ_LENGTH 8192		// Maximum length for a request
#define LIGHTBOX_MSG_LEN 100

#ifndef TEST
#define WEB_PORT 5000
#else
#define WEB_PORT 5050
#endif

// Handler return values
#define CLOSE 0
#define CONTINUE 1

int make_server_socket(int port);

#endif // LIGHTBOX_H