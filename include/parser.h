#ifndef PARSECOMMAND_H
#define PARSECOMMAND_H

#include "args.h"
#include "command.h"

Command createCommandFromBuffer(char *buffer);
unsigned char *serializeBuffer(char *buffer);

#endif
