#include "socket.h"

#define PORT 12345


/**
 * create_socket - program that creates a TCP socket using the IPv4 protocol
 * this function attempts to create a socket and reports an error if the socket
 * cannot be created
 * Return: the socket file descriptor (sockfd) on success;
 *          if an error occurs during socket creation, the program will print
 *          an error message and terminate
 */

int create_socket(void)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0)
	{
		perror("Error creating socket");
		exit(EXIT_FAILURE);
	}
	return (sockfd);
}

/**
 * setup_address - program that initializes the provided 'sockaddr_in'
 * structure with the server's address settings
 * this function sets the address family to AF_INET (IPv4), allows
 * the socket to bind to all IP addresses available on the machine,
 * and sets the port number to a predefined value (PORT), converting
 * it from host byte order to network byte order using htons()
 * @address: a pointer to a 'sockaddr_in' structure that will be
 *           initialized with the server's address settings
 * Return: nothing (void)
 */

void setup_address(struct sockaddr_in *address)
{
	memset(address, 0, sizeof(*address));
	address->sin_family = AF_INET;
	address->sin_addr.s_addr = INADDR_ANY;
	address->sin_port = htons(PORT);
}

/**
 * bind_socket - program that binds the server socket to the specified
 * IP address and port number contained within the 'sockaddr_in' structure
 * if the socket cannot be bound (the address is in use), the function prints
 * an error message and exits the program
 * @sockfd: the socket file descriptor returned by create_socket()
 * @address: a pointer to a 'sockaddr_in' structure that contains the
 *           IP address and port number to which the socket will be bound
 * Return: nothing (void)
 */

void bind_socket(int sockfd, struct sockaddr_in *address)
{
	if (bind(sockfd, (struct sockaddr *)address, sizeof(*address)) < 0)
	{
		perror("Bind failed");
		exit(EXIT_FAILURE);
	}
}

/**
 * start_listening - program that places the server socket in a listening
 * state, where it can accept connection requests
 * this function allows the socket to queue up to 5 incoming connection
 * requests before refusing additional requests;
 * if an error occurs while attempting to listen on the socket, an error
 * message is printed and the program terminates
 * @sockfd: the socket file descriptor on which the server will listen
 *          for incoming connections
 * Return: nothing (void)
 */

void start_listening(int sockfd)
{
	if (listen(sockfd, 5) < 0)
	{
		perror("Listen failed");
		exit(EXIT_FAILURE);
	}
	printf("Server listening on port %d\n", PORT);
}

/**
 * main - the entry point
 * this program orchestrates the process of creating a socket, setting up
 * the server's address, binding the socket, and starting to listen for
 * incoming connections on port 12345;
 * after setup, the program enters an infinite loop, simulating a server
 * that runs indefinitely until it is terminated by an external signal
 * Return: always returns 0, although this program is designed to run
 *         indefinitely and only terminate upon receiving a 'kill' signal
 */

int main(void)
{
	struct sockaddr_in address;

	int sockfd = create_socket();

	setup_address(&address);
	bind_socket(sockfd, &address);
	start_listening(sockfd);

	/* Hang indefinitely until killed by a signal */
	while (1)
	{
		sleep(1);
	}

	return (0);
}
