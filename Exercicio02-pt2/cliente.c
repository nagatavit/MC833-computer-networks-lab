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

// Create Socket
int Socket(int family, int type, int flags) {
    int sockfd;
    if ((sockfd = socket(family, type, flags)) < 0) {
        perror("socket");
        exit(1);
    } else
        return sockfd;
}

// Initialize servaddr
void ConfigureServSocket(struct sockaddr_in *servaddr, char *ip, int port) {
    bzero(servaddr, sizeof(*servaddr));
    servaddr->sin_family = AF_INET;
    servaddr->sin_port   = htons(port);
    if (inet_pton(AF_INET, ip, &(servaddr->sin_addr)) <= 0) {
        perror("inet_pton error");
        exit(1);
    }
}

// Connect to a remote socket
void Connect(int sockfd, struct sockaddr_in *servaddr) {
    if (connect(sockfd, (struct sockaddr *) servaddr, sizeof(*servaddr)) < 0) {
        perror("connect error");
        exit(1);
    }
}

// Main
int main(int argc, char **argv) {
    int sockfd, n;
    char send_buffer_str[MAXDATASIZE], recv_buffer_str[MAXDATASIZE];
    char serv_IP[IPV4_LEN], local_IP[IPV4_LEN];
    struct sockaddr_in servaddr, cliaddr;
    unsigned int len = sizeof(cliaddr);

    CheckArguments(argc,argv);

    sockfd = Socket(AF_INET, SOCK_STREAM, 0);

    ConfigureServSocket(&servaddr, argv[1], atoi(argv[2]));

    Connect(sockfd, &servaddr);

    inet_ntop(AF_INET, &servaddr.sin_addr, serv_IP, sizeof(serv_IP));
    printf("Server's IP: %s\nServer's Port: %d\n",serv_IP, ntohs(servaddr.sin_port));

    bzero(&cliaddr, sizeof(cliaddr));
    getsockname(sockfd, (struct sockaddr *) &cliaddr, &len);
    inet_ntop(AF_INET, &cliaddr.sin_addr, local_IP, sizeof(local_IP));
    printf("Local IP address: %s\n", local_IP);
    printf("Local Port : %u\n", ntohs(cliaddr.sin_port));

    for(;;) {
        fgets(send_buffer_str, MAXLINE, stdin);
        write(sockfd, send_buffer_str, strlen(send_buffer_str));

        n = read(sockfd, recv_buffer_str, MAXLINE);
        if (n < 0)
            exit(1);
        recv_buffer_str[n] = 0;
        write(1, recv_buffer_str, strlen(recv_buffer_str));
    }

    /* while ( (n = read(sockfd, recvline, MAXLINE)) > 0) { */
    /*    recvline[n] = 0; */
    /*    if (fputs(recvline, stdout) == EOF) { */
    /*       perror("fputs error"); */
    /*       exit(1); */
    /*    } */
    /* } */

    if (n < 0) {
        perror("read error");
        exit(1);
    }

    exit(0);
}
