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

int main(int argc, char **argv) {
   int    sockfd, n;
   char   recvline[MAXLINE + 1];
   char   error[MAXLINE + 1];
   struct sockaddr_in servaddr;
   struct sockaddr_in cliaddr;
   unsigned int len;
   char cli_IP[16];

   // Verifica se o IP foi passado como argumento
   if (argc != 2) {
      strcpy(error,"uso: ");
      strcat(error,argv[0]);
      strcat(error," <IPaddress>");
      perror(error);
      exit(1);
   }

   // Aloca um socket e recebe o file descriptor
   if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      perror("socket error");
      exit(1);
   }

   // Zera a struct do servaddr
   bzero(&servaddr, sizeof(servaddr));
   // Tipo de IP = IPv4
   servaddr.sin_family = AF_INET;
   // Porta utilizada (1024 apos modificacao)
   servaddr.sin_port   = htons(1024);
   // Converte o IP passado como argumento para binario e coloca no socket
   if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
      perror("inet_pton error");
      exit(1);
   }

   // conecta ao socket configurado
   if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
      perror("connect error");
      exit(1);
   }

   /* -------------------------------------------------
      Modificacoes para imprimir as portas e ips locais
    -------------------------------------------------- */

   len = sizeof(cliaddr);

   getsockname(sockfd, (struct sockaddr *) &cliaddr, &len);
   inet_ntop(AF_INET, &cliaddr.sin_addr, cli_IP, sizeof(cli_IP));
   printf("Local IP: %s \nLocal Port: %d\n", cli_IP, cliaddr.sin_port);

   // Leitura dos pacotes recebidos pelo socket
   while ( (n = read(sockfd, recvline, MAXLINE)) > 0) {
      recvline[n] = 0;
      // Imprime na saida padrao
      if (fputs(recvline, stdout) == EOF) {
         perror("fputs error");
         exit(1);
      }
   }

   if (n < 0) {
      perror("read error");
      exit(1);
   }

   exit(0);
}
