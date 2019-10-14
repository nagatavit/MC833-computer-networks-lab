/*
 * Description
 */

#ifndef CLIENTE_H_
#define CLIENTE_H_

/* ===========================================================================
 * FUNCTION: CheckArguments
 *
 * DESCRIPTION:
 * Checks if the client has passed the correct number of arguments (IP and Port)
 *
 * PARAMETERS:
 * argc - number of arguments passed on command line
 * argv - string of arguments passed
 *
 * RETURN VALUE: none
 *
 * ===========================================================================*/

void CheckArguments(int argc, char **argv);

/* ===========================================================================
 * FUNCTION: Socket
 *
 * DESCRIPTION: Requests the creation of a socket and return it's fd
 *
 * PARAMETERS:
 * family - family of IP (IPv4 or IPV6)
 * type - type of socket: TCP or UDP
 * flags - protocol to be used
 *
 * RETURN VALUE: socket file descriptor - if successful
 *
 * ===========================================================================*/

int Socket(int family, int type, int flags);

/* ===========================================================================
 * FUNCTION: ConfigureServSocket
 *
 * DESCRIPTION: Configure fields of the socket (IP type, IP to connect, and port)
 *
 * PARAMETERS:
 * servaddr - socket structure to be configured
 * ip - ip of the server to connect
 * port - port to be stored on socket structure
 *
 * RETURN VALUE: none
 *
 * ===========================================================================*/

void ConfigureServSocket(struct sockaddr_in *servaddr, char *ip, int port);

/* ===========================================================================
 * FUNCTION: Connect
 *
 * DESCRIPTION: Request a connection to a remote socket
 *
 * PARAMETERS:
 * sockfd - local socket file descriptor to connect
 * servaddr - remote socket to request a connection
 *
 * RETURN VALUE: none
 *
 * ===========================================================================*/

void Connect(int sockfd, struct sockaddr_in *servaddr);

/* ===========================================================================
 * FUNCTION: Read
 *
 * DESCRIPTION: Reads data from socket
 *
 * PARAMETERS:
 * sockfd - socket fd to be read
 * recv_buffer_str - buffer to store the data
 *
 * RETURN VALUE: none
 *
 * ===========================================================================*/

int Read(int sockfd, char *recv_buffer_str);

/* ===========================================================================
 * FUNCTION: Write
 *
 * DESCRIPTION: Write data to a socket
 *
 * PARAMETERS:
 * sockfd - socket fd to be read
 * send_buffer_str - buffer with data to be sent
 *
 * RETURN VALUE: none
 *
 * ===========================================================================*/

void Write(int sockfd, char *send_buffer_str);

/* ===========================================================================
 * FUNCTION: PrintSocketInfo
 *
 * DESCRIPTION: Prints local and remote sockets info
 *
 * PARAMETERS:
 * servaddr - Socket structure of the server
 * sockfd - socket file descriptor from the local socket
 *
 * RETURN VALUE: none
 *
 * ===========================================================================*/

void PrintSocketInfo(struct sockaddr_in servaddr, int sockfd);

#endif
