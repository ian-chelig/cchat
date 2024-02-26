#include <arpa/inet.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../include/args.h"
#include "../include/client.h"
#include "../include/server.h"

void printUsage() {
  printf("Usage: cchat -p <port> [-s | -c <address>] [-h] [-d] [-u]");
  exit(2);
}

void processArgs(Args args) {
  if (args.s == 1) {
    if (!args.d) {
      // setup own client here
      pthread_t clientThread;
      Args a = {.c = "127.0.0.1", .port = args.port, .s = 0, .u = args.u};
      pthread_create(&clientThread, NULL, setupLocalClient, &a);
    }
    initServer(args);
  }
  if (args.c != NULL) {
    initClient(args);
  }
}

Args handleArgs(Args args) {
  if (args.port == -1)
    printUsage();
  if (args.c == NULL)
    args.s = 1;
  if (args.u == NULL)
    args.u = "guest";

  return args;
}

Args parseArgs(int argc, char **argv) {
  int opt;
  Args args = (Args){.s = 0, .c = NULL, .port = -1, .d = 0, .u = NULL};

  while ((opt = getopt(argc, argv, ":shdc:p:u:")) != -1) {
    switch (opt) {
    case 's':
      args.s = 1;
      break;
    case 'c':
      args.c = optarg;
      break;
    case 'p':
      sscanf(optarg, "%hd", &args.port);
      break;
    case 'h':
      break;
    case 'd':
      args.d = 1;
      break;
    case 'u':
      args.u = optarg;
      break;
    case ':':
      printf("%c option needs a value\n", optopt);
      printUsage();
      break;
    case '?':
      printf("unknown option: %c\n", optopt);
      printUsage();
      break;
    }
  }

  // optind is for extra arguments which are not parsed
  for (; optind < argc; optind++) {
    printf("extra arguments: %s\n", argv[optind]);
    printUsage();
  }

  return args;
}

int main(int argc, char **argv) {
  Args args = parseArgs(argc, argv);
  args = handleArgs(args);
  processArgs(args);

  return 0;
}
