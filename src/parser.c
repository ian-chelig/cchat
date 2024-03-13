#include <arpa/inet.h>
#include <cbor.h>
#include <cbor/maps.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "cbor_functions.h"
#include "command.h"
#include "parser.h"
#include "server.h"

int deserializeBuffer(unsigned char *inBuffer, Command **out_cmd) {
  cbor_item_t *item = NULL;

  if ((deserializeData(strlen((const char *)inBuffer), inBuffer, &item)) ==
      -1) {
    printf("\nFailed to deserialize buffer!");
    fflush(stdout);
    return -1;
  }

  if ((createCommandFromItem(item, *out_cmd)) == -1) {
    printf("\nFailed to create command!");
    fflush(stdout);
    return -1;
  }

  return 0;
}

int serializeBuffer(char *inBuffer, unsigned char **outBuffer) {
  cbor_item_t *item = cbor_new_definite_map(2);
  Command *cmd;

  if ((createCommandFromPlaintext(inBuffer, cmd)) == -1) {
    printf("\nFailed to create command!");
    fflush(stdout);
    return -1;
  }

  if ((createItemFromCommand(*cmd, item)) == -1) {
    printf("\nFailed to create item!");
    fflush(stdout);
    return -1;
  }

  if ((serializeData(item, outBuffer)) == -1) {
    printf("\nFailed to serialized item!");
    fflush(stdout);
    return -1;
  }
  return 0;
}

int plaintextFromMessageCMD(Command in_cmd, char *out_buffer) {
  if ((strncpy(out_buffer, in_cmd.args[0], strlen(in_cmd.args[0]))) == NULL) {
    printf("\nCould not copy string to buffer!");
    fflush(stdout);
    return -1;
  }

  return 0;
}

/*
 * Takes a plaintext buffer and turns it into a command struct
 */
int createCommandFromPlaintext(char *in_buffer, Command *out_cmd) {
  char *token;
  char *temp = malloc(sizeof(char) * (strlen(in_buffer) + 1));
  out_cmd->argc = 0;
  out_cmd->args = NULL;

  strncpy(temp, in_buffer, sizeof(char) * strlen(in_buffer));
  if (temp == NULL) {
    printf("\nCopying string failed.");
    fflush(stdout);
    return -1;
  }

  token = strtok(in_buffer, " ");
  /*if (token == NULL) {
    printf("\nTokenization failed!");
    return -1;
  }*/

  if (token[0] == '/') {
    out_cmd->command = strdup(token + 1);
    if (out_cmd->command == NULL) {
      printf("\nString duplication failed!");
      fflush(stdout);
      return -1;
    }
  } else {
    out_cmd->command = strdup("message");
    if (out_cmd->command == NULL) {
      printf("\nString duplication failed!");
      fflush(stdout);
      return -1;
    }

    out_cmd->args = malloc(sizeof(char *));
    out_cmd->args[0] = temp;
    out_cmd->argc = 1;
    return 0;
  }

  out_cmd->args = malloc(sizeof(char *));
  token = strtok(NULL, " ");
  /*if (token == NULL) {
    printf("\nTokenization failed!");
    return -1;
  }*/

  while (token != NULL && out_cmd->argc < 10) { //...10? really?
    out_cmd->args =
        realloc(out_cmd->args, (out_cmd->argc + 1) * sizeof(char *));
    out_cmd->args[out_cmd->argc++] = strdup(token);
    token = strtok(NULL, " ");
    /*if (token == NULL) {
      printf("\nTokenization failed!");
      return -1;
    }*/
  }

  return 0;
}
