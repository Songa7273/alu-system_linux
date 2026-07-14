#include "socket.h"

#define PORT 12345


/**
 * initialize_address - program that initializes a 'sockaddr_in' structure
 * for IPv4 addresses
 * this function sets the family to AF_INET (IPv4), the address to INADDR_ANY
 * (accepting connections on any IP), and the port to a predefined PORT number,
 * converting it from host byte order to network byte order;
 * the 'sockaddr_in' structure will be used for the server's address settings
 * @address: a pointer to the 'sockaddr_in' structure to be initialized
 * Return: nothing (void)
 */

void initialize_address(struct sockaddr_in *address)
{
	address->sin_family = AF_INET;
	address->sin_addr.s_addr = INADDR_ANY;
	address->sin_port = htons(PORT);
}

/**
 * create_and_setup_server_socket - program that creates and sets up a server
 * socket for listening to incoming connections
 * this function encapsulates the creation of a socket, setting socket options,
 * binding it to a specified address, and setting it to listen for connections
 * @address: a pointer to a 'sockaddr_in' structure that contains the address
 * settings (port, IP) the server will use
 * Return: the file descriptor for the created server socket;
 *	   the program exits if any step fails
 *	   (socket creation, setting options, binding, or listening)
 */

int create_and_setup_server_socket(struct sockaddr_in *address)
{
	int server_fd, opt = 1;

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	if (bind(server_fd, (struct sockaddr *)address, sizeof(*address)) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	if (listen(server_fd, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}

	return (server_fd);
}

/**
 * main - the entry point
 * this program demonstrates setting up a TCP server that listens on
 * a predefined port, accepts an incoming connection, retrieves and prints
 * the client's IP address, and then closes the connection
 * Return: 0 upon successful execution,
 *	   or exits with a failure status if any critical operation fails
 *	   (accepting a connection)
 */

int main(void)
{
	struct sockaddr_in address = {0};
	int server_fd, new_socket;
	char client_ip[INET_ADDRSTRLEN];
	int addrlen = sizeof(address);

	initialize_address(&address);

	server_fd = create_and_setup_server_socket(&address);

	printf("Server listening on port %d\n", PORT);

	new_socket = accept(server_fd, (struct sockaddr *)&address,
			    (socklen_t *)&addrlen);
	if (new_socket < 0)
	{
		perror("accept");
		exit(EXIT_FAILURE);
	}

	inet_ntop(AF_INET, &(address.sin_addr), client_ip, INET_ADDRSTRLEN);
	printf("Client connected: %s\n", client_ip);

	close(new_socket);
	close(server_fd);

	return (0);
}
