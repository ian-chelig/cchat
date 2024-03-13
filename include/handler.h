#ifndef HANDLER_H
#define HANDLER_H

#include "command.h"
#include "server.h"

int handleCommand(Command cmd, user_t clientNode);

#endif
