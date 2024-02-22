#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>

typedef struct Args {
  int s;
  char *port, *c;
} Args;

void initClient(Args args) {
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in address = {
    AF_INET,
    htons((int) args.port), // Error check this!
    0
  };

  connect(sockfd, &address, sizeof(address));
}

void initServer(Args args) {
  printf("Initializing Server");
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in address = {
    AF_INET,
    htons(1000), // Error check this!
    0
  };
  
  bind (sockfd, &address, sizeof(address));
  listen(sockfd, 10);
  int clientfd = accept(sockfd, 0, 0);
  
  // stdin - 0
  struct pollfd fds[2] = {
    {
      0,
      POLLIN,
      0
    },
    {
      clientfd,
      POLLIN,
      0
    }
  };

  for (;;) {
    char buffer[256] = { 0 };

    poll(fds, 2, 50000);

    if (fds[0].revents & POLLIN) {
      read(0, buffer, 255);
      send(clientfd, buffer, 255, 0);
    } else if (fds[1].revents & POLLIN) {
      if (recv(clientfd, buffer, 255, 0) == 0)
        return;
      printf("%s\n", buffer);
    }
  }
}

void printUsage () {
  printf("Usage: cchat -p <port> [-s | -c <address>]");
  exit(2);
}

void processArgs (Args args) {
  printf("%d\n", args.s);
  if (args.s == 1)
    initServer(args);
  if (args.c != '\0')
    printf("init client");

}

Args handleArgs (Args args) {
  if (args.port == '\0') 
    printUsage();
  if (args.c == '\0')
    args.s = 1;

  return args;
}

Args parseArgs (int argc, char **argv) {
  int opt;
  Args args = (Args) {.s = 0, .c = '\0', .port = '\0'};

  while((opt = getopt (argc, argv, ":sc:p:")) != -1) {
    switch (opt) {
      case 's':
        args.s = 1;
        break;
      case 'c':
        args.c = optarg;
        break;
      case 'p':
        args.port = optarg;
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
