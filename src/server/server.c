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
#include "cbor_functions.h"
#include "client.h"
#include "handler.h"
#include "parser.h"
#include "server.h"

void *setupLocalClient(void *arg) {
  Args *a = (Args *)arg;
  initClient(*a);
  return NULL;
}

void *handleConnection(void *arg) {
  struct connectionArgs *args = (struct connectionArgs *)arg;
  user_t user;
  // GENERATE A UID
  // COLLECT A NICK
  // MALLOC THE USER

  char buffer[255] = {0};
  char nick[33] = {0};
  int clientfd = args->clientfd;

  // handle initial connection setup

  while (read(clientfd, buffer, 255) > 0) { // while connection not dead
    cbor_item_t *item =
        deserializeData((size_t)strlen(buffer), (unsigned char *)buffer);
    Command cmd = createCommandFromItem(item);
    user_t *user = args->clientNode;
    handleCommand(cmd, *user);

    user_t *current = args->start->next;
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
  user_t *clientNode = args->clientNode;
  user_t *tmpprev = clientNode->prev;
  user_t *tmpnext = clientNode->next;
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
  int sockfd;

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) > 0) {
    printf("Could not create socket!\n");
    printf("%s\n", strerror(errno));
  }

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
  user_t *start = &((user_t){.fd = -1,
                             .next = NULL,
                             .prev = NULL,
                             .uid = -1,
                             .nick = ""}); // ADD R/W LOCK TO NODES
  user_t *end = start;

  for (;;) {
    int clientfd = accept(sockfd, 0, 0);

    // create new connection entry in linked list
    user_t *newConnection = (user_t *)malloc(sizeof(user_t));
    *newConnection = (user_t){
        .fd = clientfd, .prev = end, .next = NULL, .uid = -1, .nick = ""};
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
