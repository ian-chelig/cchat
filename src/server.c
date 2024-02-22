#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "args.h"
#include "server.h"
#include "client.h"

void *setupLocalClient(void *arg) {
  Args *a = (Args *)arg;
  initClient(*a);
  return NULL;
}


void *handleConnection(void *arg) {
  struct connectionArgs *args = (struct connectionArgs *)arg;
  char buffer[255] = { 0 };
  int clientfd = args->clientfd;
  while (read(clientfd, buffer, 255) > 0) {
    fdNode_t *current = args->start->next;
    while (current != NULL) {
      if (current->fd == clientfd) {
        current = current->next;
        continue;
      }
      send(current->fd, buffer, 255, 0);
      current = current->next;
    }
  }

  fdNode_t *clientNode = args->clientNode;
  fdNode_t *tmp = clientNode->next->prev;
  clientNode->prev->next = clientNode->next;
  if (clientNode->next != NULL) {
    clientNode->next->prev = tmp;
  } else {
    clientNode->prev = NULL;
  }
  free(clientNode);

}

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
  
  fdNode_t *start = &((fdNode_t) {.fd = -1, .next = NULL, .prev = NULL});
  fdNode_t *end = start;

  for(;;) {
    int clientfd = accept(sockfd, 0, 0);
    pthread_t newThread;
    struct connectionArgs *args = (struct connectionArgs *)malloc(sizeof(struct connectionArgs)); //NEEDS R/W LOCK
    fdNode_t *newConnection = (fdNode_t *)malloc(sizeof(fdNode_t));
    *newConnection = (fdNode_t) {
      .fd = clientfd,
      .prev = end,
      .next = NULL
    };
    end->next = newConnection;
    *args = (struct connectionArgs) {
      .clientfd = clientfd,
      .start = start,
      .clientNode = newConnection
    };

    int result = pthread_create(&newThread, NULL, handleConnection, args);
  }
}