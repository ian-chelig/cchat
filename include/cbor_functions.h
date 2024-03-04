#ifndef CBOR_H
#define CBOR_H

#include <cbor.h>
#include <stddef.h>

#include "command.h"

unsigned char *serializeData(size_t length, const cbor_item_t *item);
cbor_item_t *deserializeData(size_t length, unsigned char *buffer);
cbor_item_t *createItemFromCommand(Command cmd);
Command createCommandFromItem(cbor_item_t *item);
char **getKeysFromMap(cbor_item_t *buffer);
char **getValueFromKey(cbor_item_t *map, char *key);
void freeCommand(Command cmd);
void freeKeys(char **keys, size_t count);

#endif
