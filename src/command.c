#include <stdio.h>
#include <stdlib.h>

#include "command.h"

void free_command(Command *cmd) {
  if (cmd == NULL)
    return;

  // Free dynamically allocated memory for command
  if (cmd->command != NULL) {
    free(cmd->command);
  }

  // Free dynamically allocated memory for args
  if (cmd->args != NULL) {
    for (size_t i = 0; i < cmd->argc; ++i) {
      if (cmd->args[i] != NULL) {
        free(cmd->args[i]);
      }
    }
    free(cmd->args);
  }

  // Free the struct itself
  free(cmd);
}
