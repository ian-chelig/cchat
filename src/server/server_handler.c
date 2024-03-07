#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "command.h"
#include "server.h"

void handleServerCommand(Command cmd, fdNode_t clientNode) {
  if (strncmp(cmd.command, "message", 7)) {
    // prepend nickname and send it forward
  } else if (strncmp(cmd.command, "nick", 4)) {
    clientNode.user.nick = cmd.args[0];
  } else if (strncmp(cmd.command, "pubkey", 6)) {
    // unimplemented
    // accept public key and add it to user struct
  }
}
