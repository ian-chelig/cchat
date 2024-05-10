#include <arpa/inet.h>
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
  if (user == NULL)
    return -1;

  if (user->nick != NULL) {
    free(user->nick);
  }

  if (user->next != NULL) {
    free(user->next);
  }
  user->next = NULL;

  if (user->prev != NULL) {
    free(user->prev);
  }
  user->prev = NULL;

  free(user);
  user->fd = -1;
  user->uid = -1;
  user = NULL;
  return 0;
}

int free_args(struct connectionArgs *args) {
  if (args == NULL)
    return -1;

  // Free dynamically allocated memory for command

  // Free dynamically allocated memory for args
  if (args->clientNode != NULL) {
    free_user(args->clientNode);
  }
  args->clientNode = NULL;

  if (args->start != NULL) {
    free_user(args->start);
  }
  args->start = NULL;
  // Free the struct itself
  free(args);
  args->clientfd = -1;
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
  user_t *tmpprev = NULL;
  user_t *tmpnext = NULL;
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

  // remove client node from linked list, heal the linked list
  // might be more cleanup that needs to be done here??
  tmpprev = args->clientNode->prev;
  tmpnext = args->clientNode->next;
  tmpprev->next = tmpnext;
  if (tmpnext != NULL) {
    tmpnext->prev = tmpprev;
  }

cleanup:
  if (cmd->args != NULL)
    free_command(cmd);
  cmd = NULL;
  pthread_exit(NULL); // Doesn't return anything
}

int spawn_connectons(int sockfd, user_t *start, user_t **end) {
  int res = -1;

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
      .fd = clientfd, .prev = *end, .next = NULL, .uid = -1, .nick = ""};
  (*end)->next = newConnection;
  *end = (*end)->next;

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
cleanup:
  if (clientfd < 1)
    close(clientfd);
  clientfd = -1;

  // seg faults in here somewhere
  // i suspect it may be because of us freeing
  // memory that the detached threads are using
  //
  if (connArgs != NULL)
    free_args(connArgs);
  connArgs = NULL;

  if (newConnection != NULL)
    free_user(newConnection);
  newConnection = NULL;

  return res;
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

  while (spawn_connectons(sockfd, start, &end) == 0)
    ;

  res = 0;
cleanup:
  if (sockfd < 1)
    close(sockfd);
  sockfd = -1;

  return res;
}
