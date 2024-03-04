#ifndef COMMAND_H
#define COMMAND_H

#include <stddef.h>

typedef struct Command {
  char *key;
  char **args;
  size_t argc;
} Command;

#endif
