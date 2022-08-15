// A simple server for lightbox communication
// See readme.MD for communication standards
// Author: Colby Reinhart
// 7-18-2022

#include "lightbox.h"
#include "response.h"

#define errorQuit(msg) { perror(msg); exit(1); }

int handle_connect(const int sock_fd);
int handle_options(const int sock_fd);
int handle_get(const int sock_fd);
int handle_put(const int sock_fd, const char* body);
int handle_delete(const int sock_fd);

int lightbox_socket = -1;

int main()
{
	// Prepare server socket
	const int listen_socket = make_server_socket(WEB_PORT);
	if (listen_socket == -1) errorQuit("Couldn't make server socket");

	// Accept incoming calls
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
		char* const end_of_header = strstr(request, "\r\n\r\n");
		*end_of_header = '\0';
		char* const request_header = request;
		const char* const request_body = end_of_header + 4;

		// TODO: replace this with a log
		printf("%s\n-----\n%s\n", request_header, request_body);
		
		// Get what we care about from the header
		const char* const request_type = strtok(request_header, " ");
		const char* const request_route = strtok(NULL, " ");

		// TODO: replace this with a log
		printf("Type:\t%s\n", request_type);
		printf("Route:\t%s\n", request_route);

		// TODO: convert this to tabular form
		int action = CLOSE;
		if (strcmp(request_type, "CONNECT") == 0)
		{
			action = handle_connect(client_connection);
		}
		else if (strcmp(request_type, "OPTIONS") == 0)
		{
			action = handle_options(client_connection);
		}
		else if (strcmp(request_type, "GET") == 0)
		{
			action = handle_get(client_connection);
		}
		else if (strcmp(request_type, "PUT") == 0)
		{
			action = handle_put(client_connection, request_body);
		}
		else if (strcmp(request_type, "DELETE") == 0)
		{
			action = handle_delete(client_connection);
		}

		if (action == CLOSE)
		{
			close(client_connection);
		}
	}
}

// If this is an OPTIONS call:
// Send back the appropriate CORS headers
int handle_options(const int sock_fd)
{
	char response[] = "HTTP/1.1 204 No Content\r\n"
	"Access-Control-Allow-Origin: http://colbyreinhart.com\r\n"
	"Access-Control-Allow-Methods: GET, PUT, DELETE, CONNECT, OPTIONS\r\n"
	"Access-Control-Allow-Headers: Content-Type\r\n"
	"Access-Control-Max-Age: 86400\r\n\r\n"; // 24 hours

	if (write(sock_fd, response, strlen(response)) == -1)
	{
		errorQuit("Couldn't respond to preflight request");
	}

	return CLOSE;
}

// If this is a CONNECT call:
// Close the current connection (if applicable)
// and save the new connection
int handle_connect(const int sock_fd)
{
	if (lightbox_socket != -1)
	{
		close(lightbox_socket);
	}
	lightbox_socket = sock_fd;
	printf("Connected! Fd: %i\n", lightbox_socket);///

	return CONTINUE;
}

// If this is a GET call:
// Ask lightbox what color it currently is
// Give this information back to the client
int handle_get(const int sock_fd)
{
	// Check if we're connected
	if (lightbox_socket == -1)
	{
		send_response(sock_fd, http_404, NULL);
		return CLOSE;
	}

	// Ask lightbox what it's color is
	char req[] = "GET\n";
	if (write(lightbox_socket, req, strlen(req)) == -1)
	{
		perror("GET: couldn't write to lightbox");
		send_response(sock_fd, http_500, NULL);
		return CLOSE;
	}

	// Read lightbox's answer
	char res[LIGHTBOX_MSG_LEN];
	bzero(res, LIGHTBOX_MSG_LEN);
	if (read(lightbox_socket, res, LIGHTBOX_MSG_LEN) == -1)
	{
		perror("GET: couldn't read from lightbox");
		send_response(sock_fd, http_500, NULL);
		return CLOSE;
	}

	// Give lightbox's answer to the client
	send_response(sock_fd, http_200, res);
	return CLOSE;
}

// If this is a PUT call:
// Send the body to lightbox
int handle_put(const int sock_fd, const char* const body)
{
	// Check if we're connected
	if (lightbox_socket == -1)
	{
		send_response(sock_fd, http_404, NULL);
		return CLOSE;
	}

	// Give lightbox the PUT body
	char req[100];
	sprintf(req, "PUT %s\n", body);
	if (write(lightbox_socket, req, strlen(req)) == -1)
	{
		perror("PUT: couldn't write to lightbox");
		send_response(sock_fd, http_500, NULL);
		return CLOSE;
	}

	// Tell the client that all went well
	send_response(sock_fd, http_200, NULL);

	return CLOSE;
}

// If this is a DELETE call:
// Close the lightbox socket
int handle_delete(const int sock_fd)
{
	if (lightbox_socket != -1)
	{
		close(lightbox_socket);
		lightbox_socket = -1;
		send_response(sock_fd, http_200, NULL);
	}
	else
	{
		send_response(sock_fd, http_200, NULL);
	}

	return CLOSE;
}
