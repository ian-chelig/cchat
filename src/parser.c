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

char *deserializeBuffer(char *buffer) {
  cbor_item_t item =
      *deserializeData(strlen((const char *)buffer), (unsigned char *)buffer);
  Command cmd = createCommandFromItem(&item);
  char *nBuf = plaintextFromMessageCMD(cmd);
  return nBuf;
}

unsigned char *serializeBuffer(char *buffer) {
  Command cmd = createCommandFromPlaintext(buffer);
  cbor_item_t item = *createItemFromCommand(cmd);
  return serializeData(cmd.argc, &item);
}

char *plaintextFromMessageCMD(Command cmd) {
  size_t len = 0;
  for (int i = 0; i < cmd.argc; i++) {
    len += strlen(cmd.args[i]);
  }

  char *buffer = malloc(sizeof(char) * (len + 1));
  for (int i = 0; i < cmd.argc; i++) {
    strcat(buffer, cmd.args[i]);
  }
  return buffer;
}

/*
 * Takes a plaintext buffer and turns it into a command struct
 */
Command createCommandFromPlaintext(char *buffer) {
  Command cmd;
  cmd.argc = 0;
  cmd.args = NULL;
  char *temp = malloc(sizeof(char) * (strlen(buffer) + 1));
  strcpy(temp, buffer);

  char *token = strtok(buffer, " ");
  if (token == NULL) {
    perror("Invalid command");
    exit(EXIT_FAILURE);
  }

  if (token[0] == '/') {
    cmd.command = strdup(token + 1);
  } else {
    cmd.command = strdup("message");
    cmd.args = malloc(sizeof(char *));
    if (cmd.args == NULL) {
      perror("Error allocating memory");
      exit(EXIT_FAILURE);
    }
    cmd.args[0] = temp;
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
