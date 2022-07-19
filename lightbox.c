// A simple server for lightbox communication
// See readme.MD for communication standards
// Author: Colby Reinhart
// 7-18-2022

#include "lightbox.h"

#define errorQuit(msg) { perror(msg); exit(1); }

int lightbox_socket;

int main()
{
	lightbox_socket = -1;

	// Prepare server socket
	const int listen_socket = make_server_socket(WEB_PORT);
	if (listen_socket == -1) errorQuit("Couldn't make server socket");

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

		printf("%s-----\n%s", request_header, request_body);
		
		// Get what we care about from the header (this will improve later)
		const char* request_type = strtok(request_header, " ");
		const char* request_route = strtok(NULL, " ");

		// If this is a CONNECT call, save the socket
		if (strcmp(request_type, "CONNECT") == 0)
		{
			if (lightbox_socket != -1)
			{
				close(lightbox_socket);
				lightbox_socket = client_connection;
			}

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
		const char* red = strtok(request_body, " ");
		const char* green = strtok(NULL, " ");
		const char* blue = strtok(NULL, "\r\n");

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