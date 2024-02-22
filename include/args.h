#ifndef ARGS_H
#define ARGS_H

#include <getopt.h>

typedef struct Args {
  int s;
  short port;
  char *c;
  int h;
} Args;

#endif