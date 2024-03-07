#ifndef PARSECOMMAND_H
#define PARSECOMMAND_H

#include <cbor.h>

#include "args.h"
#include "command.h"

Command createCommandFromPlaintext(char *buffer);
unsigned char *serializeBuffer(char *buffer);
unsigned char *createBufferFromCommand(cbor_item_t item);
#endif
