#ifndef SERVER_H
#define SERVER_H

#include "args.h"

typedef struct user {
  int fd;
  int uid;
  char *nick;
  struct user *next;
  struct user *prev;
} user_t;

struct connectionArgs {
  int clientfd;
  user_t *start;
  user_t *clientNode;
};

int *setupLocalClient(void *arg);
int initServer(Args args);

#endif
