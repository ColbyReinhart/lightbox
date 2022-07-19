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

		const char* request_type = strtok(request, " ");
		const char* request_route = strtok(NULL, " ");

		// If this is a CONNECT call, save the socket
		if (strcmp(request_type, "CONNECT") == 0)
		{
			if (lightbox_socket != -1)
			{
				close(lightbox_socket);
				lightbox_socket = client_connection;
			}
		}
		// Otherwise, handle the request
		else
		{
			// Can't do anything if the esp hasn't connected
			if (lightbox_socket == -1)
			{
				return send_empty_response(client_connection, http_404);
			}

			// This sucks, but I promise I'll make it better soon.
			// I really just want to get it working.
			char* token = strtok(NULL, "\r\n");
			while (strstr(token, "DATA") == NULL && token != NULL)
			{
				token = strtok(NULL, "\r\n");
			}

			// If we couldn't find the right request body stuff
			if (token == NULL)
			{
				return send_empty_response(client_connection, http_400);
			}

			// Get the data
			strtok(token, " "); // Get rid of the "DATA" part
			const char* red = strtok(NULL, " ");
			const char* green = strtok(NULL, " ");
			const char* blue = strtok(NULL, " ");

			// Write the data to the socket
			char response[100];
			sprintf(response, "%s %s %s\n", red, green, blue);
			if (write(lightbox_socket, response, strlen(response)) > -1)
			{
				return send_empty_response(client_connection, http_200);
			}
			else
			{
				perror("Couldn't write to lightbox");
				return send_empty_response(client_connection, http_500);
			}
		}
	}
}