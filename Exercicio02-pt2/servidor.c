#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define LISTENQ 10
#define MAXDATASIZE 100
#define MAXLINE 4096
#define MIN_ARG 2

void CheckArguments(int argc, char **argv, char error[MAXLINE + 1]){
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

int main (int argc, char **argv) {
    int listenfd, connfd, n;
    char buffer_str[MAXLINE + 1];
    struct sockaddr_in servaddr;
    char error[MAXLINE + 1];

    CheckArguments(argc, argv, error);

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    ConfigureServSocket(&servaddr, atoi(argv[1]));

    Bind(listenfd, &servaddr);

    Listen(listenfd);

    for ( ; ; ) {
        if ((connfd = accept(listenfd, (struct sockaddr *) NULL, NULL)) == -1 ) {
            perror("accept");
            exit(1);
        }

        /*    n = read(connfd, buffer_str, MAXLINE); */
        /*    if (n < 0) */
        /*        exit(1); */
        /*    buffer_str[n] = 0; */
        /*    write(connfd, buffer_str, strlen(buffer_str)); */

        /*    close(connfd); */

        while ( (n = read(connfd, buffer_str, MAXLINE)) > 0) {
            buffer_str[n] = 0;

            /* buffer_str[n] = 0; */
            /* if (fputs(buffer_str, stdout) == EOF) { */
            /*     perror("fputs error"); */
            /*     exit(1); */
            /* } */
            printf("%d\n", n);
            printf("%s\n", buffer_str);
            write(connfd, buffer_str, strlen(buffer_str));
            system(buffer_str);
        }
    }

    return(0);
}
