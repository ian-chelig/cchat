#include <arpa/inet.h>
#include <errno.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "args.h"
#include "client.h"
#include "server.h"

void *setupLocalClient(void *arg) {
  Args *a = (Args *)arg;
  initClient(*a);
  return NULL;
}

void *handleConnection(void *arg) {
  struct connectionArgs *args = (struct connectionArgs *)arg;
  char buffer[255] = {0};
  char nick[33] = {0};
  int clientfd = args->clientfd;

  // handle initial connection setup

  while (read(clientfd, buffer, 255) > 0) { // while connection not dead
    fdNode_t *current = args->start->next;
    while (current != NULL) {        // traverse linked list
      if (current->fd == clientfd) { // don't send back to sending client
        current = current->next;
        continue;
      }

      send(current->fd, buffer, 255, 0); // send to other clients
      current = current->next;
    }
  }

  // remove client node from linked list, heal the linked list
  fdNode_t *clientNode = args->clientNode;
  fdNode_t *tmpprev = clientNode->prev;
  fdNode_t *tmpnext = clientNode->next;
  tmpprev->next = tmpnext;
  if (tmpnext != NULL) {
    tmpnext->prev = tmpprev;
  }
  free(clientNode);

  return NULL;
}

void initServer(Args args) {
  printf("Initializing Server\n");
  fflush(stdout);
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in address = {.sin_family = AF_INET,
                                .sin_port = htons(args.port),
                                .sin_addr = INADDR_ANY};

  if (bind(sockfd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    printf("Could not bind to socket!\n");
    printf("%s\n", strerror(errno));
  }
  if (listen(sockfd, 10) < 0) {
    printf("Could not listen on bound socket!\n");
    printf("%s\n", strerror(errno));
  }

  // create head
  fdNode_t *start = &((fdNode_t){
      .fd = -1, .next = NULL, .prev = NULL}); // ADD R/W LOCK TO NODES
  fdNode_t *end = start;

  for (;;) {
    int clientfd = accept(sockfd, 0, 0);

    // create new connection entry in linked list
    fdNode_t *newConnection = (fdNode_t *)malloc(sizeof(fdNode_t));
    *newConnection = (fdNode_t){.fd = clientfd, .prev = end, .next = NULL};
    end->next = newConnection;
    end = end->next;

    // provide proper information to connection handler thread
    struct connectionArgs *connArgs =
        (struct connectionArgs *)malloc(sizeof(struct connectionArgs));
    *connArgs = (struct connectionArgs){
        .clientfd = clientfd, .start = start, .clientNode = newConnection};

    pthread_t newThread;
    if (pthread_create(&newThread, NULL, handleConnection, connArgs) < 0) {
      printf("Could not create connection handler thread!\n");
      printf("%s\n", strerror(errno));
    }
  }
}
