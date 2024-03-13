#include <cbor.h>
#include <cbor/arrays.h>
#include <cbor/common.h>
#include <cbor/data.h>
#include <cbor/maps.h>
#include <cbor/strings.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "command.h"

int serializeData(cbor_item_t *inItem, unsigned char **outBuffer) {
  size_t len = 0;
  if ((cbor_serialize_alloc(inItem, outBuffer, &len)) == 0) {
    printf("\nSerialization failed");
    fflush(stdout);
    return -1;
  }

  return 0;
}

int deserializeData(size_t len, char *inBuffer, cbor_item_t **outItem) {
  struct cbor_load_result cbor_result;

  if ((strlen(inBuffer)) == 0) {
    printf("\nReceived empty buffer!");
    fflush(stdout);
    return -1;
  }

  *outItem = cbor_load((unsigned char *)inBuffer, len, &cbor_result);
  if (*outItem == NULL) {
    printf("\nDeserialization failed");
    fflush(stdout);
    return -1;
  }

  return 0;
}

int createItemFromCommand(Command incmd, cbor_item_t *outItem) {
  int result = -1;
  cbor_item_t *args_array;

  result = cbor_map_add(
      outItem, (struct cbor_pair){.key = cbor_build_string("command"),
                                  .value = cbor_build_string(incmd.command)});
  if (result == 0) {
    printf("\nFailed to allocate command to cbor map");
    fflush(stdout);
    return -1;
  }

  args_array = cbor_new_definite_array(incmd.argc);
  if (args_array == NULL) {
    printf("\nFailed to allocate memory for args array");
    fflush(stdout);
    return -1;
  }

  for (size_t i = 0; i < incmd.argc; i++) {
    result = cbor_array_set(args_array, i, cbor_build_string(incmd.args[i]));
    if (result == 0) {
      printf("\nFailed to set args array");
      fflush(stdout);
      return -1;
    }
  }

  result =
      cbor_map_add(outItem, (struct cbor_pair){.key = cbor_build_string("args"),
                                               .value = args_array});
  if (result == 0) {
    printf("\nFailed to add pair to map");
    fflush(stdout);
    return -1;
  }

  return 0;
}

int getKeysFromMap(cbor_item_t *buffer, char **keys) {
  int result = -1;
  struct cbor_pair *pairs;
  size_t mapSize;

  if (keys == NULL) {
    printf("\nKeys is null!");
    fflush(stdout);
    return -1;
  }

  if (buffer == NULL || !cbor_isa_map(buffer)) {
    printf("\nBuffer is not a map!");
    fflush(stdout);
    return -1;
  }

  pairs = cbor_map_handle(buffer);
  if (pairs == NULL) {
    printf("\nError accessing CBOR map");
    fflush(stdout);
    return -1;
  }

  mapSize = cbor_map_size(buffer);
  if (mapSize < 1) {
    printf("\nMap size is less than 1!");
    fflush(stdout);
    return -1;
  }

  for (size_t i = 0; i < mapSize; i++) {
    size_t strLen;
    unsigned char *str;

    result = cbor_isa_string(pairs[i].key);
    if (result == 0) {
      printf("\nExpected string key in CBOR map");
      fflush(stdout);
      return -1;
    }

    str = cbor_string_handle(pairs[i].key);
    if (str == NULL) {
      printf("\nKey is not a string.");
      fflush(stdout);
      return -1;
    }

    strLen = cbor_string_length(pairs[i].key);
    if (strLen == 0) {
      printf("\nString length is null");
      fflush(stdout);
      return -1;
    }

    keys[i] = malloc((strLen + 1) * sizeof(char));
    if (keys[i] == NULL) {
      printf("\nError allocating memory for key");
      fflush(stdout);
      return -1;
    }

    strncpy(keys[i], (const char *)str, strLen);
    if (keys[i] == NULL) {
      printf("\nCopying string failed.");
      return -1;
    }
  }

  return 0;
}

