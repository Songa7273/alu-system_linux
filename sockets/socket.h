#ifndef SOCKET_H
#define SOCKET_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

/* task 0 */
int create_socket(void);
void setup_address(struct sockaddr_in *address);
void bind_socket(int sockfd, struct sockaddr_in *address);
void start_listening(int sockfd);

/* task 1 */
void initialize_address(struct sockaddr_in *address);
int create_and_setup_server_socket(struct sockaddr_in *address);

/* task 2 */
void parse_arguments(int argc, char *argv[], char **host, int *port);
int create_socket(void);
void connect_to_server(int sockfd, const char *host, int port);

/* task 3 */
void initialize_server(int *server_fd);
int accept_connection(int server_fd);
void handle_client(int client_socket);

#endif /* SOCKET_H */
