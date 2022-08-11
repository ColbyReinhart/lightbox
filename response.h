struct response_entry
{
	int code;
	char* message;
};
const struct response_entry http_200;
const struct response_entry http_400;
const struct response_entry http_404;
const struct response_entry http_500;