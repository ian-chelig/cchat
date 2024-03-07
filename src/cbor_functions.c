#include <cbor.h>
#include <cbor/arrays.h>
#include <cbor/common.h>
#include <cbor/data.h>
#include <cbor/maps.h>
#include <cbor/strings.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "command.h"
#include "tlpi_hdr.h"

unsigned char *serializeData(size_t length, const cbor_item_t *item) {
  unsigned char *buffer = NULL;
  bool success = cbor_serialize_alloc(item, &buffer, &length);
  if (!success) {
    // Error handling for serialization failure
    errExit("Serialization failed");
  }
  return buffer;
}

cbor_item_t *deserializeData(size_t length, unsigned char *buffer) {
  struct cbor_load_result result;
  cbor_item_t *item = cbor_load(buffer, length, &result);
  if (item == NULL) {
    // Error handling for deserialization failure
    errExit("Deserialization failed");
  }
  return item;
}

cbor_item_t *createItemFromCommand(Command cmd) {
  cbor_item_t *map = cbor_new_definite_map(2);

  (void)cbor_map_add(
      map, (struct cbor_pair){.key = cbor_build_string("command"),
                              .value = cbor_build_string(cmd.command)});

  cbor_item_t *args_array = cbor_new_definite_array(cmd.argc);
  for (size_t i = 0; i < cmd.argc; i++) {
    (void)cbor_array_set(args_array, i, cbor_build_string(cmd.args[i]));
  }
  (void)cbor_map_add(map, (struct cbor_pair){.key = cbor_build_string("args"),
                                             .value = args_array});

  return map;
}

char **getKeysFromMap(cbor_item_t *buffer) {
  if (buffer == NULL || !cbor_isa_map(buffer)) {
    return NULL;
  }

  struct cbor_pair *pairs = cbor_map_handle(buffer);
  if (pairs == NULL) {
    errExit("Error accessing CBOR map");
  }

  size_t mapSize = cbor_map_size(buffer);
  char **dest = malloc(mapSize * sizeof(char *));
  if (dest == NULL) {
    errExit("Error allocating memory for keys");
  }

  for (size_t i = 0; i < mapSize; i++) {
    if (!cbor_isa_string(pairs[i].key)) {
      errExit("Expected string key in CBOR map");
    }
    unsigned char *str = cbor_string_handle(pairs[i].key);
    size_t strLen = cbor_string_length(pairs[i].key);
    dest[i] = malloc((strLen + 1) * sizeof(char));
    if (dest[i] == NULL) {
      errExit("Error allocating memory for key");
    }
    strcpy(dest[i], (const char *)str);
  }

  return dest;
}

Command createCommandFromItem(cbor_item_t *item) {
  Command cmd;
  struct cbor_pair *pairs = cbor_map_handle(item);
  size_t mapSize = cbor_map_size(item);
  cmd.argc = 0;
  cmd.args = NULL;
  cbor_describe(item, stdout);

  for (size_t i = 0; i < mapSize; i++) {
    if (!strncmp((char *)cbor_string_handle(pairs[i].key), "command", 7)) {
      cmd.command = strdup((const char *)cbor_string_handle(pairs[i].value));
    } else if (!strncmp((char *)cbor_string_handle(pairs[i].key), "args", 4)) {
      if (!cbor_isa_array(pairs[i].value)) { // better way to do this is to
                                             // return int as an err level
        errMsg("args field is not array!");  // pass in a pointer to a command
                                             // struct to be written to
        _exit(errno);
      }

      size_t arraySize = cbor_array_size(pairs[i].value);
      cmd.argc = arraySize;
      cmd.args = malloc(arraySize * sizeof(char *)); // Allocate memory for args
      if (cmd.args == NULL) {
        perror("Error allocating memory");
        exit(EXIT_FAILURE);
      }
      for (size_t j = 0; j < arraySize; j++) {
        unsigned char *arg =
            cbor_string_handle(cbor_array_get(pairs[i].value, j));
        cmd.args[j] = strdup((const char *)arg); // Copy the argument
      }
    }
  }
  return cmd;
}

char **getValueFromKey(cbor_item_t *map, char *key) {
  struct cbor_pair *pairs = cbor_map_handle(map);
  size_t mapSize = cbor_map_size(map);

  for (size_t i = 0; i < mapSize; i++) {
    if (cbor_isa_string(pairs[i].key)) {
      unsigned char *mapKey = cbor_string_handle(pairs[i].key);
      if (strcmp((const char *)mapKey, key) == 0) {
        if (cbor_isa_array(pairs[i].value)) {
          size_t arraySize = cbor_array_size(pairs[i].value);
          char **values = malloc(arraySize * sizeof(char *));
          if (values == NULL) {
            perror("Error allocating memory");
            exit(EXIT_FAILURE);
          }
          for (size_t j = 0; j < arraySize; j++) {
            unsigned char *arg =
                cbor_string_handle(cbor_array_get(pairs[i].value, j));
            values[j] = strdup((const char *)arg);
          }
          return values;
        }
      }
    }
  }
  return NULL;
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
