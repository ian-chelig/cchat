#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "args.h"
#include "cbor_functions.h"
#include "client.h"
#include "command.h"
#include "handler.h"
#include "parser.h"
#include "server.h"

int *setupLocalClient(void *arg) {
  Args *a = (Args *)arg;
  initClient(*a); // report check and handle
  return NULL;
}

int handleConnection(void *arg) {
  struct connectionArgs *args = (struct connectionArgs *)arg;
  int clientfd = args->clientfd;
  unsigned char outBuffer[256] = {0};
  unsigned char userBuf[256] = {0};
  char *deserialized;
  unsigned char *serialized;
  user_t *current;
  user_t *clientNode;
  user_t *tmpprev;
  user_t *tmpnext;
  Command *cmd = (Command *)malloc(sizeof(Command));

  while ((read(clientfd, userBuf, 255)) > 0) { // while connection not dead
    if ((deserializeBuffer(userBuf, &cmd)) == -1) {
      printf("\nFailed to deserialize buffer!");
      fflush(stdout);
      continue;
    }
    printf("\nRecv: %s", userBuf);
    fflush(stdout);

    current = args->start->next;
    while (current != NULL) {        // traverse linked list
      if (current->fd == clientfd) { // don't send back to sending client
        current = current->next;
        continue;
      }

      // send to other clients
      if ((send(current->fd, userBuf, 255, 0)) == -1) {
        printf("\nFailed to send data!");
        fflush(stdout);
        continue;
      }
      printf("\nSent: %s", userBuf);
      fflush(stdout);

      current = current->next;
    }
  }

  // remove client node from linked list, heal the linked list
  // might be more cleanup that needs to be done here??
  clientNode = args->clientNode;
  tmpprev = clientNode->prev;
  tmpnext = clientNode->next;
  tmpprev->next = tmpnext;
  if (tmpnext != NULL) {
    tmpnext->prev = tmpprev;
  }
  free(clientNode);

  return 0;
}

int initServer(Args args) {
  int sockfd = -1;
  int port = -1;
  int res = -1;
  struct sockaddr_in address;
  // Create head
  user_t *start = &((user_t){.fd = -1,
                             .next = NULL,
                             .prev = NULL,
                             .uid = -1,
                             .nick = ""}); // ADD R/W LOCK TO NODES
  user_t *end = start;

  printf("Initializing Server\n");
  fflush(stdout);

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    printf("\nFailed to create socket!");
    fflush(stdout);
    res = -1;
    goto cleanup;
  }

  port = htons(args.port);
  if (port < 0 || port > 65535) {
    printf("\nInvalid port! Choose a range 1-65535");
    fflush(stdout);
    res = -1;
    goto cleanup;
  }

  address.sin_family = AF_INET;
  address.sin_port = port;
  address.sin_addr.s_addr = INADDR_ANY;

  if ((bind(sockfd, (struct sockaddr *)&address, sizeof(address))) < 0) {
    printf("\nFailed to bind to socket!");
    fflush(stdout);
    res = -1;
    goto cleanup;
  }

  if ((listen(sockfd, 10)) < 0) {
    printf("\nFailed to listen on socket!");
    fflush(stdout);
    res = -1;
    goto cleanup;
  }

  for (;;) {
    int clientfd = -1;
    pthread_t newThread;
    user_t *newConnection = (user_t *)malloc(sizeof(user_t));
    struct connectionArgs *connArgs =
        (struct connectionArgs *)malloc(sizeof(struct connectionArgs));

    clientfd = accept(sockfd, 0, 0);
    if (clientfd == -1) {
      printf("\nFailed to accept connection!");
      fflush(stdout);
      res = -1;
      goto cleanup;
    }

    // create new connection entry in linked list
    *newConnection = (user_t){
        .fd = clientfd, .prev = end, .next = NULL, .uid = -1, .nick = ""};
    end->next = newConnection;
    end = end->next;

    // provide proper information to connection handler thread
    *connArgs = (struct connectionArgs){
        .clientfd = clientfd, .start = start, .clientNode = newConnection};

    if ((pthread_create(&newThread, NULL, (void *)handleConnection, connArgs)) <
        0) {
      printf("\nFailed to handle connection!");
      fflush(stdout);
      res = -1;
      goto cleanup;
    }
  }

  res = 0;
cleanup:
  return res;
}
