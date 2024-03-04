#include <arpa/inet.h>
#include <cbor.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "cbor_functions.h"
#include "command.h"
#include "parser.h"
#include "server.h"
#include "tlpi_hdr.h"
/*
unsigned char *deserializeBuffer(char *buffer) {
  cbor_item_t item =
      *deserializeData(strlen((const char *)buffer), (unsigned char *)buffer);
  Command cmd = createCommandFromItem(&item);
  // createBufferFromCommand and return it
}
*/

unsigned char *serializeBuffer(char *buffer) {
  Command cmd = createCommandFromBuffer(buffer);
  cbor_item_t item = *createItemFromCommand(cmd);
  return serializeData(cmd.argc, &item);
}

Command createCommandFromBuffer(char *buffer) {
  Command cmd;
  cmd.argc = 0;
  cmd.args = NULL;

  char *token = strtok(buffer, " ");
  if (token == NULL) {
    perror("Invalid command");
    exit(EXIT_FAILURE);
  }

  if (token[0] == '/') {
    cmd.key = strdup(token + 1);
  } else {
    cmd.key = strdup("message");
    cmd.args = malloc(sizeof(char *));
    if (cmd.args == NULL) {
      perror("Error allocating memory");
      exit(EXIT_FAILURE);
    }
    cmd.args[0] = strdup(token);
    cmd.argc = 1;
    return cmd;
  }

  cmd.args = malloc(sizeof(char *));
  token = strtok(NULL, " ");
  while (token != NULL && cmd.argc < 10) {
    cmd.args = realloc(cmd.args, (cmd.argc + 1) * sizeof(char *));
    if (cmd.args == NULL) {
      perror("Error allocating memory");
      exit(EXIT_FAILURE);
    }
    cmd.args[cmd.argc++] = strdup(token);
    token = strtok(NULL, " ");
  }

  return cmd;
}
