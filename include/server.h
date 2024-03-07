#ifndef SERVER_H
#define SERVER_H

#include "args.h"
#include "user.h"

typedef struct fdNode {
  int fd;
  User user;
  struct fdNode *next;
  struct fdNode *prev;
} fdNode_t;

struct connectionArgs {
  int clientfd;
  fdNode_t *start;
  fdNode_t *clientNode;
};

void *setupLocalClient(void *arg);
void initServer(Args args);

#endif
