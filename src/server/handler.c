#include <cbor/data.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cbor_functions.h"
#include "command.h"
#include "parser.h"
#include "server.h"

/* Takes a Command and possibly a user_t
 * Returns 0 if able to handle command
 * -1 otherwise
 *
 * If user_t is NULL it is assumed this function was called
 * from a client and not a server.
 */

int handleCommand(Command cmd, user_t *user) {
  int result = -1;
  char *nBuf;
  unsigned char *oBuf;

  if (user == NULL) { // Handled by client
    if (strncmp(cmd.command, "message", 7) == 0) {
      // createitemfromcommand
      // serialize and send it
    } else if (strncmp(cmd.command, "stamp", 5) == 0) {
      // prepend time
    }
  } else if (user != NULL) { // Handled by server
    if (strncmp(cmd.command, "message", 7) == 0) {
      result = plaintextFromMessageCMD(cmd, nBuf);
      if (result == -1) {
      }
      // prependString(&cmd, &user);
      result = serializeBuffer(nBuf, &oBuf);
      if (result == -1) {
      }
    } else if (strncmp(cmd.command, "nick", 4) == 0) {
      user->nick = cmd.args[0];
    } else if (strncmp(cmd.command, "pubkey", 6) == 0) {
      // unimplemented
      // accept public key and add it to user struct
    }
  } else if (strncmp(cmd.command, "help", 4) == 0) {
    // print help
  } else {
    printf("\nInvalid command: %s", cmd.command);
    printf("\"/help\" for mor information!");
    return -1;
  }
  return 0;
}

int prependUsername(Command *cmd, user_t *user) {
  if (cmd->args == NULL) {
    printf("No buffer to prepend to!");
    return -1;
  }
  return 0;
}
