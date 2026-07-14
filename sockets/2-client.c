#include "socket.h"

/**
 * parse_arguments - program that parses the command-line arguments to extract
 * the host and port number
 * if the number of arguments is incorrect, the function prints the correct
 * usage format and exits the program
 * @argc: the number of command-line arguments
 * @argv: the array of command-line arguments
 * @host: a pointer to a string where the host name will be stored
 * @port: a pointer to an integer where the port number will be stored
 * Return: nothing (void)
 */

void parse_arguments(int argc, char *argv[], char **host, int *port)
{
	if (argc != 3)
	{
		fprintf(stderr, "Usage: %s <host> <port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	*host = argv[1];
	*port = atoi(argv[2]);
}

/**
 * create_socket - program that creates a socket using the IPv4 address family
 * and a stream socket type for TCP communication
 * if the socket cannot be created, the function prints an error message and
 * exits the program
 * Return: the file descriptor for the newly created socket
 */

int create_socket(void)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0)
	{
		perror("Cannot create socket");
		exit(EXIT_FAILURE);
	}
	return (sockfd);
}

/**
 * connect_to_server - program that establishes a connection to a server
 * using the provided socket, host name, and port number
 * this function resolves the host name to an IP address and sets up the
 * server address structure for the connection attempt;
 * if connection to the server fails, it prints an error message and
 * exits the program
 * @sockfd: the file descriptor of the socket to use for the connection
 * @host: the name of the host to connect to
 * @port: the port number on which to connect to the server
 * Return: nothing (void)
 */

void connect_to_server(int sockfd, const char *host, int port)
{
	struct sockaddr_in server_addr;
	struct hostent *server;

	server = gethostbyname(host);

	if (server == NULL)
	{
		fprintf(stderr, "ERROR, no such host\n");
		exit(EXIT_FAILURE);
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);
	server_addr.sin_port = htons(port);

	if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		perror("Cannot connect to the server");
		exit(EXIT_FAILURE);
	}
	printf("Connected to %s:%d\n", host, port);
}

/**
 * main - the entry point
 * this program parses command-line arguments to extract the host name and
 * port number, creates a socket, connects to the server at the specified host
 * and port, and then closes the socket;
 * this function orchestrates the setup and tear-down process for the
 * client-side of a TCP connection
 * @argc: the number of command-line arguments
 * @argv: the array of command-line arguments
 * Return: 0 (success)
 */

int main(int argc, char *argv[])
{
	char *host;
	int port;
	int sockfd;

	parse_arguments(argc, argv, &host, &port);
	sockfd = create_socket();
	connect_to_server(sockfd, host, port);

	close(sockfd);
	return (0);
}
