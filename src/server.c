#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>

#include "args.h"

void initServer(Args args) {
  printf("Initializing Server\n");
  fflush(stdout);
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in address = {
    .sin_family = AF_INET,
    .sin_port = htons(args.port), // Error check this!
    .sin_addr = INADDR_ANY
  };
  
  int res = bind (sockfd, (struct sockaddr *)&address, sizeof(address));
  res = listen(sockfd, 10);
  int clientfd = accept(sockfd, 0, 0);
  
  // stdin - 0
  struct pollfd fds[2] = {
    {
      0,
      POLLIN,
      0
    },
    {
      clientfd,
      POLLIN,
      0
    }
  };

  for (;;) {
    char buffer[256] = { 0 };

    poll(fds, 2, 50000);

    if (fds[0].revents & POLLIN) {
      read(0, buffer, 255);
      send(clientfd, buffer, 255, 0);
    } else if (fds[1].revents & POLLIN) {
      if (recv(clientfd, buffer, 255, 0) == 0)
        return;
      printf("%s\n", buffer);
    }
  }
}