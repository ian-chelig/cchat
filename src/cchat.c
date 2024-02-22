#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>
#include <pthread.h>

#include "args.h"
#include "client.h"
#include "server.h"

void printUsage () {
  printf("Usage: cchat -p <port> [-s | -c <address>] [-h]");
  exit(2);
}

void processArgs (Args args) {
  if (args.s == 1)
    if (!args.h) {
      //setup own client here
      pthread_t clientThread;
      Args a = {
        .c = "127.0.0.1",
        .port = htons(args.port),
        .s = 0
      };
      pthread_create(&clientThread, NULL, clientThread, &a);
    }
    initServer(args);
  if (args.c != NULL)
    initClient(args);
}

Args handleArgs (Args args) {
  if (args.port == -1) 
    printUsage();
  if (args.c == NULL)
    args.s = 1;

  return args;
}

Args parseArgs (int argc, char **argv) {
  int opt;
  Args args = (Args) {.s = 0, .c = NULL, .port = -1, .h = 0};

  while((opt = getopt (argc, argv, ":sc:p:h")) != -1) {
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
        args.h = 1;
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
  for(; optind < argc; optind++) {
    printf("extra arguments: %s\n", argv[optind]);
    printUsage();
  }

  return args;
}

int main (int argc, char **argv) {
  Args args = parseArgs(argc, argv);
  args = handleArgs(args);
  processArgs(args);

  return 0;
}
