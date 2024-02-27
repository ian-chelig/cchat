#include <arpa/inet.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "parseCommand.h"
#include "server.h"

void parseCommand(char *buffer, int sockfd) {
  if (!strncmp(buffer, "/nick", 5)) {
  } else if (!strncmp(buffer, "/stamp", 6)) {
  }
}
