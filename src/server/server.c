#include <arpa/inet.h>
#include <bits/pthreadtypes.h>
#include <cbor/data.h>
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

int free_user(user_t *user) {
  user_t *tmpprev;
  user_t *tmpnext;

  if (user == NULL)
    return -1;

  // Server crashes when it tries to send message to a disconnected client
  // Perhaps we arent healing the linked list properly
  pthread_mutex_lock(&user->lock);
  tmpprev = user->prev;
  tmpnext = user->next;
  pthread_mutex_lock(&tmpprev->lock);
  pthread_mutex_lock(&tmpnext->lock);
  tmpprev->next = tmpnext;
  if (tmpnext != NULL) {
    tmpnext->prev = tmpprev;
  }
  pthread_mutex_unlock(&tmpnext->lock);
  pthread_mutex_unlock(&tmpprev->lock);
  pthread_mutex_unlock(&user->lock);
  pthread_mutex_destroy(&user->lock);

  if (user->fd < 1)
    close(user->fd);
  user->fd = -1;

  if (user != NULL)
    free(user);
  user = NULL;

  user->uid = -1;
  return 0;
}

int free_args(struct connectionArgs *args) {
  if (args == NULL)
    return -1;

  if (args->clientNode != NULL) {
    free_user(args->clientNode);
  }
  args->clientNode = NULL;

  if (args->start != NULL) {
    free_user(args->start);
  }
  args->start = NULL;

  if (args->clientfd < 1)
    close(args->clientfd);
  args->clientfd = -1;

  if (args != NULL)
    free(args);
  args = NULL;

  return 0;
}

int *setupLocalClient(void *arg) {
  Args *a = (Args *)arg;
  initClient(*a); // report check and handle
  return NULL;
}

void handleConnection(void *arg) {
  struct connectionArgs *args = (struct connectionArgs *)arg;
  unsigned char userBuf[256] = {0};
  user_t *current = NULL;
  Command *cmd = (Command *)malloc(sizeof(Command));

  // while connection not dead
  while ((read(args->clientfd, userBuf, 255)) > 0) {
    if ((deserializeBuffer(userBuf, &cmd)) == -1) {
      printf("\nFailed to deserialize buffer!");
      fflush(stdout);
      continue;
    }
    printf("\nRecv: %s", userBuf);
    fflush(stdout);

    current = args->start->next;
    while (current != NULL) {              // traverse linked list
      if (current->fd == args->clientfd) { // don't send back to sending client
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
cleanup:
  if (arg != NULL)
    free_args(arg);
  arg = NULL;

  if (cmd->args != NULL)
    free_command(cmd);
  cmd = NULL;
  pthread_exit(NULL); // Doesn't return anything
}

int spawn_connections(int sockfd, user_t *start, user_t **end) {
  int res = -1;
  int clientfd = -1;
  pthread_t newThread;
  pthread_mutex_t lock;
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
  pthread_mutex_init(&lock, NULL);
  *newConnection = (user_t){.fd = clientfd,
                            .prev = *end,
                            .next = NULL,
                            .uid = -1,
                            .nick = "",
                            .lock = lock};
  pthread_mutex_lock(&(*end)->lock);
  (*end)->next = newConnection;
  *end = (*end)->next;
  pthread_mutex_unlock(&(*end)->lock);

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

  if (pthread_detach(newThread) != 0) { // Allow thread to clean itself up
    printf("\nFailed to handle connection!");
    fflush(stdout);
    res = -1;
    goto cleanup;
  }
  res = 0;
  return res;
cleanup:
  // cleanup here
  return res;
}

int initServer(Args args) {
  int sockfd = -1;
  int port = -1;
  int res = -1;
  struct sockaddr_in address;
  // Create head
  user_t *start =
      &((user_t){.fd = -1,
                 .next = NULL,
                 .prev = NULL,
                 .uid = -1,
                 .nick = "",
                 .lock = PTHREAD_MUTEX_INITIALIZER}); // ADD R/W LOCK TO NODES
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

  while (spawn_connections(sockfd, start, &end) == 0)
    ;

  res = 0;
cleanup:
  if (sockfd < 1)
    close(sockfd);
  sockfd = -1;
  pthread_mutex_destroy(&start->lock);

  return res;
}
