// Lightbox utilitiy functions
// Author: Colby Reinhart
// 7-18-2022

#include "lightbox.h"

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
