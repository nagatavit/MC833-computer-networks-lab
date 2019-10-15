#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "cliente.h"
#define MAXLINE 1024
#define MAXDATASIZE 100
#define IPV4_LEN 16

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
    int n;
    char send_buffer_str[MAXLINE + 1], recv_buffer_str[MAXLINE + 1];
    struct sockaddr_in servaddr;
    FILE *fp = stdin; // file to be sent (via redirect)
    fd_set rset; // select descriptor for multiplexing
    int sockfd; // socket de conexão ao servidor
    int maxfdp1; // number of file descriptors to multiplex

    // initial socket configurations
    /* CheckArguments(argc,argv); */
    sockfd = Socket(AF_INET, SOCK_STREAM, 0);
    ConfigureServSocket(&servaddr, argv[1], atoi(argv[2]));
    Connect(sockfd, &servaddr);

    // set select descriptor to zero
    FD_ZERO(&rset);

    for(;;) {
        // configure file descriptors to select (file + socket)
        FD_SET(fileno(fp), &rset);
        FD_SET(sockfd, &rset);
        // number of descriptors + 1
        maxfdp1 = 4;

        // I/O multiplexing with select
        select(maxfdp1, &rset, NULL, NULL, NULL);

        // ------------------------- //
        // ------ Multiplexing ---- //
        // ------------------------- //

        // socket activity
        if (FD_ISSET(sockfd, &rset)){
            if ((n = read(sockfd, recv_buffer_str, MAXLINE)) == 0){
                perror("server terminated connection");
                return 0;
            }

            recv_buffer_str[n] = 0;
            fputs(recv_buffer_str, stdout);
        }

        // file activity
        if (FD_ISSET(fileno(fp), &rset)){
            if (fgets(send_buffer_str, MAXLINE, fp) != NULL){
                write(sockfd, send_buffer_str, strlen(send_buffer_str));
            }

            if (feof(fp)){
                shutdown(sockfd, SHUT_WR);
            }
        }

    }
}

void CheckArguments(int argc, char **argv){
    char error[MAXLINE + 1];

    if (argc != 4) {
        strcpy(error,"uso: ");
        strcat(error,argv[0]);
        strcat(error," <IPaddress>");
        strcat(error," <Port>");
        perror(error);
        exit(1);
    }
}

int Socket(int family, int type, int flags) {
    int sockfd;
    if ((sockfd = socket(family, type, flags)) < 0) {
        perror("socket");
        exit(1);
    } else
        return sockfd;
}

void ConfigureServSocket(struct sockaddr_in *servaddr, char *ip, int port) {
    bzero(servaddr, sizeof(*servaddr));
    servaddr->sin_family = AF_INET;
    servaddr->sin_port   = htons(port);
    if (inet_pton(AF_INET, ip, &(servaddr->sin_addr)) <= 0) {
        perror("inet_pton error");
        exit(1);
    }
}

void Connect(int sockfd, struct sockaddr_in *servaddr) {
    if (connect(sockfd, (struct sockaddr *) servaddr, sizeof(*servaddr)) < 0) {
        perror("connect error");
        exit(1);
    }
}

int Read(int sockfd, char *recv_buffer_str){
    int n;

    n = read(sockfd, recv_buffer_str, MAXDATASIZE);

    // if errors occur during the receiving package
    if (n < 0) {
        perror("read error");
        exit(1);
    }

    // put a \0 at the end of the string
    recv_buffer_str[n] = 0;

    return n;
}

void Write(int sockfd, char *send_buffer_str){
    int n;
    n = write(sockfd, send_buffer_str, strlen(send_buffer_str));

    // if errors occur during the writting
    if (n < 0) {
        perror("write error");
        exit(1);
    }
}

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

