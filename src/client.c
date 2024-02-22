#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>

#include "args.h"

void initClient(Args args) {
  printf("Initializing Client\n");
  fflush(stdout);
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in address = {
    .sin_family = AF_INET,
    .sin_port = htons(args.port), // Error check this!
    .sin_addr = inet_addr(args.c)
  };

  int result = connect(sockfd, (struct sockaddr *)&address, sizeof(address));

  // stdin - 0
  struct pollfd fds[2] = {
    {
      0,
      POLLIN,
      0
    },
    {
      sockfd,
      POLLIN,
      0
    }
  };
  
  for (;;) {
   char buffer[256] = { 0 };
  
   poll(fds, 2, 50000);
  
   if (fds[0].revents & POLLIN) {
     read(0, buffer, 255);
     send(sockfd, buffer, 255, 0);
   } else if (fds[1].revents & POLLIN) {
     if (recv(sockfd, buffer, 255, 0) == 0) {
        return;
      }
     printf("%s\n", buffer);
   }
  }
}
