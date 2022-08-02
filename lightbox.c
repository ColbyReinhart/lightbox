// A simple server for lightbox communication
// See readme.MD for communication standards
// Author: Colby Reinhart
// 7-18-2022

#include "lightbox.h"

#define errorQuit(msg) { perror(msg); exit(1); }

int lightbox_socket;
char* red = "255";
char* green = "0";
char* blue = "255";

// Send a message to the box to keep the connection alive
void pingBox()
{
	if (lightbox_socket == -1)
	{
		return;
	}
	
	char response[100] = {0};
	sprintf(response, "%s %s %s\n", red, green, blue);
	if (write(lightbox_socket, response, strlen(response)) == -1)
	{
		perror("Couldn't write to lightbox");
		exit(1);
	}
}

int main()
{
	lightbox_socket = -1;

	// Prepare server socket
	const int listen_socket = make_server_socket(WEB_PORT);
	if (listen_socket == -1) errorQuit("Couldn't make server socket");

	// Set a timer to occasionally ping the box
	struct itimerval new_timeset;

	// It will go off every PING_INTERVAL seconds
	new_timeset.it_interval.tv_sec = PING_INTERVAL;
	new_timeset.it_interval.tv_usec = 0;

	// Starting in PING_INTERVAL seconds
	new_timeset.it_value.tv_sec = PING_INTERVAL;
	new_timeset.it_value.tv_usec = 0;

	if (setitimer(ITIMER_REAL, &new_timeset, NULL) == -1)
	{
		perror("Couldn't set ping timer");
		return 1;
	}

	// Ping the box when the timer goes off
	signal(SIGALRM, pingBox);

	// Accept incoming calls (no multithreading for now)
	while(1)
	{
		// Get a client connection
		const int client_connection = accept(listen_socket, NULL, NULL);
		if (client_connection == -1) errorQuit("Couldn't accept a socket connection");

		// Read the request
		char request[MAX_REQ_LENGTH];
		bzero(request, MAX_REQ_LENGTH);
		if (read(client_connection, request, MAX_REQ_LENGTH) == -1)
		{
			errorQuit("Couldn't read client request");
		}

		// Split the request into the head and body
		char* end_of_header = strstr(request, "\r\n\r\n");
		*end_of_header = '\0';
		char* request_header = request;
		char* request_body = end_of_header + 4;

		printf("%s\n-----\n%s\n", request_header, request_body);
		
		// Get what we care about from the header (this will improve later)
		const char* request_type = strtok(request_header, " ");
		const char* request_route = strtok(NULL, " ");

		printf("Type:\t%s\n", request_type);
		printf("Route:\t%s\n", request_route);

		// If this is a CONNECT call, save the socket
		if (strcmp(request_type, "CONNECT") == 0)
		{
			if (lightbox_socket != -1)
			{
				close(lightbox_socket);
			}
			lightbox_socket = client_connection;
			printf("Connected! Fd: %i\n", lightbox_socket);///

			continue;
		}

		// If this is an OPTIONS call, send back a 200
		if (strcmp(request_type, "OPTIONS") == 0)
		{
			send_empty_response(client_connection, http_200);
			continue;
		}

		// Otherwise, handle the request. We'll worry about the request type later
		// Can't do anything if the esp hasn't connected
		if (lightbox_socket == -1)
		{
			send_empty_response(client_connection, http_404);
			continue;
		}

		// Get the data
		red = strtok(request_body, " ");
		green = strtok(NULL, " ");
		blue = strtok(NULL, "\r\n");

		// Write the data to the socket
		char response[100] = {0};
		sprintf(response, "%s %s %s\n", red, green, blue);
		if (write(lightbox_socket, response, strlen(response)) > -1)
		{
			send_empty_response(client_connection, http_200);
		}
		else
		{
			perror("Couldn't write to lightbox");
			send_empty_response(client_connection, http_500);
		}
	}
}
