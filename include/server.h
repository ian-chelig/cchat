#ifndef SERVER_H
#define SERVER_H

#include "args.h"


typedef struct fdNode {
    int fd;
    char *clientName;
    struct fdNode *next;
    struct fdNode *prev;
} fdNode_t;

struct connectionArgs {
  int clientfd;
  char *clientName;
  fdNode_t *start;
  fdNode_t *clientNode;
};

void *setupLocalClient(void *arg);
void initServer(Args args);

#endif
