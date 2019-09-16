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

int Socket(int family, int type, int flags) {
    int sockfd;
    if ((sockfd = socket(family, type, flags)) < 0) {
        perror("socket");
        exit(1);
    } else
        return sockfd;
}

void ConfigureServSocket(struct sockaddr_in *servaddr, int port) {
    bzero(servaddr, sizeof(*servaddr));
    servaddr->sin_family = AF_INET;
    servaddr->sin_port   = htons(port);
}

void Bind(int listenfd, struct sockaddr_in *servaddr) {
    if (bind(listenfd, (struct sockaddr *)servaddr, sizeof(*servaddr)) == -1) {
        perror("bind");
        exit(1);
    }
}

void Listen(int listenfd) {
    if (listen(listenfd, LISTENQ) == -1) {
        perror("listen");
        exit(1);
    }
}

int Accept(int listenfd, struct sockaddr_in *addr, unsigned int *addrlen){
    int connfd;
    if ((connfd = accept(listenfd, (struct sockaddr *) addr, addrlen)) == -1 ) {
        perror("accept");
        exit(1);
    } else {
        return connfd;
    }
}

void Close(int sockfd) {
    if (close(sockfd) == -1) {
        perror("close");
    }
}

int main (int argc, char **argv) {
    int listenfd, connfd, n;
    char buffer_str[MAXLINE + 1];
    struct sockaddr_in servaddr;
    struct sockaddr_in cliaddr;
    unsigned int cliaddr_len = sizeof cliaddr;
    char cli_IP[IPV4_LEN];
    pid_t pid;
    /* FILE *logger; */

    /* fopen("servidor_log.txt", "a"); */

    CheckArguments(argc, argv);

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    ConfigureServSocket(&servaddr, atoi(argv[1]));

    Bind(listenfd, &servaddr);

    Listen(listenfd);

    for (;;) {

        connfd = Accept(listenfd, &cliaddr, &cliaddr_len);

        if ((pid=fork()) == 0) {

            Close(listenfd);

            inet_ntop(AF_INET, &cliaddr.sin_addr, cli_IP, sizeof(cli_IP));
            printf("Client's IP: %s\nClient's Port: %d\n",cli_IP, ntohs(cliaddr.sin_port));

            while ( (n = read(connfd, buffer_str, MAXLINE)) > 0) {
                buffer_str[n] = 0;

                /* buffer_str[n] = 0; */
                /* if (fputs(buffer_str, stdout) == EOF) { */
                /*     perror("fputs error"); */
                /*     exit(1); */
                /* } */
                /* printf("%d\n", n); */
                /* printf("%s\n", buffer_str); */
                write(connfd, buffer_str, strlen(buffer_str));
                system(buffer_str);

            }

            Close(connfd);
            exit(0);
        }

        Close(connfd);
    }

    return(0);

}
