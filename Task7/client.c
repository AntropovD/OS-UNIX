#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#define HOST "127.0.0.1"
#define PORT "5001"

#define HEIGHT 20
#define WIDTH 40

int main(int argc, char *argv[])
{
   int sockfd, portno, n;
   struct sockaddr_in serv_addr;
   struct hostent *server;

   portno = atoi(PORT);
   sockfd = socket(AF_INET, SOCK_STREAM, 0);

   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }

   server = gethostbyname(HOST);
   if (server == NULL) {
      fprintf(stderr,"ERROR, no such host\n");
      exit(0);
   }

   bzero((char *) &serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
   serv_addr.sin_port = htons(portno);

   if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR connecting");
      exit(1);
   }

   char buffer[HEIGHT * WIDTH];
   bzero(buffer, HEIGHT * WIDTH);

   n = read(sockfd, buffer, HEIGHT*WIDTH);

   if (n < 0) {
      perror("ERROR reading from socket");
      exit(1);
   }
   int i,j;
   for (i = 0; i < HEIGHT; i++) {
       for (j = 0; j < WIDTH; j++)
           printf("%c", buffer[i * WIDTH + j]);
       printf("\n");
   }
   return 0;
}
