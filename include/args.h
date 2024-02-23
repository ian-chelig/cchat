#ifndef ARGS_H
#define ARGS_H

#include <getopt.h>

typedef struct Args {
  int s;
  short port;
  char *c;
  char *u;
  int d;
} Args;

#endif
