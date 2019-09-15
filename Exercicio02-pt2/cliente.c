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

// Checks if the client has passed the arguments (IP and port)
void CheckArguments(int argc, char **argv, char error[MAXLINE + 1]){
    if (argc != 3) {
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

/* ===========================================================================
 * FUNCTION: main
 *
 * DESCRIPTION:
 *
 * PARAMETERS:
 *
 * DEPENDENDECIES:
 *
 * RETURN VALUE: 0 - if successful
 *
 * ===========================================================================*/
int main(int argc, char **argv) {
    int sockfd, n;
    char buffer_str[MAXLINE + 1];
    char error[MAXLINE + 1];
    struct sockaddr_in servaddr;

    CheckArguments(argc,argv,error);

    sockfd = Socket(AF_INET, SOCK_STREAM, 0);

    ConfigureServSocket(&servaddr, argv[1], atoi(argv[2]));

    Connect(sockfd, &servaddr);

    for(;;) {
        fgets(buffer_str, MAXLINE, stdin);
        write(sockfd, buffer_str, strlen(buffer_str));

       n = read(sockfd, buffer_str, MAXLINE);
       if (n < 0)
           exit(1);
       buffer_str[n] = 0;
       write(1, buffer_str, strlen(buffer_str));
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
