#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "args.h"
#include "cbor_functions.h"
#include "command.h"
#include "parser.h"

void client_cleanup(Command *cmd, unsigned char *buffer) {
  if (buffer != NULL)
    free(buffer);
  buffer = NULL;

  if (cmd->args != NULL) {
    free_command(cmd);
    cmd = NULL; // Avoid double free
  }
}

int initClient(Args args) {
  int sockfd = -1;
  int port = -1;
  char nickBuf[33] = {0}; // gotta change these sizes...
  unsigned char *nickoBuf = NULL;
  struct pollfd fds[2] = {{0, POLLIN, 0}, {sockfd, POLLIN, 0}}; // 0 = stdin
  struct in_addr addr = {0};
  struct sockaddr_in address = {0};
  int res = 0;

  printf("Initializing Client\n\n");
  fflush(stdout);

  port = htons(args.port);
  if (port < 0 || port > 65535) {
    printf("\nInvalid port number! Choose 1-65535");
    fflush(stdout);
    res = -1;
    goto cleanup;
  }

  addr.s_addr = inet_addr(args.c);
  if (addr.s_addr == INADDR_NONE) {
    printf("\nInvalid IP!");
    fflush(stdout);
    res = -1;
    goto cleanup;
  }

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    printf("\nCould not create socket!");
    fflush(stdout);
    res = -1;
    goto cleanup;
  }

  // assign address and server socket fd
  address.sin_family = AF_INET;
  address.sin_port = port;
  address.sin_addr = addr;
  fds[1].fd = sockfd;

  if ((connect(sockfd, (struct sockaddr *)&address, sizeof(address))) == -1) {
    printf("\nFailed to connect!");
    fflush(stdout);
    res = -1;
    goto cleanup;
  }

  // send username to server
  if ((sprintf(nickBuf, "/nick %s", args.u)) < 0) { // snprintf is safer
    printf("\nFailed to write nickname to buffer!");
    fflush(stdout);
    res = -1;
    goto cleanup;
  }

  if ((serializeBuffer(nickBuf, &nickoBuf)) == -1) {
    printf("\nFailed to serialize nickname!");
    fflush(stdout);
    res = -1;
    goto cleanup;
  }

  if ((send(sockfd, nickoBuf, 255, 0)) == -1) {
    printf("\nFailed to send nickname!");
    fflush(stdout);
    res = -1;
    goto cleanup;
  }

  if (nickoBuf != NULL) {
    free(nickoBuf);
  }
  nickoBuf = NULL;

  for (;;) { // are we only sending/receiving half the cbor object or
    Command *cmd = (Command *)malloc(sizeof(Command)); // memory leak
    char userBuf[256] = {0};
    char inBuffer[256] = {0};
    unsigned char recvBuf[256] = {0};
    unsigned char *outBuffer = NULL;

    if ((poll(fds, 2, 50000)) < 0) {
      printf("\nFailed to poll from file descriptors");
      fflush(stdout);
      client_cleanup(cmd, outBuffer);
      continue;
    }

    if (fds[0].revents & POLLIN) {
      if ((read(0, userBuf, 255)) < 1) { // read is not safe
        printf("\nFailed to read from stdin!");
        fflush(stdout);
        client_cleanup(cmd, outBuffer);
        res = -1;
        goto cleanup;
      }

      if ((serializeBuffer(userBuf, &outBuffer)) == -1) {
        printf("\nFailed to serialize buffer!");
        fflush(stdout);
        client_cleanup(cmd, outBuffer);
        continue;
      }

      if ((send(sockfd, outBuffer, 255, 0)) == -1) {
        printf("\nFailed to send buffer!");
        fflush(stdout);
        client_cleanup(cmd, outBuffer);
        continue;
      }
    } else if (fds[1].revents & POLLIN) {
      if ((recv(sockfd, recvBuf, 255, 0)) < 1) {
        printf("\nLost connection to server!");
        fflush(stdout);
        client_cleanup(cmd, outBuffer);
        res = 0;
        goto cleanup;
      }

      if ((deserializeBuffer(recvBuf, &cmd)) == -1) {
        printf("\nFailed to deserialize buffer!");
        fflush(stdout);
        client_cleanup(cmd, outBuffer);
        continue;
      }

      if ((plaintextFromMessageCMD(*cmd, inBuffer)) == -1) {
        printf("\nFailed to create buffer!");
        fflush(stdout);
        client_cleanup(cmd, outBuffer);
        continue;
      }

      printf("\n%s", inBuffer);
    }

    fflush(stdout);
    if (outBuffer != NULL)
      free(outBuffer);
    if (cmd->args != NULL)
      free_command(cmd);
  }

  res = 0;
cleanup:
  if (sockfd) {
    if ((close(sockfd)) == -1) {
      printf("Failed to close socket file descriptor!");
      fflush(stdout);
      res = -1;
    }
  }
  sockfd = -1;

  if (nickoBuf)
    free(nickoBuf);
  nickoBuf = NULL;
  return res;
}
