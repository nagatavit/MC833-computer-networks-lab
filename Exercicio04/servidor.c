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
#include <sys/wait.h>
#include <signal.h>
#include "servidor.h"

#define LISTENQ 10
#define MAXDATASIZE 100
#define MAXLINE 4096
#define MIN_ARG 3
#define IPV4_LEN 16

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
    char send_buffer_str[MAXLINE + 1], recv_buffer_str[MAXLINE + 1];
    struct sockaddr_in servaddr, cliaddr;

    unsigned int cliaddr_len = sizeof cliaddr;

    pid_t pid;

    // Initial socket configurations
    CheckArguments(argc, argv);
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    ConfigureServSocket(&servaddr, atoi(argv[1]));
    Bind(listenfd, &servaddr);
    Listen(listenfd, atoi(argv[2]));
    signal(SIGCHLD, sig_child);

    for (;;){
        // Accept connections
        connfd = Accept(listenfd, &cliaddr, &cliaddr_len);

        // If there is a disconnection during the accept process, and the
        // interrupt causes an error, try to accept the connection again
        if (connfd == -1){
            if (errno == EINTR)
                continue;
            else {
                perror("Accept");
                exit(1);
            }
        }

        /* after the connection is extablished the process executes a fork, which
         * will be responsible for the actual execution of the echo function.
         */
        if ((pid=fork()) == 0) {

            Close(listenfd);

            // Prints the new connected client socket
            /* PrintClientSocketInfo(cliaddr, 0); */
            // Logs the connection from the client
            /* ConnectionLogger(cliaddr, 0); */

            // clear send buffer
            bzero(send_buffer_str, MAXDATASIZE);

            // Reads the socket
            while((n = read(connfd, recv_buffer_str, MAXLINE)) > 0){
                // put end to the received string
                recv_buffer_str[n-1] = 0;

                /* printf("%s", recv_buffer_str); */
                write(connfd, recv_buffer_str, strlen(recv_buffer_str));
            }

            // Close connection
            Close(connfd);

            // Logs disconnection
            /* PrintClientSocketInfo(cliaddr, 1); */
            /* ConnectionLogger(cliaddr, 1); */
            exit(0);
        }

        Close(connfd);
    }

    return(0);
}

void CheckArguments(int argc, char **argv){
    char error[MAXLINE + 1];
    if (argc != MIN_ARG) {
        strcpy(error,"uso: ");
        strcat(error,argv[0]);
        strcat(error," <Port>");
        strcat(error," <Backlog>");
        perror(error);
        exit(1);
    }
}

int Socket(int family, int type, int flags) {
    int sockfd;
    if ((sockfd = socket(family, type, flags)) < 0) {
        perror("socket");
        exit(1);
    } else {
        return sockfd;
    }
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

void Listen(int listenfd, int backlog) {
    if (listen(listenfd, backlog) == -1) {
        perror("listen");
        exit(1);
    }
}

int Accept(int listenfd, struct sockaddr_in *addr, unsigned int *addrlen){
    int connfd;
    if ((connfd = accept(listenfd, (struct sockaddr *) addr, addrlen)) == -1 ) {
        perror("accept");
        return -1;
    } else {
        return connfd;
    }
}


void Close(int sockfd) {
    if (close(sockfd) == -1) {
        perror("close");
    }
}

void PrintClientSocketInfo(struct sockaddr_in cliaddr, int connect_disconnect){
    char cli_IP[IPV4_LEN];
    char connect_string[MAXLINE];
    // Prints client socket information

    if (connect_disconnect == 0)
        strcpy(connect_string,"======== New connection ========");
    else
        strcpy(connect_string,"======== Disconnection ========");

    printf("%s\n", connect_string);
    inet_ntop(AF_INET, &cliaddr.sin_addr, cli_IP, sizeof(cli_IP));
    printf("Client's IP: %s\nClient's Port: %d\n",cli_IP, ntohs(cliaddr.sin_port));
    printf("================================\n");
}

void ConnectionLogger(struct sockaddr_in cliaddr, int connect_disconnect){
    char cli_IP[IPV4_LEN];
    char state[MAXLINE];
    char log_msg[MAXLINE], log_aux[MAXLINE];
    time_t ticks;
    FILE *fp;

    if (connect_disconnect == 0)
        strcpy(state, "[Connetion] - ");
    else
        strcpy(state, "[Disconnect] - ");

    // converts cliaddr to IP string
    inet_ntop(AF_INET, &cliaddr.sin_addr, cli_IP, sizeof(cli_IP));

    ticks = time(NULL);
    snprintf(log_msg, sizeof(log_msg), "%.24s\t", ctime(&ticks));
    strcat(log_msg, state);
    snprintf(log_aux, sizeof(log_aux), "Client's IP: %s | Client's Port: %d\n",cli_IP, ntohs(cliaddr.sin_port));
    strcat(log_msg, log_aux);

    fp = fopen("logger.txt", "a");
    fprintf(fp, "%s", log_msg);
    fclose(fp);
}

void sig_child(int signo){
    pid_t pid;
    int stat;
    while( (pid = waitpid(-1, &stat, WNOHANG)) > 0)
        printf("\nchild %d terminated\n", pid);
    return;
}
