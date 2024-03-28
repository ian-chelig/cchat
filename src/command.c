#include <stdio.h>
#include <stdlib.h>

#include "command.h"

void free_command(Command *cmd) {
  printf("testfreecmd\n");
  fflush(stdout);
  if (cmd == NULL)
    return;

  // Free dynamically allocated memory for command
  printf("testfreecmdcmd\n");
  fflush(stdout);
  if (cmd->command != NULL) {
    free(cmd->command);
  }

  printf("testfreecmdargs\n");
  fflush(stdout);
  // Free dynamically allocated memory for args
  if (cmd->args != NULL) {
    for (size_t i = 0; i < cmd->argc; ++i) {
      if (cmd->args[i] != NULL) {
        free(cmd->args[i]);
      }
    }
    printf("testfreecmdargsargs\n");
    fflush(stdout);
    free(cmd->args);
  }

  // Free the struct itself
  printf("testfreecmd\n");
  fflush(stdout);
  free(cmd);
}
