#include "response.h"
#include "lightbox.h"

const struct response_entry http_200 = {200, "OK"};
const struct response_entry http_400 = {400, "Bad Request"};
const struct response_entry http_404 = {404, "Not Found"};
const struct response_entry http_500 = {500, "Internal Server Error"};

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
		close(sock_fd);
		return -1;
	}

	close(sock_fd);
	return 0;
}