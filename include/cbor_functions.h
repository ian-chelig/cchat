#ifndef CBOR_H
#define CBOR_H

#include <cbor.h>
#include <cbor/data.h>
#include <stddef.h>

#include "command.h"

int serializeData(cbor_item_t *inItem, unsigned char **outBuffer);
int deserializeData(size_t len, unsigned char *inBuffer, cbor_item_t **outItem);
int createItemFromCommand(Command incmd, cbor_item_t *outItem);
int getKeysFromMap(cbor_item_t *buffer, char *keys);
int getValueFromKey(cbor_item_t *map, char *key, char *values);
int createCommandFromItem(cbor_item_t *inItem, Command *outCmd);
void freeCommand(Command cmd);
void freeKeys(char **keys, size_t count);

#endif
