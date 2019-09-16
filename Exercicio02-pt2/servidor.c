#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define LISTENQ 10
#define MAXDATASIZE 100
#define MAXLINE 4096
#define MIN_ARG 2
#define IPV4_LEN 16

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
void CheckArguments(int argc, char **argv){
    char error[MAXLINE + 1];
    if (argc != MIN_ARG) {
        strcpy(error,"uso: ");
        strcat(error,argv[0]);
        strcat(error," <Port>");
        perror(error);
        exit(1);
    }
}

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
int Socket(int family, int type, int flags) {
    int sockfd;
    if ((sockfd = socket(family, type, flags)) < 0) {
        perror("socket");
        exit(1);
    } else {
        return sockfd;
    }
}

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
void ConfigureServSocket(struct sockaddr_in *servaddr, int port) {
    bzero(servaddr, sizeof(*servaddr));
    servaddr->sin_family = AF_INET;
    servaddr->sin_port   = htons(port);
}

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
void Bind(int listenfd, struct sockaddr_in *servaddr) {
    if (bind(listenfd, (struct sockaddr *)servaddr, sizeof(*servaddr)) == -1) {
        perror("bind");
        exit(1);
    }
}

/* ===========================================================================
 * FUNCTION: Listen
 *
 * DESCRIPTION: Puts socket on listen mode
 *
 * PARAMETERS: listenfd - file descriptor of the socket
 *
 * RETURN VALUE: none - if successful
 *
 * ===========================================================================*/
void Listen(int listenfd) {
    if (listen(listenfd, LISTENQ) == -1) {
        perror("listen");
        exit(1);
    }
}

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
int Accept(int listenfd, struct sockaddr_in *addr, unsigned int *addrlen){
    int connfd;
    if ((connfd = accept(listenfd, (struct sockaddr *) addr, addrlen)) == -1 ) {
        perror("accept");
        exit(1);
    } else {
        return connfd;
    }
}

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
void Close(int sockfd) {
    if (close(sockfd) == -1) {
        perror("close");
    }
}

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
void PrintClientSocketInfo(struct sockaddr_in cliaddr){
    char cli_IP[IPV4_LEN];

    printf("======== New connection ========\n");
    inet_ntop(AF_INET, &cliaddr.sin_addr, cli_IP, sizeof(cli_IP));
    printf("Client's IP: %s\nClient's Port: %d\n",cli_IP, ntohs(cliaddr.sin_port));
    printf("================================\n");
}


/* ===========================================================================
 * FUNCTION: main
 *
 * DESCRIPTION: Creates an echo server that executes Unix commands requested
 * from a remote socket.
 *
 * PARAMETERS:
 * argc - number of arguments passed
 * argv - arguments passed
 *
 * RETURN VALUE: 0 - if successful
 *
 * ===========================================================================*/
int main (int argc, char **argv) {
    int listenfd, connfd, n;
    char send_buffer_str[MAXDATASIZE], recv_buffer_str[MAXDATASIZE];
    struct sockaddr_in servaddr, cliaddr;

    unsigned int cliaddr_len = sizeof cliaddr;

    pid_t pid;

    // Initial socket configurations
    CheckArguments(argc, argv);
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    ConfigureServSocket(&servaddr, atoi(argv[1]));
    Bind(listenfd, &servaddr);
    Listen(listenfd);

    for (;;) {
        // Accept connections
        connfd = Accept(listenfd, &cliaddr, &cliaddr_len);

        /* after the connection is extablished the process executes a fork, which
         * will be responsible for the actual execution of the echo function.
         */
        if ((pid=fork()) == 0) {

            Close(listenfd);

            // Prints the new connected client socket
            PrintClientSocketInfo(cliaddr);

            // Waits for commands to be received
            while ( (n = read(connfd, recv_buffer_str, MAXDATASIZE)) > 0) {
                recv_buffer_str[n] = 0;

                // Copy received string to sender string
                strcpy(send_buffer_str,recv_buffer_str);

                // Send the received string to client
                write(connfd, recv_buffer_str, strlen(recv_buffer_str));

                // executes the received string
                system(recv_buffer_str);
            }

            Close(connfd);
            exit(0);
        }

        Close(connfd);
    }

    return(0);


}
