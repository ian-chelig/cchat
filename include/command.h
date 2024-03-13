#ifndef COMMAND_H
#define COMMAND_H

#include <stddef.h>

typedef struct Command {
  char *command;
  char **args;
  size_t argc;
} Command;

#endif
