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
  cmd->command = NULL;

  // Free dynamically allocated memory for args
  if (cmd->args != NULL) {
    for (size_t i = 0; i < cmd->argc; ++i) {
      if (cmd->args[i] != NULL) {
        free(cmd->args[i]);
      }
      cmd->args[i] = NULL;
    }
    free(cmd->args);
    cmd->args = NULL;
  }

  // Free the struct itself
  free(cmd);
  cmd = NULL;
}
