#ifndef SERVER_H
#define SERVER_H

#include "args.h"


typedef struct fdNode {
    struct fdNode *next;
    struct fdNode *prev;
    int fd;
} fdNode_t;

struct connectionArgs {
  fdNode_t *start;
  fdNode_t *clientNode;
  int clientfd;
};

void *setupLocalClient(void *arg);
void initServer(Args args);

#endif