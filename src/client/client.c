#include <arpa/inet.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "args.h"
#include "cbor_functions.h"
#include "command.h"
#include "parser.h"

void initClient(Args args) {
  printf("Initializing Client\n\n");
  fflush(stdout);
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in address = {.sin_family = AF_INET,
                                .sin_port =
                                    htons(args.port), // Error check this!
                                .sin_addr = inet_addr(args.c)};

  // handle username input

  int result = connect(sockfd, (struct sockaddr *)&address, sizeof(address));

  // stdin - 0
  struct pollfd fds[2] = {{0, POLLIN, 0}, {sockfd, POLLIN, 0}};
  // set username
  char nBuf[256] = {0};
  sprintf(nBuf, "/nick %s", args.u);
  unsigned char *serialized = serializeBuffer(nBuf);
  send(sockfd, serialized, 255, 0);
  struct Command cmd;
  cbor_item_t item;

  for (;;) {
    char buffer[256] = {0};
    poll(fds, 2, 50000);
    if (fds[0].revents & POLLIN) {
      read(0, buffer, 255);

      if (buffer[0] != '/') {
        send(sockfd, buffer, 255, 0);
      }
    } else if (fds[1].revents & POLLIN) {
      if (recv(sockfd, buffer, 255, 0) == 0)
        return;
      // deserializeBuffer();
      printf("%s", buffer);
    }

    fflush(stdout);
  }
}
