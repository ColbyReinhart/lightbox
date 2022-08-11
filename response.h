#ifndef RESPONSE_H
#define RESPONSE_H

struct response_entry
{
	int code;
	char* message;
};
const struct response_entry http_200;
const struct response_entry http_400;
const struct response_entry http_404;
const struct response_entry http_500;

int send_empty_response(int sock_fd, struct response_entry response);

#endif // RESPONSE_H