int createCommandFromItem(cbor_item_t *inItem, Command *outCmd) {
  struct cbor_pair *pairs;
  size_t mapSize;

  pairs = cbor_map_handle(inItem);
  if (pairs == NULL) {
    printf("\nError accessing CBOR map");
    fflush(stdout);
    return -1;
  }

  mapSize = cbor_map_size(inItem);
  if (mapSize < 1) {
    printf("\nMap size is less than 1!");
    fflush(stdout);
    return -1;
  }
  outCmd->argc = 0;
  // outCmd->args = NULL;

  for (size_t i = 0; i < mapSize; i++) {
    if (!strncmp((char *)cbor_string_handle(pairs[i].key), "command", 7)) {
      outCmd->command =
          strdup((const char *)cbor_string_handle(pairs[i].value));
      if (outCmd->command == NULL) {
        printf("\nString duplication failed!");
        fflush(stdout);
        return -1;
      }
    } else if (!strncmp((char *)cbor_string_handle(pairs[i].key), "args", 4)) {
      size_t arraySize;

      if (!cbor_isa_array(pairs[i].value)) {
        printf("\nArgs field is not an array!");
        fflush(stdout);
        return -1;
      }

      arraySize = cbor_array_size(pairs[i].value);
      if (arraySize < 1) {
        printf("\nArray not found!");
        fflush(stdout);
        return -1;
      }

      outCmd->argc = arraySize;
      outCmd->args =
          malloc(arraySize * sizeof(char *)); // Allocate memory for args
      if (outCmd->args == NULL) {
        printf("\nError allocating memory for args!");
        fflush(stdout);
        return -1;
      }

      for (size_t j = 0; j < arraySize; j++) {
        unsigned char *arg;

        arg = cbor_string_handle(cbor_array_get(pairs[i].value, j));
        if (arg == NULL) {
          printf("\nValue is not a string!");
          fflush(stdout);
          return -1;
        }

        outCmd->args[j] = strdup((const char *)arg); // Copy the argument
        if (outCmd->args[j] == NULL) {
          printf("\nString duplication failed!");
          fflush(stdout);
          return -1;
        }
      }
    }
  }
  return 0;
}

int getValueFromKey(cbor_item_t *map, char *key, char **values) {
  struct cbor_pair *pairs;
  size_t mapSize;

  pairs = cbor_map_handle(map);
  if (pairs == NULL) {
    printf("\nError accessing CBOR map");
    fflush(stdout);
    return -1;
  }

  mapSize = cbor_map_size(map);
  if (mapSize < 1) {
    printf("\nMap size is less than 1!");
    fflush(stdout);
    return -1;
  }

  for (size_t i = 0; i < mapSize; i++) {
    size_t arraySize;
    unsigned char *mapKey;

    if (!cbor_isa_string(pairs[i].key)) {
      printf("Pair is not a string!");
      fflush(stdout);
      return -1;
    }

    if (!cbor_isa_array(pairs[i].value)) {
      printf("\nValue is not an array!");
      fflush(stdout);
      return -1;
    }

    arraySize = cbor_array_size(pairs[i].value);
    if (arraySize < 1) {
      printf("\nArray not found!");
      fflush(stdout);
      return -1;
    }

    mapKey = cbor_string_handle(pairs[i].key);
    if (mapKey == NULL) {
      printf("Map Key is not a string");
      fflush(stdout);
      return -1;
    }
    // If no match found
    if (strncmp((const char *)mapKey, key, sizeof(char) * strlen(key)) != 0) {
      printf("\nNo match found!");
      fflush(stdout);
      return -1;
    }

    for (size_t j = 0; j < arraySize; j++) {
      unsigned char *arg;

      values[j] = strdup((const char *)arg);
      if (values[j] == NULL) {
        printf("\nString duplication failed!");
        fflush(stdout);
        return -1;
      }
    }
  }
  return 0;
}

void freeCommand(Command cmd) {
  free(cmd.command);
  if (cmd.args != NULL) {
    for (size_t i = 0; i < cmd.argc; i++) {
      free(cmd.args[i]);
    }
    free(cmd.args);
  }
}

void freeKeys(char **keys, size_t count) {
  if (keys != NULL) {
    for (size_t i = 0; i < count; i++) {
      free(keys[i]);
    }
    free(keys);
  }
}
