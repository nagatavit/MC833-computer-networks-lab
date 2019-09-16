#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXLINE 4096
#define MAXDATASIZE 100
#define IPV4_LEN 16

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
void CheckArguments(int argc, char **argv){
    char error[MAXLINE + 1];

    if (argc != 3) {
        strcpy(error,"uso: ");
        strcat(error,argv[0]);
        strcat(error," <IPaddress>");
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
    } else
        return sockfd;
}

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
void ConfigureServSocket(struct sockaddr_in *servaddr, char *ip, int port) {
    bzero(servaddr, sizeof(*servaddr));
    servaddr->sin_family = AF_INET;
    servaddr->sin_port   = htons(port);
    if (inet_pton(AF_INET, ip, &(servaddr->sin_addr)) <= 0) {
        perror("inet_pton error");
        exit(1);
    }
}

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
void Connect(int sockfd, struct sockaddr_in *servaddr) {
    if (connect(sockfd, (struct sockaddr *) servaddr, sizeof(*servaddr)) < 0) {
        perror("connect error");
        exit(1);
    }
}

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
void Read(int sockfd, char *recv_buffer_str){
    int n;
    n = read(sockfd, recv_buffer_str, MAXDATASIZE);

    // if errors occur during the receiving package
    if (n < 0) {
        perror("read error");
        exit(1);
    }

    // put a \0 at the end of the string
    recv_buffer_str[n] = 0;
}

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
void Write(int sockfd, char *send_buffer_str){
    int n;
    n = write(sockfd, send_buffer_str, strlen(send_buffer_str));

    // if errors occur during the writting
    if (n < 0) {
        perror("write error");
        exit(1);
    }
}

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
void PrintSocketInfo(struct sockaddr_in servaddr, int sockfd){
    struct sockaddr_in cliaddr;
    char serv_IP[IPV4_LEN], local_IP[IPV4_LEN];
    unsigned int len = sizeof(cliaddr);

    printf("======== Connection extablished ========\n");
    // Prints server socket info
    inet_ntop(AF_INET, &servaddr.sin_addr, serv_IP, sizeof(serv_IP));
    printf("Server's IP: %s\nServer's Port: %d\n",serv_IP, ntohs(servaddr.sin_port));

    // Prints local socket info
    bzero(&cliaddr, sizeof(cliaddr));
    getsockname(sockfd, (struct sockaddr *) &cliaddr, &len);
    inet_ntop(AF_INET, &cliaddr.sin_addr, local_IP, sizeof(local_IP));
    printf("Local IP: %s\nLocal Port : %u\n", local_IP, ntohs(cliaddr.sin_port));
    printf("========================================\n");
}


/* ===========================================================================
 * FUNCTION: main
 *
 * DESCRIPTION: Connects to a remote socket and send Unix commands
 *
 * PARAMETERS:
 * argc - number of arguments passed
 * argv - arguments passed
 *
 * RETURN VALUE: 0 - if successful
 *
 * ===========================================================================*/
int main(int argc, char **argv) {
    int sockfd;
    char send_buffer_str[MAXDATASIZE], recv_buffer_str[MAXDATASIZE];
    struct sockaddr_in servaddr;

    // Initial socket configurations
    CheckArguments(argc,argv);
    sockfd = Socket(AF_INET, SOCK_STREAM, 0);
    ConfigureServSocket(&servaddr, argv[1], atoi(argv[2]));
    Connect(sockfd, &servaddr);

    // prints sockets informations
    PrintSocketInfo(servaddr, sockfd);

    for(;;) {
        fgets(send_buffer_str, MAXDATASIZE, stdin);
        Write(sockfd, send_buffer_str);

        Read(sockfd, recv_buffer_str);

        if (strncmp(recv_buffer_str, "sair", 4) == 0) {
            close(sockfd);
            printf(" Connection closed ");
            break;
        }

        // print on the screen the command
        Write(1, recv_buffer_str);
    }

    exit(0);

}
