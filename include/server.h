#ifndef SERVER_H
#define SERVER_H

#include "args.h"

struct connectionArgs {
  int index;
  int *length;
  int *array;
};

void initServer(Args args);

#endif