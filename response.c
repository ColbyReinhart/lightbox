#include "response.h"
#include "lightbox.h"

const struct response_entry http_200 = {200, "OK"};
const struct response_entry http_400 = {400, "Bad Request"};
const struct response_entry http_404 = {404, "Not Found"};
const struct response_entry http_500 = {500, "Internal Server Error"};

int send_response
(
	const int sock_fd,
	const struct response_entry response,
	char* body
)
{
	char response_text[MAX_REQ_LENGTH]; // Arbitrarily large
	sprintf
	(
		response_text,
		"HTTP/1.1 %d %s\r\n"
		"Access-Control-Allow-Origin: http://colbyreinhart.com\r\n"
		"Content-Length: %ld\r\n\r\n"
		"%s",
		response.code,
		response.message,
		(body != NULL ? strlen(body) : 0),
		(body != NULL ? body : "")
	);
	if (write(sock_fd, response_text, strlen(response_text)) == -1)
	{
		perror("Couldn't respond to client");
		return -1;
	}

	return 0;
} 