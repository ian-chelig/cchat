#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void handleCommand(char *buffer, int sockfd) {
  char nick[255] = {0};
  sscanf(buffer, "/nick %s", nick);
  char *tmp = malloc(strlen(nick) + 1);
  strcpy(tmp, nick);
  // current->clientName = tmp;
}
