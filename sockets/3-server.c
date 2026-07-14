#include "socket.h"

#define PORT 12345
#define BUFFER_SIZE 1024


/**
 * initialize_server - program that initializes the server by creating
 * a TCP socket, binding it to a specific port and setting it to listen
 * for incoming connections
 * this function sets up the server's address to accept connections
 * on any network interface;
 * if socket creation, binding, or listening fails, it prints an error
 * message and exits the program
 * @server_fd: a pointer to an integer where the file descriptor of the
 *             created socket will be stored;
 *             his descriptor is used for all subsequent operations on
 *             the socket
 * Return: nothing (void)
 */

void initialize_server(int *server_fd)
{
	struct sockaddr_in address;

	*server_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (*server_fd == -1)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	if (bind(*server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	if (listen(*server_fd, 1) < 0)
	{
		perror("listen failed");
		exit(EXIT_FAILURE);
	}
	printf("Server listening on port %d\n", PORT);
}

/**
 * accept_connection - program that accepts an incoming connection request
 * on the listening socket
 * this function retrieves the IP address of the connecting client and
 * prints it;
 * it also prints the IP address of the client that just connected
 * @server_fd: the file descriptor of the server socket that is listening
 *             for connections
 * Return: the file descriptor of the newly accepted client socket;
 *         if accepting the client fails, the function prints an error
 *         message and exits the program
 */

int accept_connection(int server_fd)
{
	struct sockaddr_in client_address;
	socklen_t client_address_len = sizeof(client_address);
	char client_ip[INET_ADDRSTRLEN];
	int new_socket;

	new_socket = accept(server_fd, (struct sockaddr *)&client_address,
		&client_address_len);

	if (new_socket < 0)
	{
		perror("accept failed");
		exit(EXIT_FAILURE);
	}

	inet_ntop(AF_INET, &client_address.sin_addr, client_ip, INET_ADDRSTRLEN);
	printf("Client connected: %s\n", client_ip);
	return (new_socket);
}

/**
 * handle_client - program that receives a message from the connected
 * client, prints the message, and then closes the connection with the client
 * this function is responsible for handling communication with the client
 * after a connection has been established;
 * if receiving a message fails, it prints an error message and exits
 * the program;
 * after printing the received message, it closes the client socket
 * @client_socket: the file descriptor of the client socket from which
 *                 the message will be received
 * Return: nothing (void)
 */

void handle_client(int client_socket)
{
	char buffer[BUFFER_SIZE];
	ssize_t message_len = recv(client_socket, buffer, BUFFER_SIZE, 0);

	if (message_len < 0)
	{
		perror("recv failed");
		exit(EXIT_FAILURE);
	}

	buffer[message_len] = '\0';
	printf("Message received: \"%s\"\n", buffer);
	close(client_socket);
}

/**
 * main - the entry point
 * the program orchestrates the initialization of the server, accepting
 * a connection from a client, handling the client's request, and then
 * cleaning up before exiting;
 * it calls 'initialize_server' to set up the listening socket, accepts
 * a connection from a client through accept_connection, handles the
 * client's message with 'handle_client', and finally closes the server
 * socket
 * Return: 0 (success)
 */

int main(void)
{
	int server_fd;
	int client_socket;

	initialize_server(&server_fd);

	client_socket = accept_connection(server_fd);

	handle_client(client_socket);

	close(server_fd);

	return (0);
}
