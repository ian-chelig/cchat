#ifndef PARSECOMMAND_H
#define PARSECOMMAND_H

#include <cbor.h>

#include "args.h"
#include "command.h"

int serializeBuffer(char *inBuffer, unsigned char **outBuffer);
int deserializeBuffer(unsigned char *inBuffer, Command **out_cmd);
int createCommandFromPlaintext(char *in_buffer, Command *out_cmd);
int plaintextFromMessageCMD(Command in_cmd, char *out_buffer);

#endif
