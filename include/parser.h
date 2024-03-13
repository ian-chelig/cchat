#ifndef PARSECOMMAND_H
#define PARSECOMMAND_H

#include <cbor.h>

#include "args.h"
#include "command.h"

Command createCommandFromPlaintext(char *buffer);
unsigned char *serializeBuffer(char *buffer);
char *plaintextFromMessageCMD(Command cmd);
char *deserializeBuffer(char *buffer);

#endif
