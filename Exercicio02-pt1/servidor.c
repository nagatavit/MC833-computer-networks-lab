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
#include <arpa/inet.h>

#define LISTENQ 10
#define MAXDATASIZE 100

int main (int argc, char **argv) {
   int    listenfd, connfd;
   struct sockaddr_in servaddr;
   char   buf[MAXDATASIZE];
   time_t ticks;
   unsigned int len;
   char cli_IP[16];
   struct sockaddr_in cliaddr;

   // Abre o socket do tipo IPv4
   if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      perror("socket");
      exit(1);
   }

   // Configura o tipo do socket (IPv4, deixar o sistema escolher o endereco e porta 1024)
   bzero(&servaddr, sizeof(servaddr));
   servaddr.sin_family      = AF_INET;
   servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
   servaddr.sin_port        = htons(1024);

   // Configura o socket gerado com as configuracoes acima
   if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
      perror("bind");
      exit(1);
   }

   // Deixa o modo do socket em listen
   if (listen(listenfd, LISTENQ) == -1) {
      perror("listen");
      exit(1);
   }

   for ( ; ; ) {
       // Espera uma conexao para dar aceite (e cria um socket para sessao)
       if ((connfd = accept(listenfd, (struct sockaddr *) NULL, NULL)) == -1 ) {
         perror("accept");
         exit(1);
      }

       /* -------------------------------------------------
          Modificacoes para imprimir as portas e ips remotos
          -------------------------------------------------- */

      len = sizeof(cliaddr);
      getpeername(connfd, (struct sockaddr *) &cliaddr, &len);
      inet_ntop(AF_INET, &cliaddr.sin_addr, cli_IP, sizeof(cli_IP));
      printf("Client IP: %s \nClient Port: %d\n", cli_IP, cliaddr.sin_port);

      // Pega o horario atual e envia pelo socket
      ticks = time(NULL);
      snprintf(buf, sizeof(buf), "%.24s\r\n", ctime(&ticks));
      write(connfd, buf, strlen(buf));

      // Fecha socket do cliente
      close(connfd);
   }
   return(0);
}
