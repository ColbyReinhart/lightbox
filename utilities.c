// Lightbox utilitiy functions
// Author: Colby Reinhart
// 7-18-2022

#include "lightbox.h"

const struct response_entry http_200 = {200, "OK"};
const struct response_entry http_400 = {400, "Bad Request"};
const struct response_entry http_404 = {404, "Not Found"};
const struct response_entry http_500 = {500, "Internal Server Error"};

// Make a server socket and return it
// Input: the port number to tell the server to listen on
// Output: the file descriptor of the created socket (-1 if error)
int make_server_socket(int port)
{
	struct sockaddr_in socket_addr;						// Socket address information
	struct hostent* host_info;							// Host information

	// Ask the kernel for a socket
	int listen_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_socket == -1)
	{
		perror("Couldn't get socket");
		return -1;
	}

	// Fill in host information
	socket_addr.sin_port = htons(port);					// Fill in port
	socket_addr.sin_family = AF_INET;					// Use IPV4
	socket_addr.sin_addr.s_addr = INADDR_ANY;			// Use any available address

	// Allow the socket to be immediately reused after closing
	const int opt = 1;
	setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	// Bind address to socket
	if (bind(listen_socket, (struct sockaddr*)&socket_addr, sizeof(socket_addr)) == -1)
	{
		perror("Could not bind to socket");
		return -1;
	}

	// Allow incoming calls
	if (listen(listen_socket, QSIZE) == -1)
	{
		perror("Could not listen for calls");
		return -1;
	}

	return listen_socket;
}

// Send a response with no body (typically 400-500 codes)
// Input: the file descriptor to write to and the response type
// Returns 0 on success, -1 on error
int send_empty_response(int sock_fd, struct response_entry response)
{
	char response_text[MAX_REQ_LENGTH]; // Arbitrarily large
	sprintf
	(
		response_text,
		"HTTP/1.1 %d %s\r\n"
		"Access-Control-Allow-Origin: *\r\n"
		"Access-Control-Allow-Methods: GET, PUT, OPTIONS\r\n"
		"Access-Control-Allow-Headers: X-PINGOTHER, Content-Type\r\n"
		"Access-Control-Max-Age: 86400\r\n\r\n",
		response.code,
		response.message
	);
	if (write(sock_fd, response_text, strlen(response_text)) == -1)
	{
		perror("Couldn't respond to client");
		return -1;
	}

	return 0;
}