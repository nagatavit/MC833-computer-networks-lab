/*
 * Description
 */

#ifndef SERVIDOR_H_
#define SERVIDOR_H_

/* ===========================================================================
 * FUNCTION: CheckArguments
 *
 * DESCRIPTION: Checks if the number of arguments are correct
 *
 * PARAMETERS:
 * argc - number of arguments passed
 * argv - arguments
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
 * DESCRIPTION: Configure fields of the socket (IP type and port)
 *
 * PARAMETERS:
 * servaddr - socket structure to be configured
 * port - port to be stored on socket structure
 *
 * RETURN VALUE: none
 *
 * ===========================================================================*/
void ConfigureServSocket(struct sockaddr_in *servaddr, int port);

/* ===========================================================================
 * FUNCTION: Bind
 *
 * DESCRIPTION: Binds a socket a previous configured settings
 *
 * PARAMETERS:
 * listenfd - socket to be binded
 * servaddr - structure of the socket
 *
 * RETURN VALUE: none - if successful
 *
 * ===========================================================================*/
void Bind(int listenfd, struct sockaddr_in *servaddr);

/* ===========================================================================
 * FUNCTION: Listen
 *
 * DESCRIPTION: Puts socket on listen mode
 *
 * PARAMETERS:
 * listenfd - file descriptor of the socket
 * backlog - backlog size
 *
 * RETURN VALUE: none - if successful
 *
 * ===========================================================================*/
void Listen(int listenfd, int backlog);

/* ===========================================================================
 * FUNCTION: Accept
 *
 * DESCRIPTION: Accepts the connection from a remote socket
 *
 * PARAMETERS:
 * listenfd - socket file descriptor
 * addr - remote socket requesting a connection
 * addrlen - len of the remote socket
 *
 * RETURN VALUE: connfd - new conection socket (now connected to the client)
 *
 * ===========================================================================*/
int Accept(int listenfd, struct sockaddr_in *addr, unsigned int *addrlen);

/* ===========================================================================
 * FUNCTION: Close
 *
 * DESCRIPTION: Close the socket from it's file descriptor. Prints an
 * error if something unexpected occurs
 *
 * PARAMETERS: sockfd - socket file descriptor
 *
 * RETURN VALUE: none - if successful
 *
 * ===========================================================================*/
void Close(int sockfd);

/* ===========================================================================
 * FUNCTION: PrintClientSocketInfo
 *
 * DESCRIPTION: Prints client socket information
 *
 * PARAMETERS:
 * servaddr - Socket structure of the server
 * sockfd - socket file descriptor from the local socket
 *
 * RETURN VALUE: none
 *
 * ===========================================================================*/
void PrintClientSocketInfo(struct sockaddr_in cliaddr, int connect_disconnect);

/* ===========================================================================
 * FUNCTION: ConnectionLogger
 *
 * DESCRIPTION: logs each time a client connects or disconnects
 *
 * PARAMETERS:
 * cliaddr - client socket information
 * connect_disconnect - 0 if it was a connection, 1 if it was a disconnection
 *
 * RETURN VALUE: none
 *
 * ===========================================================================*/
void ConnectionLogger(struct sockaddr_in cliaddr, int connect_disconnect);

/* ===========================================================================
 * FUNCTION: sig_child
 *
 * DESCRIPTION: handler of SIGCHILD
 *
 * RETURN VALUE: none
 *
 * ===========================================================================*/

void sig_child(int signo);



#endif
