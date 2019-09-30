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

#define LISTENQ 10
#define MAXDATASIZE 100
#define MAXLINE 4096
#define MIN_ARG 3
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
        strcat(error," <Backlog>");
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
 * PARAMETERS:
 * listenfd - file descriptor of the socket
 * backlog - backlog size
 *
 * RETURN VALUE: none - if successful
 *
 * ===========================================================================*/
void Listen(int listenfd, int backlog) {
    if (listen(listenfd, backlog) == -1) {
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
        return -1;
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

    ticks = time(NULL);
    snprintf(log_msg, sizeof(log_msg), "%.24s\t", ctime(&ticks));
    strcat(log_msg, state);
    snprintf(log_aux, sizeof(log_aux), "Client's IP: %s | Client's Port: %d\n",cli_IP, ntohs(cliaddr.sin_port));
    strcat(log_msg, log_aux);

    fp = fopen("logger.txt", "a");
    fprintf(fp, "%s", log_msg);
    fclose(fp);
}


/* ===========================================================================
 * FUNCTION: PrintCommand
 *
 * DESCRIPTION: prints client's IP and Port and desired command
 *
 * PARAMETERS:
 * cliaddr - client's socket
 * command - input command
 *
 * RETURN VALUE: none
 *
 * ===========================================================================*/
void PrintCommand(struct sockaddr_in cliaddr, char *command){
    char cli_IP[IPV4_LEN];

    inet_ntop(AF_INET, &cliaddr.sin_addr, cli_IP, sizeof(cli_IP));
    printf("IP: %s, Port: %d, Command: %s\n",cli_IP, ntohs(cliaddr.sin_port), command);
}


/* ===========================================================================
 * FUNCTION: sig_child
 *
 * DESCRIPTION: handler of SIGCHILD
 *
 * RETURN VALUE: none
 *
 * ===========================================================================*/

void sig_child(int signo){
    pid_t pid;
    int stat;
    while( (pid = waitpid(-1, &stat, WNOHANG)) > 0)
        printf("child %d terminated\n", pid);
    return;
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
    FILE *fp;

    // Initial socket configurations
    CheckArguments(argc, argv);
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    ConfigureServSocket(&servaddr, atoi(argv[1]));
    Bind(listenfd, &servaddr);
    Listen(listenfd, atoi(argv[2]));
    signal(SIGCHLD, sig_child);

    for (;;) {

        // Accept connections
        connfd = Accept(listenfd, &cliaddr, &cliaddr_len);

        if (connfd == -1){
            if (errno == EINTR)
                continue;
            else
                perror("Accept");
        }

        /* after the connection is extablished the process executes a fork, which
         * will be responsible for the actual execution of the echo function.
         */
        if ((pid=fork()) == 0) {

            Close(listenfd);

            // Prints the new connected client socket
            PrintClientSocketInfo(cliaddr, 0);
            // Logs the connection from the client
            ConnectionLogger(cliaddr, 0);

            // clear send buffer
            bzero(send_buffer_str, MAXDATASIZE);

            // Reads the socket
            n = read(connfd, recv_buffer_str, MAXDATASIZE);
            if (n <= 0){
                PrintClientSocketInfo(cliaddr, 1);
                ConnectionLogger(cliaddr, 1);
                Close(connfd);
                exit(1);
            }

            // put end to the received string
            recv_buffer_str[n-1] = 0;

            // Prints client commands
            PrintCommand(cliaddr, recv_buffer_str);

            // open pipe for Unix socket
            fp = popen(recv_buffer_str, "r");

            // send data to client
            while (fgets(send_buffer_str, MAXDATASIZE, fp) != NULL){
                send(connfd, send_buffer_str, strlen(send_buffer_str), 0);
            }
            // close pipe
            pclose(fp);

            // Close connection
            Close(connfd);
            // Logs disconnection
            PrintClientSocketInfo(cliaddr, 1);
            ConnectionLogger(cliaddr, 1);
            exit(0);
        }

        Close(connfd);

        sleep(1000);
    }

    return(0);
}
