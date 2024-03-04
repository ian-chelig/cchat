#include <arpa/inet.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "command.h"
#include "parser.h"
#include "server.h"
#include "tlpi_hdr.h"

Command createCommandFromBuffer(char *buffer) {
  Command cmd;
  cmd.argc = 0;
  cmd.args = NULL;

  // Tokenize the buffer
  char *token = strtok(buffer, " ");
  if (token == NULL) {
    // Error handling for invalid command
    usageErr("Invalid command: No command provided\n");
  }

  // Determine the key based on the first token
  if (token[0] == '/') {
    cmd.key = strdup(token + 1);
  } else {
    cmd.key = strdup("message");
    // Allocate memory for args and add the first token
    cmd.args = malloc(sizeof(char *));
    if (cmd.args == NULL) {
      // Error handling for memory allocation failure
      errExit("Error allocating memory for args");
    }
    cmd.args[0] = strdup(token);
    cmd.argc = 1;
    return cmd;
  }

  // Allocate memory for args
  cmd.args = malloc(sizeof(char *));
  if (cmd.args == NULL) {
    // Error handling for memory allocation failure
    errExit("Error allocating memory for args");
  }

  // Tokenize remaining tokens and add them to args
  token = strtok(NULL, " ");
  while (token != NULL && cmd.argc < 10) {
    // Reallocate memory for args
    cmd.args = realloc(cmd.args, (cmd.argc + 1) * sizeof(char *));
    if (cmd.args == NULL) {
      // Error handling for memory allocation failure
      errExit("Error reallocating memory for args");
    }
    cmd.args[cmd.argc++] = strdup(token);
    token = strtok(NULL, " ");
  }

  return cmd;
}
