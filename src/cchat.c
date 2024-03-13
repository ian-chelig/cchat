#include <arpa/inet.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "args.h"
#include "client.h"
#include "server.h"

void printHelp() {
  printf("\nOptions");
  printf("\n-p [port]                 Mandatory. Specify port to open on.");
  printf("\n-c [address]              Specify opening in client mode, "
         "attempting to connect to address given.");
  printf(
      "\n-s                        Default. Specify running in server mode.");
  printf("\n-d                        Run server in daemon mode.");
  printf("\n-u [nickname]             Set nickname to value upon connection");
  printf("\n-h                        Display this help message.");
  fflush(stdout);
  exit(0);
}

void printUsage() {
  printf("Usage: cchat -p <port> [-s | -c <address>] [-h] [-d] [-u]");
  printHelp();
}

void processArgs(Args args) {
  if (args.port == -1)
    printUsage();
  if (args.u == NULL)
    args.u = "guest";
  if (args.c == NULL)
    args.s = 1;
  else
    initClient(args);

  if (args.s == 1) {
    if (args.d == 0) {
      // setup own client here
      // error check and handle the thread creation here.
      pthread_t clientThread;
      Args a = {.c = "127.0.0.1", .port = args.port, .s = 0, .u = args.u};
      pthread_create(&clientThread, NULL, setupLocalClient, &a);
    }
    initServer(args);
  }
}

int main(int argc, char **argv) {
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
      printUsage();
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

  processArgs(args);
  return 0;
}
