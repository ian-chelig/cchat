#ifndef HANDLER_H
#define HANDLER_H

#include "command.h"
#include "server.h"

void handleServerCommand(Command cmd, fdNode_t clientNode);

#endif
